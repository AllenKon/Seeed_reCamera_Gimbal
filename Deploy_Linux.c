#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

int calculateChecksum(uint8_t [], int, int);
int calculateChecksum(uint8_t byteArray[], int start, int end) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += byteArray[i];
    }
    while (sum > 0xffff)
        sum = (sum & 0xffff) + (sum >> 16);
    return sum;
}

int isSerialPortOpen(int* serialPort, const char* portName) {
    *serialPort = open(portName, O_RDWR | O_NOCTTY);
    if (*serialPort == -1) {
        fprintf(stderr, "Error opening serial port %s\n", portName);
        return 0;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(*serialPort, &tty) != 0) {
        fprintf(stderr, "Error getting serial port state\n");
        close(*serialPort);
        return 0;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // One stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS;// No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(ICRNL | INLCR); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 10; // Wait for up to 1s, returning as soon as any data is received
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(*serialPort, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error setting serial port state\n");
        close(*serialPort);
        return 0;
    }

    return 1;
}

void closeSerialPort(int serialPort) {
    close(serialPort);
}

int sendData(int serialPort, const uint8_t* data, size_t length) {
    ssize_t bytesWritten = write(serialPort, data, length);
    if (bytesWritten == -1) {
        fprintf(stderr, "Error writing to serial port\n");
        return 0;
    }
    return 1;
}

int receiveData(int serialPort, char* buffer, int length, int timeout) {
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(serialPort, &readfds);

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    int result = select(serialPort + 1, &readfds, NULL, NULL, &tv);
    if (result < 0) {
        fprintf(stderr, "Error selecting on serial port\n");
        return 0;
    } else if (result == 0) {
        return 0; // Timeout
    }

    ssize_t bytesRead = read(serialPort, buffer, length);
    if (bytesRead == -1) {
        fprintf(stderr, "Error reading from serial port\n");
        return 0;
    }

    buffer[bytesRead] = '\0'; // 添加字符串结束符
    return bytesRead > 0;
}

char ConvertHexChar(char ch) {
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return -1;
}

void String2Hex(const char* str, uint8_t* senddata, size_t* senddataLen) {
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = strlen(str);
    char lstr, hstr;
    for (int i = 0; i < len;) {
        hstr = str[i];
        if (hstr == ' ') {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i];
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    *senddataLen = hexdatalen;
}

int main() {
    uint8_t motorIDs[17];
    int motorIDCount = 0;
    int serialPort;
    double angles[17];
    int angleCount = 0;
    const char* portName = "/dev/ttyUSB0";
    char input[256];
    
    while (1) {
        if (isSerialPortOpen(&serialPort, portName)) {
            printf("Serial port %s is open.\n", portName);
            break;
        } else {
            printf("Serial port %s is not open.\n", portName);
        }
        sleep(2);  // 每隔2秒检测一次
    }
    
    printf("请输入要操控的电机ID(使用空格隔开,范围0-16): ");
    fgets(input, sizeof(input), stdin);
    char* token = strtok(input, " ");
    while (token != NULL) {
        int id = atoi(token);
        if (id >= 0 && id <= 16) {
            motorIDs[motorIDCount++] = (uint8_t)id;
        } else {
            fprintf(stderr, "电机ID必须在0到16之间。\n");
            return -1;
        }
        token = strtok(NULL, " ");
    }
    char buffer[256]; // 假设接收的数据不超过256字节

    while (1) {
        angleCount = 0;
        printf("输入旋转角度(0-360°, 使用空格隔开, 对应不同的电机ID): ");
        fgets(input, sizeof(input), stdin);
        token = strtok(input, " ");
        int validInput = 1;
        while (token != NULL) {
            double angle = atof(token);
            if (angle >= 0 && angle <= 360) {
                angles[angleCount++] = angle;
            } else {
                fprintf(stderr, "角度值必须在0到360之间。\n");
                validInput = 0;
                break;
            }
            token = strtok(NULL, " ");
        }

        if (!validInput) {
            continue; // 重新输入角度值
        }

        if (motorIDCount != angleCount) {
            fprintf(stderr, "电机ID数量和角度值数量不匹配。\n");
            continue; // 重新输入
        }

        for (int i = 0; i < motorIDCount; ++i) {
            uint8_t motorID = motorIDs[i];
            uint64_t pos = (uint64_t)(angles[i] * 100);
            uint8_t hexArray[14] = {0x3E, 0xA3, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            hexArray[2] = motorID;
            hexArray[4] = calculateChecksum(hexArray, 0, 3);

            for (int j = 0; j < 8; ++j) {
                hexArray[5 + j] = *((uint8_t*)(&pos) + j);
            }
            hexArray[13] = calculateChecksum(hexArray, 5, 12);

            // 将hexArray整合成连续的字符串
            char hexString[29];
            for (int k = 0; k < 14; ++k) {
                sprintf(&hexString,[k * 2], "%02X", hexArray[k]);
            }
            hexString[28] = '\n';  // 添加新行符
            printf("Data to send: %s\n", hexString);

            // 转换为实际的十六进制字节数组
            uint8_t dataToSend[14];
            size_t dataToSendLen;
            String2Hex(hexString, dataToSend, &dataToSendLen);

            if (sendData(serialPort, dataToSend, dataToSendLen)) {
                printf("Data sent successfully to motor ID %d.\n", motorID);
                if (receiveData(serialPort, buffer, sizeof(buffer) - 1, 500)) { // 超时时间500ms
                    // 将接收到的数据转换为字符串并输出
                    printf("Received data: %s\n", buffer);
                } else {
                    printf("No data received within timeout period.\n");
                }
            } else {
                fprintf(stderr, "Failed to send data to motor ID %d.\n", motorID);
            }
        }
    }

    closeSerialPort(serialPort);
    return 0;
}
