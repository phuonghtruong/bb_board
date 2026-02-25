#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   Quick Hardware Diagnostic                                ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

echo "BEFORE YOU START:"
echo "You will need:"
echo "  • A multimeter (or voltmeter)"
echo "  • Your Core2530 module"
echo "  • Your BeagleBone Black"
echo ""

read -p "Do you have a multimeter available? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    echo "Without a multimeter, here's what to check visually:"
    echo ""
    echo "1. Is the Core2530 module CONNECTED?"
    echo "   - Should be soldered or plugged into a breadboard"
    echo "   - Wires from module to BBB should be visible"
    echo ""
    echo "2. Is the Core2530 module POWERED?"
    echo "   - Look for a LED on the module (may be tiny, red or green)"
    echo "   - Is it lit or blinking?"
    echo ""
    echo "3. Are wires connected to the RIGHT pins?"
    echo "   - RX/TX should be to Core2530's serial pins"
    echo "   - P8_31 and P8_32 are on the BBB P8 header"
    echo ""
    echo "Next steps:"
    echo "  1. Try minicom:"
    echo "     sudo apt install minicom"
    echo "     sudo minicom -D /dev/ttyS1 -b 115200"
    echo ""
    echo "  2. Type: AT"
    echo "  3. Press: Enter"
    echo "  4. If you see OK, connection works!"
    echo ""
    exit 0
fi

echo ""
echo "MEASUREMENT 1: BeagleBone 3.3V Output"
echo "─────────────────────────────────────────"
echo "Set multimeter to DC Voltage (V or VDC)"
echo "Measure between:"
echo "  • Red probe: P9_3 or P9_4 (3.3V)"
echo "  • Black probe: P8_1 or P8_2 (GND)"
echo ""
read -p "What voltage do you measure? (e.g., 3.3) " bbv
echo "BeagleBone VCC: ${bbv}V"
if (( $(echo "$bbv < 3.0" | bc -l) )); then
    echo "⚠ WARNING: Less than 3.0V! Power supply may be weak."
fi
echo ""

echo "MEASUREMENT 2: Core2530 VCC Input"
echo "─────────────────────────────────────────"
echo "Measure between:"
echo "  • Red probe: Core2530 VCC pin"
echo "  • Black probe: Core2530 GND pin"
echo ""
read -p "What voltage do you measure? (e.g., 3.3) " modv
echo "Core2530 VCC: ${modv}V"
if (( $(echo "$modv < 2.8" | bc -l) )); then
    echo "✗ PROBLEM: Voltage too low! Module may not work."
    echo "  - Check wiring from BBB to module"
    echo "  - Check for poor connections"
elif (( $(echo "$modv > 3.6" | bc -l) )); then
    echo "✗ PROBLEM: Voltage too high! May damage module."
    echo "  - Check power supply"
elif (( $(echo "$modv > 3.0" | bc -l) )) && (( $(echo "$modv < 3.6" | bc -l) )); then
    echo "✓ GOOD: Voltage is in normal range (3.0-3.6V)"
fi
echo ""

echo "MEASUREMENT 3: Ground Continuity"
echo "─────────────────────────────────────────"
echo "Set multimeter to CONTINUITY or OHM mode"
echo "Test between:"
echo "  • Red probe: BeagleBone GND (P8_1 or P8_2)"
echo "  • Black probe: Core2530 GND"
echo ""
read -p "Do you hear a BEEP? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "✓ GOOD: Ground is connected"
else
    echo "✗ PROBLEM: No ground connection!"
    echo "  - Check GND wiring"
    echo "  - Try a different cable"
fi
echo ""

echo "═════════════════════════════════════════════════════════════"
echo ""
echo "SUMMARY:"
echo "────────"
if (( $(echo "$modv > 3.0" | bc -l) )) && (( $(echo "$modv < 3.6" | bc -l) )) && [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "✓ Power and ground look GOOD!"
    echo ""
    echo "Next step: Test serial communication with minicom"
    echo "  sudo apt install minicom"
    echo "  sudo minicom -D /dev/ttyS1 -b 115200"
    echo ""
    echo "If minicom still shows no response:"
    echo "  • TX/RX might be reversed"
    echo "  • Baud rate might be different (try 9600, 19200, etc)"
    echo "  • Module might need RESET pulse"
else
    echo "⚠ There appears to be a POWER or GROUND issue"
    echo ""
    echo "Check:"
    echo "  1. VCC wiring from BBB P9_3/P9_4 to Core2530 VCC"
    echo "  2. GND wiring from BBB P8_1/P8_2 to Core2530 GND"
    echo "  3. Try with a different cable"
    echo "  4. Check for loose connections or solder joints"
fi
echo ""

