#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdint.h>

static union i2c_smbus_data byte_data;
static struct i2c_smbus_ioctl_data byte_args = {
    .read_write = I2C_SMBUS_READ,
    .size = I2C_SMBUS_BYTE_DATA,
    .data = &byte_data
};

uint8_t read_byte(int file, uint8_t reg) {
    byte_args.command = reg;
    if (ioctl(file, I2C_SMBUS, &byte_args) == -1)
        exit(3);
    return byte_data.byte;
}


static union i2c_smbus_data block_data;
static struct i2c_smbus_ioctl_data block_args = {
    .read_write = I2C_SMBUS_READ,
    .size = I2C_SMBUS_I2C_BLOCK_DATA,
    .data = &block_data
};

uint8_t read_block(int file, uint8_t reg, uint8_t size) {

    uint32_t err;
    block_args.command = reg;
    block_data.block[0] = size;
    if (err = ioctl(file, I2C_SMBUS, &block_args) == -1)
        exit(3);
    return err;
}


uint16_t read_line3(int file, char* line, uint16_t n) {
    uint16_t i = 0;
    static uint16_t j = 32;
    while (i < n) {

        if (j == 33) {
            read_block(file, 0xff, 32);
            // printf("%d %32s\n", block_data.block[0], &block_data.block[1]);
            j = 1;
        }

        char c = block_data.block[j++];

        if (c == '\n' || c == 0xff) {
            line[i] = 0;
            while (j <= 32 && block_data.block[j] == 0xff) j++;
            return i;
        }

        line[i++] = c;
    }
}


uint16_t read_line(int file, char* line, uint16_t n) {
    char c;
    uint8_t lo;
    uint8_t hi;
    uint16_t i;
    for (i=0; i < n; i++) {
        c = read_byte(file, 0xff);
        if (c == 0xff || c == '\n') break;
        line[i] = c;
    }
    line[i] = 0;

    hi = read_byte(file, 0xfd);
    lo = read_byte(file, 0xfe);
    return (hi << 8) | lo;;
}

int open_device(const char* fname, uint8_t addr) {
    int file = 0;

    if ((file = open(fname, O_RDWR)) < 0)
        exit(1);

    if (ioctl(file, I2C_SLAVE, addr) < 0)
        exit(2);

    return file;
}

int main() {
    int file = open_device("/dev/i2c-2", 0x42);

    char line[512];
    uint16_t more = 0;

    while (1) {
        more = read_line3(file, line, 512);
        if (more) {
            printf("%x -> %s\n", more, line);
        }
    }

    return 0;
}
