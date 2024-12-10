import machine
import os
 
def boot_main():
    with open('main.py', 'r') as f:
        exec(f.read())
 
if 'main.py' not in os.listdir('/'):
    print('No main.py found in filesystem')
else:
    print('exec main')
    boot_main()

