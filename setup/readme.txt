Steps to program STM32F405 firmware

1. Navigate to /setup locally
2. Plug in board over USB
3. Connect B0 to 3.3V then reset to enter DFU bootloader
4. Then run:

dfu-util -a 0 --dfuse-address 0x08000000 -D stm32f405_cpFirmware_7.0.0.bin
