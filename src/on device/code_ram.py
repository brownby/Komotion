import time
import board
import busio
import digitalio
import neopixel
import adafruit_bno08x
from adafruit_bno08x.i2c import BNO08X_I2C
from ulab import numpy as np

dimens = 6
N = 2000

pixel = neopixel.NeoPixel(board.NEOPIXEL, 1)
pixel[0] = (0,5,0)
pixel.show()

led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT

switch = digitalio.DigitalInOut(board.A0)
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

recording = False

i2c = busio.I2C(board.SCL, board.SDA, frequency=400000)
bno = BNO08X_I2C(i2c, address = 0x4A)

bno.initialize()
print("initilized imu")
bno.begin_calibration()
print("began calibration")

data = np.zeros((N,dimens+1));

bno.enable_feature(adafruit_bno08x.BNO_REPORT_ACCELEROMETER)
bno.enable_feature(adafruit_bno08x.BNO_REPORT_GYROSCOPE)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_MAGNETOMETER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_LINEAR_ACCELERATION)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_ROTATION_VECTOR)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_GEOMAGNETIC_ROTATION_VECTOR)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_GAME_ROTATION_VECTOR)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_STEP_COUNTER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_STABILITY_CLASSIFIER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_ACTIVITY_CLASSIFIER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_SHAKE_DETECTOR)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_ACCELEROMETER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_GYROSCOPE)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_MAGNETOMETER)

while True:
    if switch.value == False: # constant pulled low
        if not recording:
            data = np.zeros((N,dimens+1)); # +1 for timestamp
            recording = not recording # toggle on
            pixel[0] = (5,0,0) # red
            initial_time = time.monotonic()

        for i in range(0,N-1):
            time_stamp = time.monotonic() - initial_time
            accel_x, accel_y, accel_z = bno.acceleration  # pylint:disable=no-member
            gyro_x, gyro_y, gyro_z = bno.gyro  # pylint:disable=no-member
            data[i,:] = [accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, time_stamp]

            #mag_x, mag_y, mag_z = bno.magnetic  # pylint:disable=no-member
            #quat_i, quat_j, quat_k, quat_real = bno.quaternion  # pylint:disable=no-member

        pixel[0] = (0,0,5) # writing run to file
        time.sleep(1)

        try:
            with open("/data.txt", "a") as f:
                for i in range(0,N-1):
                    f.write("{:.2f}, {:.2f}, {:.2f}, ".format(data[i,0], data[i,1], data[i,2]))
                    f.write("{:.2f}, {:.2f}, {:.2f}, ".format(data[i,3], data[i,4], data[i,5]))
                    f.write("{:.3f},\n".format(data[i,6]))
        except OSError as e:
                pixel[0] = (0,0,5)
                time.sleep(1)
                pixel[0] = (0,0,0)
                time.sleep(1)
                pixel[0] = (0,0,5)
                time.sleep(1)

        recording = not recording
        pixel[0] = (0,5,0)
        time.sleep(1)
        pixel[0] = (0,0,0)
        time.sleep(1)
        pixel[0] = (0,5,0)
        time.sleep(1)

    else:
        if recording:
            recording = not recording # toggle off
            pixel[0] = (0,10,0) # green, standby
