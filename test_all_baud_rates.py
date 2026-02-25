#!/usr/bin/env python3
"""
Test Core2530 at different baud rates
Helps find the correct communication speed if default isn't working
"""

import serial
import time
import sys

BAUD_RATES = [9600, 19200, 38400, 57600, 115200, 230400]
PORT = "/dev/ttyS1"

print("╔════════════════════════════════════════════════════════════╗")
print("║   Multi-Baud Rate Test for Core2530                        ║")
print("╚════════════════════════════════════════════════════════════╝")
print("")

for baud in BAUD_RATES:
    print(f"Testing {PORT} at {baud} baud...", end=" ", flush=True)
    
    try:
        ser = serial.Serial(PORT, baud, timeout=1)
        time.sleep(0.1)
        
        # Send AT command
        ser.write(b'AT\r\n')
        time.sleep(0.2)
        
        # Read response
        response = ser.read(100)
        ser.close()
        
        if response:
            print(f"✓ Response received!")
            print(f"   Raw: {response}")
            print(f"   ASCII: {response.decode('utf-8', errors='replace')}")
            print("")
            print(f"SUCCESS! Core2530 is responding at {baud} baud")
            print(f"Update your code to use: B{baud}")
            sys.exit(0)
        else:
            print("✗ No response")
    except Exception as e:
        print(f"✗ Error: {e}")

print("")
print("No responses received at any baud rate.")
print("")
print("This suggests:")
print("  1. Module is not powered")
print("  2. TX/RX connections are reversed")
print("  3. Module is not responding for some other reason")
print("")
print("Try:")
print("  1. Verify 3.3V power to module with multimeter")
print("  2. Try swapping TX/RX wires")
print("  3. Check for RESET pin that needs to be toggled")
