#!/bin/bash

echo "=== Checking BeagleBone UART Pin Configuration ==="
echo ""

# Check if pin control files exist
echo "1. Checking pinctrl interface..."
if [ -d "/sys/kernel/debug/pinctrl" ]; then
    echo "   ✓ pinctrl interface available"
else
    echo "   ✗ pinctrl interface not available"
fi

echo ""
echo "2. Checking device tree overlays..."
if [ -d "/sys/firmware/devicetree/base" ]; then
    ls -la /sys/firmware/devicetree/base/ | grep uart || echo "   No UART in device tree"
fi

echo ""
echo "3. Checking /dev devices..."
ls -la /dev/ttyS* 2>/dev/null | head -5

echo ""
echo "4. Checking BeagleBone pin slots..."
if [ -d "/sys/devices/platform/ocp" ]; then
    echo "   Pin controller found"
    # Try to find P8 pins
    find /sys/kernel/debug/pinctrl -name "*P8*" 2>/dev/null | head -5
fi

echo ""
echo "5. Manual UART1 pin info (P8_31=RX, P8_32=TX):"
echo "   P8_31 (GPIO0_10) should be configured for uart1_rxd"
echo "   P8_32 (GPIO0_11) should be configured for uart1_txd"

echo ""
echo "6. Device Tree entries:"
grep -r "uart1" /sys/firmware/devicetree/base/ 2>/dev/null | head -3

echo ""
echo "7. Checking if device tree overlay needs loading:"
if [ -f "/sys/kernel/config/device-tree/overlays" ]; then
    echo "   Dynamic overlays supported"
    ls /sys/kernel/config/device-tree/overlays/ 2>/dev/null
else
    echo "   Dynamic overlays not available - may need to use uboot or edit uEnv.txt"
fi

echo ""
echo "=== Recommendation ==="
echo "If UART1 is not showing, try:"
echo "1. Edit /boot/uEnv.txt to enable UART1 overlay"
echo "2. Add: enable_uart1=1 or a cape overlay for UART1"
echo "3. Reboot the BeagleBone"
