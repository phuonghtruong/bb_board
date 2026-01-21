# Description: Script to share Internet from host to BeagleBone over USB Ethernet
# Usage: Run this script on the host machine (Linux) with sudo privileges.
# It identifies the Internet interface and the BeagleBone USB Ethernet interface,
# enables IP forwarding, sets up NAT using iptables, and assigns an IP address
# to the host side of the USB link.
# Make sure to run this script with sudo: sudo ./host_setup_internet_over_usb.sh
# Note: This script assumes the BeagleBone USB Ethernet interface starts with 'enxb89'.
# Adjust the grep pattern if your interface name is different.
#
# Example:
#   sudo ./host_setup_internet_over_usb.sh
#----------------------------------------------------------------------------

#!/bin/bash

# 1. Find the Internet interface (Wi-Fi or Ethernet)
WAN_IFACE=$(ip route | grep default | awk '{print $5}' | head -n1)

# 2. Find the BeagleBone interface (starting with enxb89)
BBB_IFACE=$(ls /sys/class/net | grep enxb89 | head -n1)

if [ -z "$BBB_IFACE" ]; then
  echo "Error: BeagleBone interface (enxb89...) not found."
  echo "Check 'ip link' to see if the board is connected."
  exit 1
fi

echo "--- Network Setup ---"
echo "Internet from:  $WAN_IFACE"
echo "Sharing to:     $BBB_IFACE"

# 3. Enable IP Forwarding
sudo sysctl -w net.ipv4.ip_forward=1 >/dev/null

# 4. Clear old rules and set up NAT (Network Address Translation)
sudo iptables -F
sudo iptables -t nat -F
sudo iptables -t nat -A POSTROUTING -o $WAN_IFACE -j MASQUERADE
sudo iptables -A FORWARD -i $WAN_IFACE -o $BBB_IFACE -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i $BBB_IFACE -o $WAN_IFACE -j ACCEPT

# 5. Assign IP to the Host side of the USB link
# For Linux-to-Linux, the BBB usually expects the host to be 192.168.6.1
sudo ip addr add 192.168.7.1/30 dev $BBB_IFACE 2>/dev/null
sudo ip link set $BBB_IFACE up

echo "---------------------"
echo "Host setup complete."
