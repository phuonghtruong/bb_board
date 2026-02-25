#!/usr/bin/env python3
"""
BeagleBone Pin Configuration Checker
Checks and displays UART pin configuration without requiring config-pin
"""

import os
import subprocess

def check_device_tree():
    """Check if UART1 is in device tree"""
    print("=== Device Tree Check ===")
    
    try:
        result = subprocess.run(["grep", "-r", "uart", "/sys/firmware/devicetree/base/"], 
                              capture_output=True, text=True, timeout=2)
        if "uart1" in result.stdout or "serial" in result.stdout:
            print("✓ UART entries found in device tree")
            return True
        else:
            print("✗ No UART1 in device tree")
            return False
    except Exception as e:
        print(f"Could not check device tree: {e}")
        return False

def check_uenv():
    """Check uEnv.txt for UART configuration"""
    print("\n=== Checking /boot/uEnv.txt ===")
    
    try:
        with open("/boot/uEnv.txt", "r") as f:
            content = f.read()
            if "uart" in content.lower():
                print("✓ UART configuration found in uEnv.txt")
                # Show relevant lines
                for line in content.split('\n'):
                    if 'uart' in line.lower() and not line.strip().startswith('#'):
                        print(f"  {line}")
                return True
            else:
                print("⚠ No explicit UART configuration in uEnv.txt")
                print("\nTo enable UART1, add these lines to /boot/uEnv.txt:")
                print("  enable_uart1=1")
                print("\nThen reboot: sudo reboot")
                return False
    except FileNotFoundError:
        print("✗ Could not read /boot/uEnv.txt")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False

def check_serial_ports():
    """Check available serial ports"""
    print("\n=== Serial Ports ===")
    
    try:
        result = subprocess.run(["ls", "-la", "/dev/ttyS*"], 
                              capture_output=True, text=True, shell=True)
        if result.stdout:
            lines = result.stdout.strip().split('\n')
            for line in lines[:8]:
                print(f"  {line}")
            print("✓ Serial ports available")
            return True
    except Exception as e:
        print(f"Error: {e}")
        return False

def check_dmesg():
    """Check kernel messages for UART issues"""
    print("\n=== Recent Kernel Messages (UART related) ===")
    
    try:
        result = subprocess.run(["sudo", "dmesg"], 
                              capture_output=True, text=True, timeout=5)
        lines = result.stdout.split('\n')
        uart_lines = [l for l in lines if 'uart' in l.lower() or 'ttyS' in l]
        
        if uart_lines:
            for line in uart_lines[-5:]:
                print(f"  {line}")
            return True
        else:
            print("  No UART messages in kernel log")
            return False
    except Exception as e:
        print(f"Could not read dmesg (need sudo): {e}")
        return False

def check_pinctrl():
    """Check pinctrl configuration"""
    print("\n=== Pinctrl Information ===")
    
    try:
        with open("/sys/kernel/debug/pinctrl/44e10800.pinmux/pins", "r") as f:
            content = f.read()
            uart_pins = [line for line in content.split('\n') 
                        if ('pin 10' in line or 'pin 11' in line) and 'group' in line]
            if uart_pins:
                print("✓ P8_31 (pin 10) and P8_32 (pin 11) configuration:")
                for pin in uart_pins:
                    print(f"  {pin}")
                return True
            else:
                print("Could not find P8_31/P8_32 pins in pinctrl")
                return False
    except FileNotFoundError:
        print("⚠ Pinctrl debug interface not available")
        print("  (This is normal if kernel debug is disabled)")
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False

def main():
    print("=" * 60)
    print("BeagleBone UART Configuration Checker")
    print("=" * 60)
    
    check_device_tree()
    check_uenv()
    check_serial_ports()
    check_dmesg()
    check_pinctrl()
    
    print("\n" + "=" * 60)
    print("RECOMMENDATIONS:")
    print("=" * 60)
    print("""
If UART1 is not working:

1. Check if UART1 is enabled:
   cat /sys/devices/platform/ocp/44e0b000.serial/status
   (Should show "ok" or "available")

2. If disabled, enable it by editing /boot/uEnv.txt:
   sudo nano /boot/uEnv.txt
   
   Add this line (uncomment if exists):
   enable_uart1=1
   
3. Reboot:
   sudo reboot

4. After reboot, verify with:
   ./uart_diagnostics.bin

ALSO CHECK:
- Core2530 module has power (LED lit?)
- Serial cable connections are correct
- RX/TX lines not crossed
- Ground connection present
""")

if __name__ == "__main__":
    main()
