#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

void convert_value(unsigned int value, unsigned char *output) {
    unsigned int shifted_value = value << 21;
    output[0] = (shifted_value >> 24) & 0xFF;
    output[1] = (shifted_value >> 16) & 0xFF;
    output[2] = (shifted_value >> 8) & 0xFF;
    output[3] = shifted_value & 0xFF;
}

void send_motor_command(const char *port, int motor_id, float angle, int speed) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Unable to open port");
        return;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    unsigned char buffer[20];
    memset(buffer, 0, sizeof(buffer));

    int pos = (int)(angle * 100);
    unsigned int can_id_send = 0x140 + motor_id;

    buffer[0] = 0x41; // Header
    buffer[1] = 0x54;
    buffer[2] = 0x28;
    buffer[3] = 0x20;
    buffer[4] = 0x00;
    buffer[5] = 0x00;
    buffer[6] = 0x08;
    buffer[7] = 0xA4;

    convert_value(can_id_send, &buffer[8]);
    convert_value(speed, &buffer[12]);
    convert_value(pos, &buffer[16]);

    write(fd, buffer, 20);

    // Optional: Read response from device
    char response[100];
    int n = read(fd, response, sizeof(response) - 1);
    if (n > 0) {
        response[n] = '\0';
        printf("Response: %s\n", response);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <serial_port> <motor_id> <angle> <speed>\n", argv[0]);
        return 1;
    }

    const char *serial_port = argv[1];
    int motor_id = atoi(argv[2]);
    float angle = atof(argv[3]);
    int speed = atoi(argv[4]);

    send_motor_command(serial_port, motor_id, angle, speed);

    return 0;
}
