import serial
import sys
import time

def print_uart(data):
	print(data.decode().strip())

def valid_cmd(line):
	return line.startswith("G0") or line.startswith("G1") or \
			line.startswith("G1") or line.startswith("G2") or \
			line.startswith("M1") or line.startswith("M2")

def wait_ready(serial):
	res = ser.readline()
	while res != b'Ready\n':
		print_uart(res)
		res = ser.readline()

	print_uart(res)

def send_file(serial, gfile):
	line = gfile.readline()
	while line:
		if valid_cmd(line):
			ser.write(str.encode(line))
			print("Sent \"{}\"".format(line.strip()))

			res = ser.readline()
			while res != b'ak\n':
				res = ser.readline()

			print("Ack {}".format(res.decode().strip()))

		line = gfile.readline()

def wait_finish(serial):
	# Toute les lignes envoyé mais attendre pour ne pas fermer la liaison et rédémarrer le proc.
	try:
		while True:
			time.sleep(1)
	except KeyboardInterrupt:
		print("Exit")

with serial.Serial('/dev/ttyUSB0', 28800) as ser:
	wait_ready(serial)

	with open(sys.argv[1]) as gfile:
		send_file(serial, gfile)

	wait_finish(serial)

