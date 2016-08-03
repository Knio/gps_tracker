import time

import Adafruit_GPIO.I2C as i2c
import pynmea2

dev = i2c.get_i2c_device(busnum=2, address=0x42)

def read():
    hi = dev.readU8(0x7d)
    lo = dev.readU8(0x7e)
    nbytes = (hi << 8) | lo
    print('reading %d bytes' % nbytes)
    return ''.join(chr(dev.readU8(0xff)) for i in range(nbytes))

def read2():
    hi = dev.readU8(0x7d)
    lo = dev.readU8(0x7e)
    nbytes = (hi << 8) | lo
    line = []
    while 1:
        b = dev.readU8(0xff)
        if b == 0xff: break
        c = chr(b)
        if c == '\n': break
        line.append(c)
    return (nbytes, ''.join(line))

while 1:
    nb, line = read2()
    try:
        msg = pynmea2.parse(line)
        try:
            print((msg.timestamp, msg))
        except:
            print(msg)
    except pynmea2.ParseError:
        print(nb, line)

