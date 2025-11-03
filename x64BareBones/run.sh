#!/bin/bash

IMG="Image/x64BareBonesImage.qcow2"
MEM=512

COMMON_ARGS=(
    -m "$MEM"
    -drive file="$IMG",if=ide,format=qcow2
    -device isa-debug-exit,iobase=0xf4,iosize=0x04
)

if [[ ! -f "$IMG" ]]; then
    echo "No se encontró la imagen $IMG" >&2
    exit 1
fi

try_qemu() {
    local label=$1
    shift
    echo "Intentando QEMU con $label..."
    "$@"
    local exit_code=$?
    # isa-debug-exit retorna (valor << 1) | 1
    # Valor 0 enviado por outb(0xF4, 0x00) resulta en código de salida 1
    if [[ $exit_code -eq 0 || $exit_code -eq 1 ]]; then
        exit 0
    fi
    echo "Falló el arranque con $label (código: $exit_code)" >&2
}

OS=$(uname -s)

if [[ "$OS" == "Darwin" ]]; then
    try_qemu "audio CoreAudio (macOS)" \
        qemu-system-x86_64 \
            "${COMMON_ARGS[@]}" \
            -machine pc,pcspk-audiodev=core0 \
            -audiodev coreaudio,id=core0
fi

if [[ "$OS" == "Linux" ]]; then
    PULSE_SOCKET_DEFAULT="unix:/mnt/wslg/PulseServer"
    if [[ -z "${PULSE_SERVER}" && -S /mnt/wslg/PulseServer ]]; then
        export PULSE_SERVER="$PULSE_SOCKET_DEFAULT"
    fi

    try_qemu "audio SDL (Linux/WSL via PulseAudio)" \
        env SDL_AUDIODRIVER=pulseaudio \
            qemu-system-x86_64 \
                "${COMMON_ARGS[@]}" \
                -audiodev sdl,id=snd0 \
                -machine pc,pcspk-audiodev=snd0

    if command -v pactl >/dev/null 2>&1; then
        try_qemu "audio PulseAudio" \
            qemu-system-x86_64 \
                "${COMMON_ARGS[@]}" \
                -audiodev pa,id=pa0,out.frequency=44100,out.channels=2 \
                -machine pc,pcspk-audiodev=pa0
    fi
fi

echo "Audio no disponible, ejecutando sin audio..."
exec qemu-system-x86_64 "${COMMON_ARGS[@]}"