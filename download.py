#!/usr/bin/python3
import urllib.request
import hashlib
import os
import os.path
import shutil
import datetime

files = [('http://www.meteorad.ru/data/UVKNizhny.png', 'Nizhny'),
         ('http://www.meteorad.ru/data/UVKKostroma.png', 'Kostroma'),
         ('http://www.meteorad.ru/data/UVKProfsoyuz.png', 'Moscow')]
workDir = 'images'

os.chdir(workDir)
for url, id in files:
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
    now = datetime.datetime.now(datetime.timezone.utc)
    timeMark = now.strftime('%Y%m%d-%H%M')
    permFile = id + '-' + timeMark + '.png'
    shutil.copy(latestFile, permFile)