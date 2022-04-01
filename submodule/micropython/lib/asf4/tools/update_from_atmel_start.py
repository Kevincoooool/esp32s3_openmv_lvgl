import requests
import zipfile
import os.path
import shutil
import os
import os.path
import sys
import subprocess

if not subprocess.check_output(['git', 'rev-parse', '--show-toplevel'], universal_newlines=True).strip() == os.getcwd():
    print('Please run "{}" from the top directory in the asf4 repo.'.format(sys.argv[0]))
    sys.exit(1)

# Change .gitignore if you change these dir names.
DOWNLOADED_ZIP_DIR = 'downloaded-zip'
DOWNLOADED_DIR = 'downloaded'

for chip in ['samd21', 'samd51']:
    r = None
    os.makedirs(DOWNLOADED_ZIP_DIR, exist_ok=True)
    filename = os.path.join(DOWNLOADED_ZIP_DIR, chip + '.zip')
    if os.path.isfile(filename):
        print('NOTE:', filename, 'already downloaded. Delete it and re-run if you want to re-download')
    else:
        print("Downloading", filename, "...")
        with open('tools/' + chip + '.json', 'r') as project_json:
            headers = {'content-type': 'text/plain'}
            r = requests.post('http://start.atmel.com/api/v1/generate/?format=atzip&compilers=[atmel_studio,make]&file_name_base=My%20Project', headers=headers, data=project_json)
        if not r.ok:
            # Double check that the JSON is minified. If it's not, you'll get a 404.
            print(r.text)
            sys.exit(1)
        with open(filename, 'wb') as out:
            out.write(r.content)

    # Extract to a temporary location and normalize before replacing the existing location.
    z = zipfile.ZipFile(filename)
    downloaded_chip_dir = os.path.join(DOWNLOADED_DIR, chip)
    # Clean up old zip extraction.
    if os.path.isdir(downloaded_chip_dir):
        shutil.rmtree(downloaded_chip_dir)
    print("Unzipping ...")
    z.extractall(downloaded_chip_dir)

    # Remove all carriage returns.
    for dirpath, dirnames, filenames in os.walk(downloaded_chip_dir):
        for fn in filenames:
            fn = os.path.join(dirpath, fn)
            subprocess.run(['sed', '-i', 's/\r//g', fn])

    # Move files to match SAMD51 structure.
    if chip == 'samd21':
        shutil.move(os.path.join(downloaded_chip_dir, 'samd21a/include'), downloaded_chip_dir)
        shutil.move(os.path.join(downloaded_chip_dir, 'samd21a/gcc/gcc'), os.path.join(downloaded_chip_dir, 'gcc'))
        shutil.move(os.path.join(downloaded_chip_dir, 'samd21a/gcc/system_samd21.c'), os.path.join(downloaded_chip_dir, 'gcc'))

    print("Updating",chip,"from",downloaded_chip_dir)
    subprocess.run(['rsync', '-r', '--delete', downloaded_chip_dir + '/', chip], check=True)
