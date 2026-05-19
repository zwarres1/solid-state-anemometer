#!/bin/bash

BUILD_DIR="build-pi"
ENVIRONMENT_DIR="poky/oe-init-build-env"

# Initialize the standard Yocto environment
source ${ENVIRONMENT_DIR} ${BUILD_DIR}

# 2. Append our target variables to the fresh local.conf if not already there
if ! grep -q "anemometer-os" conf/local.conf; then
    echo "" >> conf/local.conf
    echo "# === Anemometer Project Configuration ===" >> conf/local.conf
    echo 'MACHINE = "raspberrypi0-wifi"' >> conf/local.conf
    echo 'DISTRO = "anemometer-os"' >> conf/local.conf
    echo "✅ local.conf updated to use anemometer-os!"
fi

# 3. Dynamically add layers to bblayers.conf
# This handles the absolute paths for you behind the scenes!
echo "⚙️ Adding layers to bblayers.conf..."

# The Open Embedded Layers
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia

# Hardware and Custom layers
bitbake-layers add-layer ../meta-raspberrypi
bitbake-layers add-layer ../meta-anemometer

echo "🎉 Setup complete! You can now run ./deploy.sh /dev/sdX"