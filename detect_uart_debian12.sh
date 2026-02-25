#!/bin/bash

echo "=== BeagleBone Serial Port Detection (Debian 12) ==="
echo ""

echo "1. Available Serial Ports:"
ls -la /dev/ttyS* /dev/ttyO* /dev/ttyAMA* 2>/dev/null || echo "   (checking...)"

echo ""
echo "2. Kernel Boot Messages:"
dmesg | grep -i "serial\|uart\|ttyS\|ttyO" | tail -20

echo ""
echo "3. Checking /proc/tty/drivers:"
grep -E "serial|uart" /proc/tty/drivers

echo ""
echo "4. Device Tree Serial Nodes:"
find /sys/firmware/devicetree/base -name "*serial*" -o -name "*uart*" 2>/dev/null | head -10

echo ""
echo "5. UART Port Details:"
for port in /dev/ttyS* /dev/ttyO* /dev/ttyAMA*; do
    if [ -e "$port" ]; then
        echo "   $port exists"
        # Try to get info
        stty -F "$port" 2>/dev/null && echo "     (accessible)"
    fi
done

echo ""
echo "6. Current UART1 Test with /dev/ttyS1:"
timeout 2 cat /dev/ttyS1 2>&1 | head -5 || echo "   (port not responding or not available)"

echo ""
echo "=== RECOMMENDATION ==="
echo "If /dev/ttyS1 doesn't exist, try:"
echo "  1. Check dmesg output above for actual port names"
echo "  2. Might be /dev/ttyO1 or /dev/ttyAMA0 instead"
echo "  3. Update your code to use the correct port"
