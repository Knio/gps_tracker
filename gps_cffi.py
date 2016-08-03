import time
import cffi

ffi = cffi.FFI()
ffi.cdef('int open_device(const char* fname, uint8_t addr);')
ffi.cdef('uint16_t read_line3(int file, char* line, uint16_t len);')

buf = ffi.new('char[]', 512)
dll = ffi.dlopen('./gps.so')

file = dll.open_device('/dev/i2c-2', 0x42)

while 1:
    read = dll.read_line3(file, buf, 512)
    if read:
        print(ffi.string(buf))
    else:
        time.sleep(0.05)

