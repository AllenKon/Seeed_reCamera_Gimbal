#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT_NAME "/dev/ttyS0"
#define SEND_SCRIPT "./send_data.sh"
#define RECEIVE_SCRIPT "./receive_data.sh"

int sendDataViaShell(const char* serialPort, const char* data) {
    char command[512];
    snprintf(command, sizeof(command), "%s %s \"%s\"", SEND_SCRIPT, serialPort, data);
    printf("Executing command: %s\n", command); // 调试信息
    int result = system(command);
    if (result == -1) {
        perror("system");
        return 0;
    }
    return 1;
}

int receiveDataViaShell(const char* serialPort, char* buffer, int bufferSize, int timeout) {
    char command[512];
    snprintf(command, sizeof(command), "%s %s %d", RECEIVE_SCRIPT, serialPort, timeout);

    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return 0;
    }

    // Initialize buffer to avoid garbage values
    memset(buffer, 0, bufferSize);

    if (fgets(buffer, bufferSize - 1, fp) == NULL) {
        if (feof(fp)) {
            fprintf(stderr, "End of file reached without data.\n");
        } else if (ferror(fp)) {
            perror("fgets failed");
        }
        pclose(fp);
        return 0; // No data received or error occurred
    }

    pclose(fp);

    return 1; // Data successfully received
}


int main() {
    char buffer[256];

    // 测试发送数据
    const char* testData = "Hello, Serial Port!";
    if (sendDataViaShell(PORT_NAME, testData)) {
        printf("Data sent successfully.\n");
    } else {
        fprintf(stderr, "Failed to send data.\n");
        return -1;
    }

    // 等待并接收数据
    sleep(1);

    if (receiveDataViaShell(PORT_NAME, buffer, sizeof(buffer) - 1, 2)) {
        printf("Received data: %s\n", buffer);
    } else {
        printf("No data received or failed to receive data.\n");
    }

    return 0;
}
