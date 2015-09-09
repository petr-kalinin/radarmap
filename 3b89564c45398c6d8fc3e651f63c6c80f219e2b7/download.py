#!/usr/bin/python3
import urllib.request
import hashlib
import os
import os.path
import shutil
import datetime
import subprocess

files = [('http://www.meteorad.ru/data/UVKNizhny.png', 'Nizhny', '43.97798', '56.296179'),
         ('http://www.meteorad.ru/data/UVKKostroma.png', 'Kostroma', '41.017018', '57.800240'),
         ('http://www.meteorad.ru/data/UVKProfsoyuz.png', 'Moscow', '37.549006', '55.648246')]
workDir = 'images'

os.chdir(workDir)
for url, id, a, b in files:
    print("Processing " + id)
    tempFile = id+'-temp.png'
    latestFile = id+'-latest.png'
    urllib.request.urlretrieve(url, tempFile)
    if (os.path.isfile(latestFile)):
        hashNew = hashlib.sha256(open(tempFile, 'rb').read()).digest()
        hashLatest = hashlib.sha256(open(latestFile, 'rb').read()).digest()
        if (hashNew == hashLatest):
            print("Hash match, skipping")
            os.remove(tempFile)
            continue

    shutil.move(tempFile, latestFile)
    latestMerc = id+'-merc-latest.png'
    stencil = '../stencil/' + id + '-stencil.png'
    subprocess.call(["../convert", a, b, latestFile, latestMerc, stencil])
    now = datetime.datetime.now(datetime.timezone.utc)
    timeMark = now.strftime('%Y%m%d-%H%M')
    permFile = id + '-merc-' + timeMark + '.png'
    shutil.copy(latestMerc, permFile)