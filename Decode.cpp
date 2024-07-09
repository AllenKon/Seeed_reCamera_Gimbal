#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <vector>

using namespace std;

// Convert byte arrays to decimal values (in reverse order)
uint64_t bytesToDec(const vector<uint8_t>& bytes) {
    uint64_t result = 0;
    for (auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
        result = (result << 8) | *it;
    }
    return result;
}

int main() {
    vector<uint8_t> buffer(20);  
    uint8_t id = 0x01; // ID is set according to your needs
    // Prints the received raw hexadecimal data
    cout << "Received Data: ";
    for (const auto& byte : buffer) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
    }
    cout << endl;

    // 校验前三字节
    if (buffer[0] == 0x3E && buffer[1] == 0xA3) {
        if (id == buffer[2]) {
            // Extract the 6th to 13th bytes of data
            vector<uint8_t> subBuffer(buffer.begin() + 5, buffer.begin() + 12);

            // Print the extracted data in reverse order
            cout << "Reversed Data: ";
            for (const auto& byte : subBuffer) {
                cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
            }
            cout << endl;

            // Convert extracted data to decimal values (reverse order)
            uint64_t rawValue = bytesToDec(subBuffer);
            double angle = rawValue / 100.0;

            cout << "Decoded Angle: " << angle << " degrees" << endl;
        } else {
            cout << "Invalid ID";
        }
    } else {
        cout << "Invalid data";
    }

    return 0;
}
