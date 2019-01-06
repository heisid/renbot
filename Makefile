compile: *.ino
	arduino-cli compile --fqbn arduino:avr:uno .
upload:
	arduino-cli upload --verbose --fqbn arduino:avr:uno --port /dev/ttyACM0 .
forget:
	arduino-cli upload --fqbn arduino:avr:uno --port /dev/ttyACM0 ../blank
