import time
import board
import busio
import digitalio
import neopixel
import adafruit_bno08x
from adafruit_bno08x import (
    BNO_REPORT_ACCELEROMETER,
    BNO_REPORT_GYROSCOPE,
    BNO_REPORT_ROTATION_VECTOR,
    BNO_REPORT_SHAKE_DETECTOR,
)
from adafruit_bno08x.i2c import BNO08X_I2C
#import mount_sd

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

bno.enable_feature(adafruit_bno08x.BNO_REPORT_ACCELEROMETER)
bno.enable_feature(adafruit_bno08x.BNO_REPORT_GYROSCOPE)
bno.enable_feature(adafruit_bno08x.BNO_REPORT_MAGNETOMETER)
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
            recording = not recording # toggle on
            pixel[0] = (5,0,0) # red
            initial_time = time.monotonic()
        try:
            with open("/data.txt", "a") as f:
                while switch.value == False:
                    accel_x, accel_y, accel_z = bno.acceleration  # pylint:disable=no-member
                    gyro_x, gyro_y, gyro_z = bno.gyro  # pylint:disable=no-member
                    mag_x, mag_y, mag_z = bno.magnetic  # pylint:disable=no-member
                    #quat_i, quat_j, quat_k, quat_real = bno.quaternion  # pylint:disable=no-member
                    time_stamp = time.monotonic() - initial_time
                    f.write("{:.2f}, {:.2f}, {:.2f}, ".format(accel_x, accel_y, accel_z))
                    f.write("{:.2f}, {:.2f}, {:.2f}, ".format(gyro_x, gyro_y, gyro_z))
                    f.write("{:.2f}, {:.2f}, {:.2f}, ".format(mag_x, mag_y, mag_z))
                    #f.write("{:.2f}i, {:.2f}j, {:.2f}k, {:.2f}r | ").format(quat_i, quat_j, quat_k, quat_real))
                    f.write("{:.4f},\n".format(time_stamp))
        except OSError as e:
            if e.args[0] == 28: # out of flash memory
                pixel[0] = (100,100,100)
            else:
                pixel[0] = (0,0,100) # other error, likely read-only
    else:
        if recording:
            recording = not recording # toggle off
            pixel[0] = (0,10,0) # green, standby
