#!/bin/bash

# shell script to load application into beaglebone black
# Usage: ./load_app.sh <application_file> <bb_user> <bb_host>
# Example: ./load_app.sh my_app debian 192.168.7.2
#
# Author: BrianT (Updated)

# Check for exactly 3 arguments
if [ "$#" -ne 3 ]; then
  echo "Usage:   $0 <application_file> <bb_user> <bb_host>"
  echo "Example: $0 security_system debian 192.168.6.2"
  exit 1
fi

APP_FILE=$1
BB_USER=$2
BB_HOST=$3
BB_PATH="/home/$BB_USER/"

# If the user is 'root', the home path is different
if [ "$BB_USER" == "root" ]; then
  BB_PATH="/root/"
fi

echo "Connecting to $BB_USER@$BB_HOST..."

# 1. Copy the application file to BeagleBone Black
# Using scp to securely copy the file
echo "Copying $APP_FILE to $BB_PATH..."
scp -C "$APP_FILE" "$BB_USER@$BB_HOST:$BB_PATH"

if [ $? -ne 0 ]; then
  echo "Error: Failed to copy $APP_FILE to $BB_HOST"
  exit 1
fi

# 2. Set executable permissions on the application file
echo "Setting executable permissions..."
ssh "$BB_USER@$BB_HOST" "chmod +x $BB_PATH$(basename "$APP_FILE")"

if [ $? -ne 0 ]; then
  echo "Error: Failed to set executable permissions on $APP_FILE"
  exit 1
fi

echo "------------------------------------------------------------"
echo "Success: $APP_FILE is now on $BB_HOST and is executable."
echo "To run it, log in and type: .$BB_PATH$(basename "$APP_FILE")"
echo "------------------------------------------------------------"

exit 0
