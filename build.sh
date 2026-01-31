#!/usr/bin/env bash
set -euo pipefail

ARCH_TARGET=""
VERBOSE=0
DRYRUN=0
JSONLOG=0
CLEAN=0
INC_VERSION=0
SIMULATE=0
DEBUG=0
LINKER_SCRIPT=""
CONFIG_FILE="./config/config.json"

timestamp() {
    date -Iseconds
}

usage() {
    cat <<EOF
Usage: $0 -t <target> [options]

Options:
  -t, --target <name>   Build target (required)
  -c, --clean           Clean before building
  --inc                 Increment build number after successful build
  -v, --verbose         Enable verbose output
  --dry-run             Show actions without executing them
  --json-log            Output logs in JSON format
  -d,--debug            Force debug mode (overrides profile)
  -l,--ldscript <file>  Override linker script
  -h, --help            Show this help message
EOF
}

log() {
    if [[ $JSONLOG -eq 1 ]]; then
        json_log "info" "$1"
    else
        echo "[LOG] $1"
    fi
}

dry() {
    if [[ $JSONLOG -eq 1 ]]; then
        json_log "dryrun" "$1"
    else
        echo "[DRY-RUN] $1"
    fi
}

json_log() {
    local level="$1"
    local msg="$2"
    local ts
    ts=$(timestamp)

    printf '{"timestamp":"%s","level":"%s","target":"%s","message":"%s"}\n' \
        "$ts" "$level" "$ARCH_TARGET" "$msg"
}

run() {
    local cmd="$*"
    if [[ $DRYRUN -eq 1 ]]; then
        dry "Would run: $cmd"
    else
        [[ $VERBOSE -eq 1 ]] && echo "[EXEC] $cmd"
        eval "$cmd"
    fi
}

increment_build_number() {
    local current
    current=$(jq -r ".version.build_number" "$CONFIG_FILE")
    local next=$((current + 1))

    jq ".version.build_number = $next" "$CONFIG_FILE" > "$CONFIG_FILE.tmp"
    mv "$CONFIG_FILE.tmp" "$CONFIG_FILE"

    log "Incremented build number: $current → $next"
}

clean_pipeline() {
    log "Cleaning build artifacts for target: $ARCH_TARGET"

    local build_dir
    build_dir=$(jq -r ".build_dir" "$CONFIG_FILE")

    export BUILD_DIR="$build_dir"
    export ARCH="$ARCH_TARGET"

    run "make clean"
}

build_pipeline() {
    log "Loading configuration for target: $ARCH_TARGET"

    local arch_json
    arch_json=$(jq -r ".architectures.\"$ARCH_TARGET\"" "$CONFIG_FILE")

    # Toolchain
    local tool
    tool=$(echo "$arch_json" | jq -r ".toolchain")

    # Base flags
    local cflags ldflags
    cflags=$(echo "$arch_json" | jq -r '.cflags[]' | xargs)
    ldflags=$(echo "$arch_json" | jq -r '.ldflags[]' | xargs)

    # -----------------------------
    # Load profile + global features
    # -----------------------------
    local profile_name
    profile_name=$(jq -r ".build_profile" "$CONFIG_FILE")

    local profile_json
    profile_json=$(jq -r ".profiles.\"$profile_name\"" "$CONFIG_FILE")

    local p_debug p_opt p_lto p_save p_jobs p_sanitize p_verbose p_cov
    p_debug=$(echo "$profile_json" | jq -r ".debug")
    p_opt=$(echo "$profile_json" | jq -r ".optimization")
    p_lto=$(echo "$profile_json" | jq -r ".lto")
    p_save=$(echo "$profile_json" | jq -r ".save_temps")
    p_jobs=$(echo "$profile_json" | jq -r ".parallel_jobs")
    p_sanitize=$(echo "$profile_json" | jq -r ".sanitize")
    p_verbose=$(echo "$profile_json" | jq -r ".verbose")
    p_cov=$(echo "$profile_json" | jq -r ".coverage")

    # Verbose mode from profile
    if [[ "$p_verbose" == "true" ]]; then
        VERBOSE=1
    fi

    # Optimization
    cflags="$cflags -${p_opt}"

    # Debug (profile OR CLI)
    if [[ "$p_debug" == "true" || $DEBUG -eq 1 ]]; then
        cflags="$cflags -g -DDEBUG"
        log "Debug enabled (profile or CLI)"
    fi

    # LTO
    if [[ "$p_lto" == "true" ]]; then
        cflags="$cflags -flto"
        ldflags="$ldflags -flto"
        log "LTO enabled"
    fi

    # Save temps
    if [[ "$p_save" == "true" ]]; then
        cflags="$cflags --save-temps"
        log "Save-temps enabled"
    fi

    # Sanitizer (local only)
    if [[ "$p_sanitize" == "address" && "$ARCH_TARGET" == "local" ]]; then
        cflags="$cflags -fsanitize=address"
        ldflags="$ldflags -fsanitize=address"
        log "AddressSanitizer enabled for local build"
    fi

    # Coverage (local only)
    if [[ "$p_cov" == "true" && "$ARCH_TARGET" == "local" ]]; then
        cflags="$cflags --coverage"
        ldflags="$ldflags --coverage"
        log "Coverage instrumentation enabled"
    fi

    # Parallel jobs
    if [[ "$p_jobs" == "auto" || "$p_jobs" == "max" ]]; then
        JOBS=$(nproc)
    else
        JOBS="$p_jobs"
    fi

    # Linker script
    local linker_script
    linker_script=$(echo "$arch_json" | jq -r '.linker_script // empty')
    if [[ -n "$linker_script" ]]; then
        ldflags="$ldflags -Wl,-T,$linker_script"
    fi

    if [[ -n "$LINKER_SCRIPT" ]]; then
        ldflags="$ldflags -Wl,-T,$LINKER_SCRIPT"
        log "Using linker script override: $LINKER_SCRIPT"
    fi

    # Export toolchain
    export CC="${tool}gcc"
    export AS="${tool}as"
    export SIZE="${tool}size"
    export OBJDUMP="${tool}objdump"

    export CFLAGS="$cflags"
    export LDFLAGS="$ldflags"

    export BUILD_DIR
    BUILD_DIR=$(jq -r ".build_dir" "$CONFIG_FILE")
    export ARCH="$ARCH_TARGET"

    # Build name
    local build_name_prefix major minor build
    build_name_prefix=$(jq -r ".build_name_prefix" "$CONFIG_FILE")
    major=$(jq -r ".version.major" "$CONFIG_FILE")
    minor=$(jq -r ".version.minor" "$CONFIG_FILE")
    build=$(jq -r ".version.build_number" "$CONFIG_FILE")

    export TARGET="${build_name_prefix}_${ARCH_TARGET}_${profile_name}_v${major}.${minor}.${build}"


    log "Building with $JOBS parallel jobs"
    run "make -j${JOBS}"
    echo""
    echo""
    echo "Build completed: $BUILD_DIR/$ARCH/$TARGET"
    echo""
    echo""
    ./run.sh -i $BUILD_DIR/$ARCH/$TARGET
    ./run.sh -d -i $BUILD_DIR/$ARCH/$TARGET
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -t|--target) ARCH_TARGET="$2"; shift 2 ;;
            -c|--clean) CLEAN=1; shift ;;
            --inc) INC_VERSION=1; shift ;;
            -v|--verbose) VERBOSE=1; shift ;;
            --dry-run) DRYRUN=1; shift ;;
            --json-log) JSONLOG=1; shift ;;
            -d|--debug) DEBUG=1; shift ;;
            -l|--ldscript) LINKER_SCRIPT="$2"; shift 2 ;;
            -h|--help) usage; exit 0 ;;
            *) echo "Unknown option: $1"; usage; exit 1 ;;
        esac
    done
}

main() {
    parse_args "$@"

    if [[ -z "$ARCH_TARGET" ]]; then
        ARCH_TARGET=$(jq -r ".default_target // empty" "$CONFIG_FILE")
        if [[ -z "$ARCH_TARGET" ]]; then
            echo "Error: No target specified and no default_target in config.json"
            exit 1
        fi
        log "Using default target from config.json: $ARCH_TARGET"
    fi


    if ! jq -e ".architectures.\"$ARCH_TARGET\"" "$CONFIG_FILE" >/dev/null; then
        echo "Error: Invalid target '$ARCH_TARGET'"
        jq -r ".architectures | keys[]" "$CONFIG_FILE"
        exit 1
    fi

    [[ $CLEAN -eq 1 ]] && clean_pipeline
    build_pipeline
    [[ $INC_VERSION -eq 1 && $DRYRUN -eq 0 ]] && increment_build_number
}

main "$@"
