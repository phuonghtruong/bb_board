#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   Hardware Connection Verification                         ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

echo "CRITICAL CHECKS (Do these first!):"
echo "═════════════════════════════════════════════════════════════"
echo ""
echo "1. POWER CHECK - Is the Core2530 getting 3.3V?"
echo "   ─────────────────────────────────────────────"
echo "   Use a multimeter to measure:"
echo ""
echo "   ✓ Measure VCC pin on Core2530: Should show 3.3V"
echo "   ✓ Measure GND pin on Core2530: Should show 0V"
echo "   ✓ Voltage difference: Should be exactly 3.3V"
echo ""
echo "   IF NO POWER:"
echo "   • Check BeagleBone P9_3 or P9_4 has 3.3V output"
echo "   • Check wiring from BeagleBone to Core2530 VCC"
echo "   • Check for loose connections or broken wire"
echo ""

echo "2. GROUND CHECK - Are grounds connected?"
echo "   ─────────────────────────────────────────────"
echo "   Use a multimeter in CONTINUITY mode:"
echo ""
echo "   ✓ Test between BeagleBone GND (P8_1 or P8_2)"
echo "   ✓ And Core2530 GND pin"
echo "   ✓ Should beep (show continuity)"
echo ""
echo "   IF NO CONTINUITY:"
echo "   • Check GND wiring is connected"
echo "   • Test with different cable"
echo "   • Check for broken solder joints"
echo ""

echo "3. SERIAL DATA CHECK - Can you see TX signal?"
echo "   ─────────────────────────────────────────────"
echo "   If you have oscilloscope/logic analyzer:"
echo "   • Probe P8_32 (UART TX from BeagleBone)"
echo "   • Should see signal pulses when we send AT"
echo "   • Signal should swing between 0V and 3.3V"
echo ""

echo "4. MODULE STATUS - Is the module responding at all?"
echo "   ─────────────────────────────────────────────────"
echo "   Try these commands:"
echo ""

PORT="/dev/ttyS1"

# Clear any pending data
stty -F $PORT raw -echo 2>/dev/null
sleep 0.5

# Test 1: Send just a carriage return
echo "   Testing with just CR (Enter key)..."
(printf "\r" > $PORT)
sleep 0.3
timeout 1 cat $PORT | wc -c
echo "   (If you see a number > 0, module is alive)"
echo ""

# Test 2: Try break signal
echo "   Sending BREAK signal to wake module..."
(stty -F $PORT break; sleep 0.1; stty -F $PORT -break) 2>/dev/null
sleep 0.5
timeout 1 cat $PORT | wc -c
echo "   (If you see a number > 0, module responded to BREAK)"
echo ""

echo "═════════════════════════════════════════════════════════════"
echo ""
echo "RECOMMENDED TEST PROCEDURE:"
echo "───────────────────────────────────────────────────────────"
echo ""
echo "Step 1: With multimeter, verify:"
echo "  □ Core2530 VCC = 3.3V"
echo "  □ Core2530 GND = 0V (relative to BeagleBone GND)"
echo ""
echo "Step 2: If power is good, check pin connections:"
echo "  □ Is P8_31 connected to Core2530 RX?"
echo "  □ Is P8_32 connected to Core2530 TX?"
echo "  □ Is GND connected?"
echo ""
echo "Step 3: If still no response, check if module needs RESET:"
echo "  □ Does Core2530 have a RESET pin?"
echo "  □ If yes, try pulsing it LOW for 100ms then release"
echo ""
echo "Step 4: Check module documentation:"
echo "  □ Does it need special initialization command?"
echo "  □ Is there a bootloader mode that's active?"
echo "  □ What is the default baud rate (not 115200?)?"
echo ""

echo "═════════════════════════════════════════════════════════════"
echo ""
echo "If STILL no response after checking all above:"
echo ""
echo "Try installing and using minicom for interactive testing:"
echo "  sudo apt install minicom"
echo "  sudo minicom -D /dev/ttyS1 -b 115200"
echo ""
echo "In minicom:"
echo "  Type: AT"
echo "  Press: Enter"
echo "  Should see: OK"
echo "  To quit: Ctrl+A then X then Y"
echo ""

