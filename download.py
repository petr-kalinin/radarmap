#!/usr/bin/python3
# (c) Petr Kalinin, https://github.com/petr-kalinin/radarmap, GNU AGPL
import urllib.request
import hashlib
import os
import os.path
import shutil
import datetime
import subprocess
import json
import re

files = [('http://www.meteorad.ru/data/UVKNizhnyNovgorod.png', 'Nizhny', '43.97798', '56.296179'),
         ('http://www.meteorad.ru/data/UVKKostroma.png', 'Kostroma', '41.017018', '57.800240'),
         ('http://www.meteorad.ru/data/UVKProfsoyuz.png', 'Moscow', '37.549006', '55.648246')]
workDir = 'images'
imagesFileName = '../images.json'

def parseCorners(output):
    match = re.search(r'Corner-coordinates of result: \(([0-9.]+) ([0-9.]+)\) \(([0-9.]+) ([0-9.]+)\)', output.decode())
    if not match:
        return None
    return match.group(1,2,3,4)

os.chdir(workDir)
for url, id, a, b in files:
    print("Processing " + id)
    tempFile = id+'-temp.png'
    latestFile = id+'-latest.png'
    in32file = id+'-temp-in32.png'
    urllib.request.urlretrieve(url, tempFile)
    if (os.path.isfile(latestFile)):
        hashNew = hashlib.sha256(open(tempFile, 'rb').read()).digest()
        hashLatest = hashlib.sha256(open(latestFile, 'rb').read()).digest()
        if (hashNew == hashLatest):
            print("Hash match, skipping")
            os.remove(tempFile)
            continue

    shutil.move(tempFile, latestFile)
    # the following line calls ImageMagic's convert, not ../convert
    subprocess.call(["convert", latestFile, 'PNG32:'+in32file])
    
    latestMerc = id+'-merc-latest.png'
    stencil = '../stencil/' + id + '-stencil.png'
    try:
        print("../convert", a, b, in32file, latestMerc, stencil)
        output = subprocess.check_output(["../convert", a, b, in32file, latestMerc, stencil])
    except subprocess.CalledProcessError:
        continue
    corners = parseCorners(output)
    
    now = datetime.datetime.now(datetime.timezone.utc)
    timeMark = now.strftime('%Y%m%d-%H%M')
    permFile = id + '-merc-' + timeMark + '.png'
    shutil.copy(latestMerc, permFile)
    
    with open(imagesFileName) as imagesFile:    
        images = json.load(imagesFile)
    if (not (id in images.keys())):
        images[id]={"images" : []}
    images[id]["images"].insert(0, timeMark)
    #if (not ("corners" in images[id].keys())) and (corners):
    if (corners):
        images[id]["corners"] = corners
    with open(imagesFileName,'w') as imagesFile:    
        json.dump(images, imagesFile)
    