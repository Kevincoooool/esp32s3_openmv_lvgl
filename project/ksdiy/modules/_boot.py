import gc,esp32
import os
import machine
from esp32 import Partition
try:
    sd = machine.SDCard(slot=1)
    os.mount(sd, '/')
except OSError as e:
    print('SD card not exists!')
    bdev1 = Partition.find(Partition.TYPE_DATA, label="vfs")
    vfs_dev = bdev1[0]
    try:
        if vfs_dev:
            vfs1 = os.VfsFat(vfs_dev)
            os.mount(vfs1, "/")
    except OSError as e:
        os.VfsFat.mkfs(vfs_dev)
        vfs1 = os.VfsFat(vfs_dev)
        os.mount(vfs1, "/")    
if ".openmv_disk" not in os.listdir('/'):
    with open(".openmv_disk", "w") as f:
        f.close()
