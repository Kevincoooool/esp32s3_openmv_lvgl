import os
from flashbdev import bdev


def check_bootsec():
    buf = bytearray(bdev.ioctl(5, 0))  # 5 is SEC_SIZE
    bdev.readblocks(0, buf)
    empty = True
    for b in buf:
        if b != 0xFF:
            empty = False
            break
    if empty:
        return True
    fs_corrupted()


def fs_corrupted():
    import time

    while 1:
        print(
            """\
The filesystem appears to be corrupted. If you had important data there, you
may want to make a flash snapshot to try to recover it. Otherwise, perform
factory reprogramming of MicroPython firmware (completely erase flash, followed
by firmware programming).
"""
        )
        time.sleep(3)


def write_main():
    with open("main.py", "w") as f:
        f.write(
            """\
import sensor, image, lcd
sensor.reset() 
sensor.set_pixformat(sensor.RGB565) 
sensor.set_framesize(sensor.HQVGA) 
while(True):
    lcd.display(sensor.snapshot())
"""
        )
        f.close()

def write_config():
    with open(".openmv_disk", "w") as f:
        f.close()


def setup():
    check_bootsec()
    print("Performing initial setup")
    os.VfsFat.mkfs(bdev)
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/")
    #write_main()
    write_config()
    with open("boot.py", "w") as f:
        f.write(
            """\
# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
#import webrepl
#webrepl.start()
"""
        )
    return vfs
