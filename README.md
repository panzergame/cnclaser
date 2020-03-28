# CNC Laser

!! Under development !!

Light CNC controller for laser cutting machine using simple G-Code.
Compatible with arduino 328P.

# Building

Install cmake-avr and :

```bash
mkdir -p build
cd build
cmake ..
make install
```

This should build and send it to your arduino.

# Usage

Use python script to send G-Code :

```bash
python send.py program.ngc
```
