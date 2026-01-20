# shell script to load application into beagblebone black
# Usage: ./load_app.sh <application_file>
# Example: ./load_app.sh my_app
# This script copies the specified application file to the BeagleBone Black
# and sets the appropriate permissions to make it executable.
# Make sure to have SSH access to the BeagleBone Black before running this script.
# Replace 'beaglebone' with the actual hostname or IP address of your BeagleBone Black.
# Ensure that you have SSH keys set up for passwordless login or be prepared to enter the password.
# Author: BrianT
#
#!/bin/bash
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <application_file>"
  exit 1
fi
APP_FILE=$1
BB_HOST="192.168.7.2"
BB_USER="debian"
BB_PATH="/home/$BB_USER/"

# Copy the application file to BeagleBone Black
# Using scp to securely copy the file
# The -C option enables compression for faster transfer
scp -C "$APP_FILE" "$BB_USER@$BB_HOST:$BB_PATH"
if [ $? -ne 0 ]; then
  echo "Error: Failed to copy $APP_FILE to $BB_HOST"
  exit 1
fi
# Set executable permissions on the application file
ssh "$BB_USER@$BB_HOST" "chmod +x $BB_PATH$(basename $APP_FILE)"
if [ $? -ne 0 ]; then
  echo "Error: Failed to set executable permissions on $APP_FILE"
  exit 1
fi
echo "Successfully loaded $APP_FILE to $BB_HOST and set executable permissions."
exit 0
