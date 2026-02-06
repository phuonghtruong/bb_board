#!/bin/bash

# 1. Check if the user provided an argument
GATEWAY=$1

if [ -z "$GATEWAY" ]; then
  echo "Error: No gateway IP address provided."
  echo "Usage: ./bbb_setup.sh <gateway_ip>"
  echo "Example: ./bbb_setup.sh 192.168.6.1"
  exit 1
fi

echo "Setting gateway to: $GATEWAY"

# 2. Remove existing default routes to avoid conflicts
# We use '|| true' so the script continues even if no route exists to delete
sudo route del default 2>/dev/null || true

# 3. Add the new gateway provided by the user
sudo route add default gw $GATEWAY

# 4. Set DNS (Google DNS)
echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf >/dev/null

# 5. Test the connection
echo "Testing connection to 8.8.8.8 (Google DNS)..."
if ping -c 2 8.8.8.8 >/dev/null; then
  echo "Success! Internet is reachable."
else
  echo "Failed. Check your Host PC's NAT/Firewall settings."
fi
