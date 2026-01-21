#!/bin/bash
# DO NOT run as ./setup.sh if you want the venv to stay active.
# RUN AS: source ./setup.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
TESTS_DIR="$PROJECT_ROOT/tests"
UNITY_DIR="$TESTS_DIR/unity"
VENV_DIR="$PROJECT_ROOT/venv"
REQUIREMENTS_FILE="$PROJECT_ROOT/requirements.txt"

# --- FIX PATHS HERE ---
# Adjust this to where you actually cloned Unity
UNITY_SRC_PATH="$PROJECT_ROOT/../Unity/src"

echo "Checking for SCRIPT_DIR in: $SCRIPT_DIR"
echo "Checking for PROJECT dir in: $PROJECT_ROOT"
echo "Checking for Unity source in: $UNITY_SRC_PATH"
echo "Checking for Unity test dir: $UNITY_DIR"
echo "Checking for test dir: $TESTS_DIR"

# Create tests/unity directory
mkdir -p "$UNITY_DIR"

# Function to create symlinks safely
create_link() {
  local src="$1"
  local dest="$2"
  if [ ! -f "$src" ]; then
    echo "ERROR: Source file $src not found!"
    return 1
  fi
  if [ -L "$dest" ]; then
    echo "Symlink for $(basename "$dest") already exists."
  else
    ln -s "$src" "$dest"
    echo "Created symlink for $(basename "$dest")"
  fi
}

# Link all 3 required Unity files
create_link "$UNITY_SRC_PATH/unity.c" "$UNITY_DIR/unity.c"
create_link "$UNITY_SRC_PATH/unity.h" "$UNITY_DIR/unity.h"
create_link "$UNITY_SRC_PATH/unity_internals.h" "$UNITY_DIR/unity_internals.h"

# Create python virtual environment
if [ -d "$VENV_DIR" ]; then
  echo "Virtual environment already exists."
else
  python3 -m venv "$VENV_DIR"
  echo "Created python virtual environment."
fi

# Activate virtual environment (Only stays active if you 'source' this script)
source "$VENV_DIR/bin/activate"

# Install packages
if [ -f "$REQUIREMENTS_FILE" ]; then
  pip install --upgrade pip
  pip install -r "$REQUIREMENTS_FILE"
  echo "Installed requirements."
fi

echo "Setup complete."
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
  echo "NOTE: Run 'source venv/bin/activate' to enter the virtual environment."
fi
