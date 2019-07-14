# /home/tristan/.arduino15/packages/STM32/tools/STM32Tools/1.3.0/tools/linux/maple_upload ttyACM0 1 1EAF:0003 build/main.ino.bin
/usr/bin/avrdude -C /etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D -Uflash:w:build/main.ino.hex:i 
