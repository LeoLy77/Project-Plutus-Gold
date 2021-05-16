
### Title
Leo Ly 44339124

### Overview

This is the complete practical 3 of CSSE4011 2021

### Requirements

JLINK SEGGER

2 X MICRO USB Cables

NRF THINGY52

### Building

export GNUARMEMB_TOOLCHAIN_PATH="/usr/local" 

export ZEPHYR_TOOLCHAIN_VARIANT="gnuarmemb"

west build -p=always -b thingy52 csse4011-s4433912/apps/p3/ 