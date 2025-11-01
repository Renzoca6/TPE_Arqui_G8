#!/bin/bash
set -e

IMG="Image/x64BareBonesImage.qcow2"
MEM=512

qemu-system-x86_64 \
  -m "$MEM" \
  -drive file="$IMG",if=ide,format=qcow2 \
  -audiodev sdl,id=snd0 \
  -machine pcspk-audiodev=snd0