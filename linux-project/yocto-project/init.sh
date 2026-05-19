#!/bin/bash

BUILD_DIR="build-pi"
ENVIRONMENT_DIR="poky/oe-init-build-env"

# Initialize the standard Yocto environment
source ${ENVIRONMENT_DIR} ${BUILD_DIR}

# Prompt user to setup wifi ssid/password
WIFI_CONF="meta-anemometer/recipes-connectivity/wpa-supplicant/files/wpa_supplicant-nl80211-wlan0.conf"
WIFI_TEMPLATE="${WIFI_CONF}.example"
if [ ! -f "$WIFI_CONF" ]; then
    echo "⚠️ Local WiFi config not found. Creating from template..."
    cp "$WIFI_TEMPLATE" "$WIFI_CONF"
    echo "❌ Action Required: Please edit $WIFI_CONF with your real SSID and password before building!"
    exit 1
fi

# Append our target variables to the fresh local.conf if not already there
if ! grep -q "anemometer-os" conf/local.conf; then
    echo "" >> conf/local.conf
    echo "# === Anemometer Project Configuration ===" >> conf/local.conf
    echo 'MACHINE = "raspberrypi0-wifi"' >> conf/local.conf
    echo 'DISTRO = "anemometer-os"' >> conf/local.conf
    echo "✅ local.conf updated to use anemometer-os!"
fi

# Dynamically add layers to bblayers.conf
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