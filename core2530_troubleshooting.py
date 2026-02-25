#!/usr/bin/env python3
"""
Core2530 Module Troubleshooting Guide
Helps diagnose why the Core2530 is not responding
"""

print("""
╔════════════════════════════════════════════════════════════════════╗
║         CORE2530 NOT RESPONDING - TROUBLESHOOTING GUIDE            ║
╚════════════════════════════════════════════════════════════════════╝

✓ UART1 is WORKING (confirmed by kernel messages)
✓ UART port can be opened and configured
✓ AT commands are being SENT correctly
✗ Core2530 is NOT responding

═══════════════════════════════════════════════════════════════════════

STEP 1: CHECK MODULE POWER
───────────────────────────────────────────────────────────────────────
□ Does the Core2530 module have a power LED?
  - If YES, is it ON/blinking?
  - If NO, check power supply voltage and connections

□ Typical power requirements:
  - Most Zigbee modules: 3.3V or 5V
  - Check module datasheet for exact specs
  - BeagleBone can supply 3.3V on pin 1, 2 (P9) or other pinouts

ACTION: Verify module has stable power with multimeter


STEP 2: CHECK SERIAL CABLE CONNECTIONS
───────────────────────────────────────────────────────────────────────
Current wiring (if not working):
  BeagleBone P8_31 (UART1_RX) → Core2530 RX
  BeagleBone P8_32 (UART1_TX) → Core2530 TX
  BeagleBone GND              → Core2530 GND

TRY REVERSING RX/TX:
  BeagleBone P8_31 (UART1_RX) → Core2530 TX  ← SWAP
  BeagleBone P8_32 (UART1_TX) → Core2530 RX  ← SWAP
  BeagleBone GND              → Core2530 GND

(Labeling is often confusing - pins might be labeled from the 
 opposite perspective on the module)

ACTION: Run ./uart_diagnostics.bin after swapping


STEP 3: CHECK MODULE INITIALIZATION
───────────────────────────────────────────────────────────────────────
Some modules need a reset pulse before responding:

□ Does your Core2530 have a RESET pin?
  - Typical: Active LOW (pull to GND for 100ms, then release)
  - May need to pulse RESET before sending AT commands

□ Does it need time to boot after power-on?
  - Try adding 2-3 second delay after opening port
  - Some Zigbee modules warm up during boot

ACTION: Check module datasheet for reset procedure


STEP 4: VERIFY AT COMMAND FORMAT
───────────────────────────────────────────────────────────────────────
□ AT command must end with \\r\\n (CR+LF)
  ✓ Our code sends: "AT\\r\\n"
  ✓ Verified in diagnostics

□ Some modules need a delay between commands
  ✓ Our code has 100ms delays

ACTION: Check module AT command manual


STEP 5: TEST WITH MINICOM (MANUAL SERIAL TERMINAL)
───────────────────────────────────────────────────────────────────────
Install if not present:
  sudo apt install minicom

Run interactive serial terminal:
  sudo minicom -D /dev/ttyS1 -b 115200

Type commands manually:
  AT<Enter>
  AT+VER<Enter>
  ATE0<Enter>

Then press Ctrl+A, Q to quit.

ACTION: This will show in real-time if module responds


STEP 6: CHECK BAUD RATE
───────────────────────────────────────────────────────────────────────
Common baud rates:
  ✓ 115200 (what we're using)
  □ 9600
  □ 19200
  □ 38400
  □ 57600

If module responds with garbage at 115200, it might be wrong baud rate.

ACTION: Try other baud rates with minicom


STEP 7: SIGNAL INTEGRITY CHECK (if you have oscilloscope/logic analyzer)
───────────────────────────────────────────────────────────────────────
□ Voltage levels correct? (3.3V or 5V as per module specs)
□ Baud rate timing correct?
□ Noise on lines?
□ Are there pull-up resistors needed?


═══════════════════════════════════════════════════════════════════════

QUICK REFERENCE - BEAGLEBONE P8 HEADER
───────────────────────────────────────────────────────────────────────
P8 Pin Layout (top view):
     1  2
     3  4
    ...
    31 32  ← UART1 (ttyS1)
    33 34
    ...

P8_31 = GPIO0_10 = UART1_RX
P8_32 = GPIO0_11 = UART1_TX


═══════════════════════════════════════════════════════════════════════

NEXT STEPS:
───────────────────────────────────────────────────────────────────────
1. Verify Core2530 module has power
2. Try reversing RX/TX connections
3. Test with: sudo minicom -D /dev/ttyS1 -b 115200
4. Manual type: AT<Enter>
5. Look for OK or any response
6. Check Core2530 datasheet for:
   - Correct baud rate
   - Reset procedure
   - Required initialization
   - Expected responses

""")

# Print specific Core2530 info if available
print("\n═══════════════════════════════════════════════════════════════════════")
print("CORE2530 SPECIFIC NOTES:")
print("───────────────────────────────────────────────────────────────────────")
print("""
The Core2530 is typically a TI CC2530 Zigbee chip module.

Key AT Command Reference:
  AT          → Test connection, module responds with OK
  ATE0        → Disable echo
  ATE1        → Enable echo
  AT+VER      → Get firmware version
  AT+ID       → Get module ID
  AT+DL       → Get/set destination address
  
If module doesn't respond with OK, it may:
  1. Not have power
  2. Be in the wrong mode (bootloader vs normal)
  3. Have incompatible firmware
  4. Need a reset/reboot first

Common baud rates for CC2530: 115200, 57600, 38400
Default is usually 115200.
""")

print("\nGood luck! 🔧")
