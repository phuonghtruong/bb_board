#!/bin/bash

# --- Load Secrets ---
# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -f "$SCRIPT_DIR/.env.discord.webhook" ]; then
  # Load the DISCORD_WEBHOOK variable from the .env file
  source "$SCRIPT_DIR/.env.discord.webhook"
else
  echo "Error: .env file not found. Please create one with DISCORD_WEBHOOK URL."
  exit 1
fi

# --- Argument Check ---
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <bbb_ip>"
  echo "Example: $0 192.168.6.2"
  exit 1
fi

# --- Configuration ---
BBB_USER="debian"
BBB_IP=$1                     # Received from script argument
REMOTE_DIR="/home/debian"     # Where BBB saves videos
LOCAL_DIR="./security_videos" # Where laptop saves them
FFMPEG_LOGLEVEL="error"       # 'error' or 'info'

# 1. Create local directory if it doesn't exist
mkdir -p "$LOCAL_DIR"

echo "-------------------------------------------------------"
echo "Connecting to BeagleBone at $BBB_IP..."

# 2. Download MJPEG files and remove from BBB to save space
# We use -e to ensure SSH is used, and quotes for paths
rsync -avz --remove-source-files \
  "${BBB_USER}@${BBB_IP}:${REMOTE_DIR}/*.mjpeg" "$LOCAL_DIR/"

# 3. Check if any files were downloaded
cd "$LOCAL_DIR" || exit
shopt -s nullglob
files=(*.mjpeg)

if [ ${#files[@]} -eq 0 ]; then
  echo "No new mjpeg files found on BeagleBone at $BBB_IP."
  exit 0
fi

echo "Downloaded ${#files[@]} files. Starting conversion..."

# 4. Loop through each file and convert
for f in "${files[@]}"; do
  base="${f%.mjpeg}"
  output_mp4="${base}.mp4"

  echo "Processing: $f -> $output_mp4"

  # --- FFmpeg Conversion ---
  # Scales down to 720p to stay under Discord's 25MB limit (usually)
  ffmpeg -y -loglevel "$FFMPEG_LOGLEVEL" -i "$f" \
    -vf "scale=-1:720" \
    -c:v libx264 -crf 25 -pix_fmt yuv420p "$output_mp4"

  # 5. Check if conversion succeeded
  if [ $? -eq 0 ]; then
    echo "Conversion successful. Uploading to Discord..."

    # --- Discord Upload ---
    # We send a text message + the file
    curl -H "Content-Type: multipart/form-data" \
      -F "content=🚨 **Motion Detected!** BeagleBone: \`$BBB_IP\` File: \`$output_mp4\`" \
      -F "file=@$output_mp4" \
      "$DISCORD_WEBHOOK_KIEM_TRA_AN_TROM"

    # 6. Clean up both files after successful upload
    if [ $? -eq 0 ]; then
      rm "$f" "$output_mp4"
      echo "✅ Successfully sent and cleaned up $f"
    else
      echo "❌ Error: Failed to upload to Discord."
    fi
  else
    echo "❌ Error: Failed to convert $f"
  fi
done

echo "-------------------------------------------------------"
echo "All tasks complete for $BBB_IP."
