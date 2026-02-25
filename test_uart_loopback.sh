#!/bin/bash
# Quick loopback test to verify UART is working at all

echo "=== UART1 Loopback Test ==="
echo "This test writes to UART and tries to read it back"
echo "(Only works if TX and RX are connected together or in loopback mode)"
echo ""

# Enable local loopback (if supported)
stty -F /dev/ttyS1 local loopback 2>/dev/null || true

echo "Sending test data..."
echo "HELLO_WORLD" > /dev/ttyS1 &
sleep 0.2

echo "Attempting to read response..."
timeout 1 cat /dev/ttyS1 &
READER_PID=$!
sleep 0.5
kill $READER_PID 2>/dev/null

echo ""
echo "If you saw 'HELLO_WORLD' above, the UART is working!"
echo "If not, the TX/RX might be crossed or reversed."
echo ""
echo "=== Wiring Check ==="
echo "BeagleBone P8 Header:"
echo "  P8_31 (GPIO0_10) = UART1_RX  → Should connect to Core2530 RX"
echo "  P8_32 (GPIO0_11) = UART1_TX  → Should connect to Core2530 TX"
echo ""
echo "If Core2530 is not responding, try REVERSING these connections:"
echo "  P8_31 → Core2530 TX"
echo "  P8_32 → Core2530 RX"
