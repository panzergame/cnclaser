import serial
import sys

with open(sys.argv[1]) as raw_file:
	with serial.Serial('/dev/ttyUSB0', 9600) as ser:
		ser.readline()

		line = raw_file.readline()
		while line:
			ser.write(str.encode(line))
			print(ser.readline())

			line = raw_file.readline()
