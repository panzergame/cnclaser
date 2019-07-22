import serial
import sys
import time

def valid_cmd(line):
	return line.startswith("G0") or line.startswith("G1") or \
			line.startswith("G1") or line.startswith("G2") or \
			line.startswith("M1") or line.startswith("M2")

with open(sys.argv[1]) as raw_file:
	with serial.Serial('/dev/ttyUSB0', 28800) as ser:
		ser.readline()

		line = raw_file.readline()
		while line:
			if valid_cmd(line):
				ser.write(str.encode(line))
				print("Sent \"{}\"".format(line.strip()))

				res = ser.readline()
				while res != b'ak\n':
					print(res)
					res = ser.readline()

				print("Ack {}".format(res))

			line = raw_file.readline()

		# Toute les lignes envoyé mais attendre pour ne pas fermer la liaison et rédémarrer le proc.
		while True:
			time.sleep(1)
