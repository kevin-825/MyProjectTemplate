#!/bin/bash

img="./build/riscv32/program_riscv32_v1.0.0"
dbg=""



run_riscv32_qemu() {
    qemu-system-riscv32 -nographic \
    -machine virt \
    -net none \
    -chardev stdio,id=con,mux=on \
    -serial chardev:con \
    -mon chardev=con,mode=readline \
    -bios none \
    $dbg -s \
    --kernel $img
    #-gdb tcp::1234,server,nowait \
}

run_riscv64_qemu() {
qemu-system-riscv64 -nographic -machine virt -net none -chardev stdio,id=con,mux=on \
    -serial chardev:con -mon chardev=con,mode=readline \ 
    -cpu rv64,zba=true,zbb=true,v=true,vlen=256,vext_spec=v1.0,rvv_ta_all_1s=true,rvv_ma_all_1s=true \
    $dbg -s \
    -bios $img 
    #--kernel $img
        #-gdb tcp::1234,server,nowait \
}

# -----------------------------
# Argument parsing
# -----------------------------
parse_args(){
    while [[ $# -gt 0 ]]; do
        case "$1" in
            -i|--image)
                img="$2"
                shift 2
                ;;
            -d|--debug)
                dbg="-S"
                shift
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                echo "Unknown option: $1" >&2
                exit 0
                ;;
        esac
    done
}

# -----------------------------
# Main function (Python style)
# -----------------------------
main() {
    parse_args "$@"

    echo "img: $img"
    echo ""
    #echo "dbg=$dbg"
    machine=$( readelf -h "$img" | grep 'Machine:' | awk '{print $2}')
    arch_class=$(readelf -h "$img" | awk '/Class:/ {print $2}')
    
    #run qemu based on target machine and class in elf
    case "$machine" in
        *RISC-V*)
            case "$arch_class" in
                *ELF32*)
                    echo "Running RISC-V 32 QEMU..."
                    run_riscv32_qemu
                    ;;
                *ELF64*)
                    echo "Running RISC-V 64 QEMU..."
                    run_riscv64_qemu
                    ;;
                *)
                    echo "Unsupported architecture class: $arch_class"
                    exit 1
                    ;;
            esac
            ;;
        *)
            echo "Unsupported machine: $machine"
            exit 1
            ;;
    esac
}

# -----------------------------
# Entry point
# -----------------------------
main "$@"

