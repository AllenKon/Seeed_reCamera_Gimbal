import ctypes
import os
import serial

dll_path = os.path.abspath("deploy.dll")
motor_dll = ctypes.CDLL(dll_path)

motor_dll.getHexData.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_char_p]
motor_dll.getHexData.restype = None

def get_motor_hex_data(motor_id, angle):
    hex_data = ctypes.create_string_buffer(29)
    motor_dll.getHexData(motor_id, angle, hex_data)
    return hex_data.value.decode('utf-8')

# Example 
port = "COM5"
baudrate = 115200
ser = serial.Serial(port, baudrate, timeout=1)
try:
    while True:
        # Enter motor degree
        angle = list(map(int, input("Enter angles for motors 1 and 2: ").split()))
        if max(angle)>360 or min(angle)<0:
            print("Please enter angles in the range of 0 to 360")
            continue
        motor_ids = [1, 2]  #Set motor id

        for i, motor_id in enumerate(motor_ids):
            hex_data = get_motor_hex_data(motor_id, angle[i])
            print(f"Motor ID: {motor_id}, Angle: {angle[i]}, Hex Data: {hex_data}")

            # Send serial data
            data_bytes = bytes.fromhex(hex_data)
            ser.write(data_bytes)

except KeyboardInterrupt:
    print("Exiting...")

finally:
    ser.close()
