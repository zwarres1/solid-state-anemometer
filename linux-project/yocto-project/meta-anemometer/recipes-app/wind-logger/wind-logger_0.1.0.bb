SUMMARY = "Rust data logger for STM32 wind sensor"
LICENSE = "CLOSED"

inherit cargo

# 1. Fetch the code: Point exactly to the main branch of your repo
SRC_URI = "git://github.com/zwarres1/solid-state-anemometer.git;protocol=https;branch=main"

# 2. Always grab the absolute latest commit on that branch
SRCREV = "${AUTOREV}"

# 3. Tell Yocto where the Git fetcher unpacks the code internally
S = "${WORKDIR}/git/linux-project/comms-project"

# 4. The Developer Hack: Keep the internet on so Cargo can grab crates.io dependencies
do_compile[network] = "1"