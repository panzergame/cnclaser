MCU=atmega328p
F_CPU=16000000
CC=avr-g++
OBJCOPY=avr-objcopy
CFLAGS=-std=gnu++11 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
LDFLAGS=
TARGET=build/main
SRCS=src/main.cpp
BUILDDIR=build

all: main
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

main: main.o
	${CC} -o ${TARGET}.bin $^ $(LDFLAGS)

%.o: build/%.cpp
	${CC} -o $@ -c $< $(CFLAGS)

flash:
	avrdude -p ${MCU} -c usbasp -U flash:w:${TARGET}.hex:i -F -P usb

clean:
	rm -f *.bin *.hex
