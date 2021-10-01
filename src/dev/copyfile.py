"""
   BNO08x data logger
   for STM32F405 feather

   Writes requested reports to a text file (csv)
   and allows for configuration of requested sample rate

   modified 28 September 2021
   by J. Evan Smith

   Active Learning Labs, Electrical Engineering
   https://www.seas.harvard.edu/active-learning-labs
 """

import os
import time
import board
import busio
import sdioio
import storage
import neopixel
import digitalio
import supervisor
import adafruit_bno08x
from adafruit_bno08x.i2c import BNO08X_I2C

debug = False # toggle to enable print statements
dimens = 6

def print_d(str_input,d_bool):
    if d_bool:
        print(str_input)

def copy_file(src_filename, dst_filename):

    """
    need to move 100kB or so at a time and use seek to return and pick up where left off
    """

    BUFFER_SIZE = 100
    try:
        print_d("Attempting to copy file ...", debug)
        with open(src_filename, 'r') as src_file:
            buf = src_file.readlines()
            src_file.
            print_d("{}".format(buf), debug)
            src_file.close()
        time.sleep(1)
        storage.umount("/sd")
        storage.remount("/",False)
        with open(dst_filename, 'w') as dst_file:
            for line in buf:
                dst_file.write(line)
        dst_file.close()
        storage.umount("/")
        storage.mount("/sd")
    except:
        print_d("Copy failed ...", debug)
        return False

pixel = neopixel.NeoPixel(board.NEOPIXEL, 1)
pixel[0] = (0,0,5) # blue, "thinking" state
pixel.show()

led = digitalio.DigitalInOut(board.D13) # red LED
led.direction = digitalio.Direction.OUTPUT

print_d("Initializing SD card ...",debug)
sdCard = sdioio.SDCard(
    clock=board.SDIO_CLOCK,
    command=board.SDIO_COMMAND,
    data=board.SDIO_DATA,
    frequency=25000000)

vfs = storage.VfsFat(sdCard)
storage.mount(vfs, "/sd")

led.value = True
tic = time.monotonic()

while (time.monotonic()-tic<5):
    if supervisor.runtime.serial_bytes_available:
        value = input()
        if value == "d":
            debug = True
            print_d("Starting with debug ...",debug)
            break

led.value = False

switch = digitalio.DigitalInOut(board.A0) # state toggle
switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

button = digitalio.DigitalInOut(board.D5) # initiate & control calibration
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

button_low = digitalio.DigitalInOut(board.D9) # ground / always low
button_low.direction = digitalio.Direction.OUTPUT
button_low.value = False

recording = False # state bool

print_d("Connecting to BNO08x ...",debug)

i2c = busio.I2C(board.SCL, board.SDA, frequency=400000) # stemma
bno = BNO08X_I2C(i2c, address = 0x4A)

print_d("Initializing IMU ...",debug)
bno.initialize()

"""
MODIFY CODE IN BLOCK BELOW

add and/or remove comments here to enable / disable reports
at the moment, 6-axis A+G or rotation vector are preferred
"""

print_d("Configuring reports ...",debug)

bno.enable_feature(adafruit_bno08x.BNO_REPORT_ACCELEROMETER)
bno.enable_feature(adafruit_bno08x.BNO_REPORT_GYROSCOPE)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_MAGNETOMETER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_ROTATION_VECTOR)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_ACCELEROMETER)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_GYROSCOPE)
#bno.enable_feature(adafruit_bno08x.BNO_REPORT_RAW_MAGNETOMETER)

print_d("Entering standby state ...",debug)
pixel[0] = (0,5,0) # green, standby state

while True:
    if switch.value == False: # constant pulled low
        if not recording:
            recording = not recording # toggle on
            pixel[0] = (5,0,0) # red, recording state
            print_d("Starting recording ...", debug)
            initial_time = time.monotonic()
        try:
            with open("/sd/data_000.txt", "a") as f:
                while switch.value == False:

                    """
                    MODIFY CODE IN BLOCK BELOW

                    for raw reports, add raw_ to the beginning of bno.raw_<type>,
                    so bno.raw_acceleration, for example, in file write block below

                        variables in format() populate braces {} in order
                        :.Nf --> N controls number of decimals recorded, f = float
                        time.monotonic() - initial_time = timestamp relative to recording start
                        \n begins a new line
                    """

                    accel_x, accel_y, accel_z = bno.acceleration  # pylint:disable=no-member
                    gyro_x, gyro_y, gyro_z = bno.gyro  # pylint:disable=no-member
                    #mag_x, mag_y, mag_z = bno.magnetic  # pylint:disable=no-member
                    #quat_i, quat_j, quat_k, quat_real = bno.quaternion  # pylint:disable=no-member
                    f.write("{:.2f},{:.2f},{:.2f},".format(accel_x, accel_y, accel_z))
                    f.write("{:.2f},{:.2f},{:.2f},".format(gyro_x, gyro_y, gyro_z))
                    #f.write("{:.2f},{:.2f},{:.2f}, ".format(mag_x, mag_y, mag_z))
                    #f.write("{:.2f},{:.2f},{:.2f},{:.2f},").format(quat_i, quat_j, quat_k, quat_real))
                    f.write("{:.3f},\n".format(time.monotonic() - initial_time))

        except OSError as e:
            pixel[0] = (0,0,100) # other error, likely read-only
    else:
        if recording:
            recording = not recording # toggle off
            pixel[0] = (0,0,5) # blue, "thinking" state
            f.close()
            time.sleep(1) # time for f.close
            copy_file("/sd/data_000.txt","/data/data_000.txt")
            time.sleep(1)
            pixel[0] = (0,5,0) # green, standby
        if not button.value:
            pixel[0] = (0,0,5) # blue
            print("began calibration")
            bno.begin_calibration()
            print(bno.calibration_status)
            time.sleep(1)
            bno.save_calibration_data()
            pixel[0] = (0,5,0)


