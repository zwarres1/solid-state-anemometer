#!/bin/bash
# deploy.sh - Build and flash the Yocto image

# Exit immediately if a command fails
set -e 

if [ -z "$1" ]; then
    echo "❌ Error: No SD card device specified."
    echo "Usage: ./deploy.sh /dev/sdX"
    echo "Run 'lsblk' to find your SD card."
    exit 1
fi

TARGET_DRIVE=$1
MACHINE_NAME="raspberrypi0-wifi"
BUILD_DIR="build-pi"
ENVIRONMENT_DIR="poky/oe-init-build-env"

echo "🚀 Step 1: Building Yocto Image..."
source ${ENVIRONMENT_DIR} ${BUILD_DIR}
bitbake core-image-base

echo "📦 Step 2: Locating generated image..."
# Yocto creates a handy symlink to the latest build, so we don't need to guess the timestamp
IMAGE_FILE="tmp/deploy/images/${MACHINE_NAME}/core-image-base-${MACHINE_NAME}.rootfs.wic.bz2"

if [ ! -f "$IMAGE_FILE" ]; then
    echo "❌ Error: Image file not found at $IMAGE_FILE"
    exit 1
fi

echo "⚡ Step 3: Flashing to $TARGET_DRIVE using bmaptool..."
sudo umount ${TARGET_DRIVE}* 2>/dev/null || true
sudo bmaptool copy "$IMAGE_FILE" "$TARGET_DRIVE"

echo "✅ Success! The SD card is ready."