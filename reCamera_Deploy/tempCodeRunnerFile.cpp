if (sendData(hSerial, dataToSend.data(), dataToSend.size())) {
                cout << "Data sent successfully to motor ID " << (int)motorID << "." << endl;
                if (receiveData(hSerial, buffer, sizeof(buffer) - 1, 500)) { // 超时时间500ms
                    // 将接收到的数据转换为字符串并输出
                    cout << "Received data: " << buffer << endl;
                } else {
                    cout << "No data received within timeout period." << endl;
                }
            } else {
                cerr << "Failed to send data to motor ID " << (int)motorID << "." << endl;
            }