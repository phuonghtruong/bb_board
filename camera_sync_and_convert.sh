#!/bin/bash

# --- Configuration ---
BBB_USER="debian"
BBB_IP="192.168.6.2"
REMOTE_DIR="/home/debian"     # Where your C app saves videos
LOCAL_DIR="./security_videos" # Where to save them on your laptop
FFMPEG_LOGLEVEL="error"       # Change to 'info' for more detail

# 1. Create local directory if it doesn't exist
mkdir -p "$LOCAL_DIR"

echo "-------------------------------------------------------"
echo "Connecting to BeagleBone at $BBB_IP..."

# 2. Use rsync to download only .mjpeg files
# --remove-source-files will delete the file from the BeagleBone
# once it is safely on your laptop (saves space on the BBB)
rsync -avz --remove-source-files \
  ${BBB_USER}@${BBB_IP}:${REMOTE_DIR}/*.mjpeg "$LOCAL_DIR/"

# 3. Check if any files were actually downloaded
cd "$LOCAL_DIR" || exit
shopt -s nullglob
files=(*.mjpeg)

if [ ${#files[@]} -eq 0 ]; then
  echo "No new mjpeg files found on BeagleBone."
  exit 0
fi

echo "Downloaded ${#files[@]} files. Starting conversion..."

# 4. Loop through each file and convert
for f in "${files[@]}"; do
  # Get filename without extension (e.g., motion_2025-02-05)
  base="${f%.mjpeg}"

  echo "Processing: $f -> ${base}.mp4"

  # Use ffmpeg to convert
  # -y: overwrite if exists
  # -pix_fmt yuv420p: ensures compatibility with all players
  ffmpeg -y -loglevel "$FFMPEG_LOGLEVEL" -i "$f" \
    -c:v libx264 -crf 23 -pix_fmt yuv420p "${base}.mp4"

  # 5. Clean up the local MJPEG after successful conversion
  if [ $? -eq 0 ]; then
    rm "$f"
    echo "Successfully converted and cleaned up $f"
  else
    echo "Error: Failed to convert $f"
  fi
done

echo "-------------------------------------------------------"
echo "Done! All videos are in $LOCAL_DIR"
