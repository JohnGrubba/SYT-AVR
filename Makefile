MCU=attiny45		# t45 would also work
F_CPU=1200000
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=main
SRCS=main.c 

# (only for STK500=Diamex Programer)
# Note: find out which PORT the USB is connected to	ex. com4
# Note: check with "avrdude -P com4 -c STK500 -p t45" if access to programer and attiny is valid

# Programer 1: Diamex=STK500 
# avrdude -p ${MCU} -c STK500 -U flash:w:${TARGET}.hex:i -F -P com4

# Programer 2: USBasp
# avrdude -p ${MCU} -c usbasp -U flash:w:${TARGET}.hex:i -F -P usb

# Programer 3: avrrisp2 MKll
# avrdude -p ${MCU} -c avrisp2 -U flash:w:${TARGET}.hex:i -F -P usb

	
all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex	
	
flash: all
	avrdude -p ${MCU} -c STK500 -U flash:w:${TARGET}.hex:i -F -P /dev/ttyACM0

clean:
	rm -f *.bin *.hex
