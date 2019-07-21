import serial
import sys

with open(sys.argv[1]) as raw_file:
	with serial.Serial('/dev/ttyUSB0', 28800) as ser:
		ser.readline()

		line = raw_file.readline()
		while line:
			ser.write(str.encode(line))
			print("Sent \"{}\"".format(line.strip()))
			res = ser.readline()
			while res != b'ack\n':
				print(res.decode())
				res = ser.readline()

			print("Ack {}".format(res))

			line = raw_file.readline()
