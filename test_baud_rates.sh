#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   Testing Different Baud Rates                             ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

PORT="/dev/ttyS1"
BAUD_RATES=(9600 19200 38400 57600 115200 230400)

for BAUD in "${BAUD_RATES[@]}"; do
    echo "Testing $BAUD baud..."
    
    # Configure port
    stty -F $PORT $BAUD raw -echo 2>/dev/null
    sleep 0.1
    
    # Send AT command
    (printf "AT\r\n" > $PORT) 2>/dev/null
    sleep 0.3
    
    # Read response
    RESPONSE=$(timeout 1 cat $PORT | head -c 100)
    
    # Check if response looks valid
    if echo "$RESPONSE" | grep -q "OK\|ERROR\|at\|AT" 2>/dev/null; then
        echo "✓ FOUND IT! Module responds at $BAUD baud"
        echo "  Response: $RESPONSE"
        echo ""
        echo "Update your code to use: B$BAUD"
        exit 0
    elif [ -z "$RESPONSE" ]; then
        echo "  ✗ No response"
    else
        echo "  ✗ Garbage data (might be wrong baud)"
        echo "    First bytes: $(echo -n "$RESPONSE" | od -An -tx1 | head -c 30)"
    fi
    echo ""
done

echo "No valid response found at any baud rate."
echo ""
echo "Try manually in minicom:"
echo "  sudo minicom -D /dev/ttyS1 -b 9600"
echo "Then press Ctrl+A O to change baud rate if needed"
