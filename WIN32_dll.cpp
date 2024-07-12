#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <windows.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <windows.h>

using namespace std;

extern "C" __declspec(dllexport) void getHexData(int motorID, double angle, char* hexData) {
    // Calculate checksum function
    auto calculateChecksum = [](uint8_t byteArray[], int start, int end) {
        int sum = 0;
        for (int i = start; i <= end; i++) {
            sum += byteArray[i];
        }
        while (sum > 0xffff)
            sum = (sum & 0xffff) + (sum >> 16);
        return sum;
    };

    // Prepare data
    uint64_t pos = static_cast<uint64_t>(angle * 100);
    uint8_t hexArray[14] = {0x3E, 0xA3, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    hexArray[2] = motorID;
    hexArray[4] = calculateChecksum(hexArray, 0, 3);
    for (int j = 0; j < 8; ++j) {
        hexArray[5 + j] = *((uint8_t*)(&pos) + j);
    }
    hexArray[13] = calculateChecksum(hexArray, 5, 12);

    // Convert to string
    stringstream hexStringStream;
    for (int k = 0; k < 14; ++k) {
        hexStringStream << uppercase << hex << setw(2) << setfill('0') << (int)hexArray[k];
    }

    // Copy the result to the output buffer
    string hexString = hexStringStream.str();
    strcpy(hexData, hexString.c_str());
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
