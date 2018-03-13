# nrf52-ble
Simple code to make the nRF52 communication channel to the mobile device.

#How to Deploy
Download nRF52 SDK11.0.0 from nordic website.
Extract repository to examples/ble_peripheral inside the SDK

Make sure arm compiler is installed and configured in Makefile.posix
Install nrfjprog 
Ensure make is installed
Ensure nRF52DK is plugged in via USB

Ensure present working directory is `examples/ble_peripheral/nrf52-ble/pca10040/s132/armgcc`
To flash run 
`make flash_softdevice
make flash`

