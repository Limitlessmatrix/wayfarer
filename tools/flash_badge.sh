#!/usr/bin/env bash
# Wait for a badge on USB, confirm its serial, flash build/badge/Wayfarer.bin, verify, reboot.
# usage: tools/flash_badge.sh [expected_serial] [timeout_s]
set -euo pipefail
source ~/badge/env.sh
W=~/badge/wayfarer
BIN=$W/build/badge/Wayfarer.bin
EXPECT=${1:-}
TIMEOUT=${2:-600}
for ((i=0; i<TIMEOUT; i++)); do
  if [ -e /dev/ttyACM0 ]; then
    SER=$(udevadm info -q property /dev/ttyACM0 2>/dev/null | sed -n 's/^ID_SERIAL_SHORT=//p')
    echo "badge on USB serial=$SER"
    if [ -n "$EXPECT" ] && [ "$SER" != "$EXPECT" ]; then echo "serial mismatch, expected $EXPECT, not flashing"; exit 2; fi
    sleep 1
    picotool load "$BIN" -o 0x10000000 -f
    picotool verify "$BIN" -o 0x10000000 2>&1 | tr '\r' '\n' | tail -1
    picotool reboot
    echo "flashed and rebooted"
    exit 0
  fi
  sleep 1
done
echo "timed out waiting for the badge"; exit 1
