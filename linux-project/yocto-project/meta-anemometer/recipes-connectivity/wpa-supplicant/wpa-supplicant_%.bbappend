FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://wpa_supplicant-wlan0.conf"

# Tell systemd to auto-enable this specific hardware service on boot
SYSTEMD_SERVICE:${PN}:append = " wpa_supplicant@wlan0.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${sysconfdir}/wpa_supplicant
    install -m 600 ${WORKDIR}/wpa_supplicant-wlan0.conf ${D}${sysconfdir}/wpa_supplicant/
}