#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   Core2530 Module Connection Troubleshoot                  ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

PORT="/dev/ttyS1"
BAUD="115200"

echo "Using: $PORT at $BAUD baud"
echo ""

# Clear port
stty -F $PORT raw -echo 2>/dev/null

echo "STEP 1: Visual Inspection"
echo "─────────────────────────────────────────────────────────────"
echo "□ Is Core2530 module physically connected to P8_31/P8_32?"
echo "□ Does Core2530 have a power LED? Is it ON?"
echo "□ Check for visible damage or loose connections"
echo ""
read -p "Press Enter to continue testing..."
echo ""

echo "STEP 2: Test AT Command (Simple)"
echo "─────────────────────────────────────────────────────────────"
echo "Sending: AT"
(echo -ne "AT\r" > $PORT) 2>&1
sleep 0.5

echo "Reading response (timeout 2s)..."
timeout 2 cat $PORT | od -c | head -20 || echo "(no response)"
echo ""

echo "STEP 3: Test with different line ending (AT+VER)"
echo "─────────────────────────────────────────────────────────────"
echo "Sending: AT+VER\\r\\n"
(printf "AT+VER\r\n" > $PORT) 2>&1
sleep 0.5

echo "Reading response (timeout 2s)..."
timeout 2 cat $PORT | od -c | head -20 || echo "(no response)"
echo ""

echo "STEP 4: Monitor Port for ANY Activity"
echo "─────────────────────────────────────────────────────────────"
echo "Listening for 5 seconds... (press Ctrl+C to stop)"
echo "If you see anything, the module IS connected!"
echo ""
timeout 5 cat $PORT | od -c || true
echo ""

echo "STEP 5: Try Different TX/RX Connections"
echo "─────────────────────────────────────────────────────────────"
echo "If nothing received so far, TX/RX might be REVERSED."
echo ""
echo "Current Wiring:"
echo "  BeagleBone P8_31 (UART RX) ← Core2530 TX"
echo "  BeagleBone P8_32 (UART TX) → Core2530 RX"
echo ""
echo "Try Swapping:"
echo "  BeagleBone P8_31 (UART RX) ← Core2530 RX"  
echo "  BeagleBone P8_32 (UART TX) → Core2530 TX"
echo ""
read -p "After swapping, press Enter to test again..."
echo ""

echo "STEP 6: Test After Swap"
echo "─────────────────────────────────────────────────────────────"
(printf "AT\r\n" > $PORT) 2>&1
sleep 0.5
timeout 2 cat $PORT | od -c | head -20 || echo "(still no response)"
echo ""

echo "╔════════════════════════════════════════════════════════════╗"
echo "║ NEXT STEPS                                                 ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "If you see any response:"
echo "  ✓ Connection is working! Update the code with correct settings"
echo ""
echo "If NO response even after swapping:"
echo "  1. Verify Core2530 has power (check with multimeter: 3.3V)"
echo "  2. Verify GND connection (continuity between BBB and module)"
echo "  3. Check serial cable for breaks (try different cable)"
echo "  4. Module might need reset pulse or special initialization"
echo "  5. Try with minicom for interactive testing:"
echo "     sudo apt install minicom"
echo "     sudo minicom -D /dev/ttyS1 -b 115200"
echo ""
