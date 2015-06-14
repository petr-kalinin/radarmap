#!/usr/bin/python3
import pyproj
from PIL import Image
from math import cos, pi

earthCenterDeg = (43.97838, 56.293779) #lat, lon
sourceName = 'UVKNizhny-crop.png'
sourceCenter = (588, 500) # x,y
sourcePixelPerRad = 12600
sourcePixelPerDeg = 12600 / 180 * pi
targetSize = 500 #1500
targetName = 'UVKNizhny-merc.png'
#-----------

earthCenterRad = (earthCenterDeg[0] / 180*pi, earthCenterDeg[1] / 180*pi) #lat, lon

source = Image.open(sourceName)
sourceSize = source.size

earthProj = pyproj.Proj(proj='latlong')
sourceCode = ('+proj=aeqd +R=1 +x_0=0 +y_0=0 +lon_0=%8.5f +lat_0=%8.5f' % earthCenterDeg)
print(sourceCode)
sourceProj = pyproj.Proj(sourceCode)
targetProj = pyproj.Proj(init='epsg:3857')
#targetProj = pyproj.Proj('+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs')

print('Center@source: ',pyproj.transform(earthProj, sourceProj, earthCenterDeg[0], earthCenterDeg[1]))
print('Center@target: ',pyproj.transform(earthProj, targetProj, earthCenterDeg[0], earthCenterDeg[1]))

earthRadiusDeg = (sourceSize[0]/2 / sourcePixelPerDeg / cos(earthCenterRad[1]), sourceSize[1]/2 / sourcePixelPerDeg)
earthTopLeftDeg = (earthCenterDeg[0] - earthRadiusDeg[0], earthCenterDeg[1] - earthRadiusDeg[1])
earthBotRightDeg = (earthCenterDeg[0] + earthRadiusDeg[0], earthCenterDeg[1] + earthRadiusDeg[1])

targetTopLeft = pyproj.transform(earthProj, targetProj, earthTopLeftDeg[0], earthTopLeftDeg[1])
targetBotRight = pyproj.transform(earthProj, targetProj, earthBotRightDeg[0], earthBotRightDeg[1])

print(targetTopLeft)
print(targetBotRight)
print(pyproj.transform(targetProj, sourceProj, targetTopLeft[0], targetTopLeft[1]))
print(pyproj.transform(targetProj, sourceProj, targetBotRight[0], targetBotRight[1]))
kstr = pyproj.transform(earthProj, sourceProj, 40.93, 57.77)
kstrXpx = int(kstr[0] * sourcePixelPerRad + sourceCenter[0])
kstrYpx = int(-kstr[1] * sourcePixelPerRad + sourceCenter[1])
print('Kostroma in source: ',kstr)
print('Kostroma in source: ',kstrXpx, kstrYpx)


bb = pyproj.transform(earthProj, sourceProj, 45.30907, 55.007129)
bbXpx = int(bb[0] * sourcePixelPerRad + sourceCenter[0])
bbYpx = int(-bb[1] * sourcePixelPerRad + sourceCenter[1])
print('BB in source: ',bb)
print('BB in source: ',bbXpx, bbYpx)

target = Image.new('RGBA', (targetSize, targetSize), (0,0,0,1))
for targetXpx in range(targetSize):
    for targetYpx in range(targetSize):
        targetX = targetTopLeft[0] + (targetBotRight[0]-targetTopLeft[0]) * targetXpx / targetSize
        targetY = targetTopLeft[1] + (targetBotRight[1]-targetTopLeft[1]) * (targetSize - targetYpx) / targetSize
        (sourceX, sourceY) = pyproj.transform(targetProj, sourceProj, targetX, targetY)
        sourceXpx = int(sourceX * sourcePixelPerRad + sourceCenter[0])
        sourceYpx = int(-sourceY * sourcePixelPerRad + sourceCenter[1])
        print("target", targetXpx, targetYpx, "source", sourceXpx, sourceYpx)
        if (sourceXpx >= 0) and (sourceXpx < sourceSize[0]) and (sourceYpx >= 0) and (sourceYpx < sourceSize[1]):
            target.putpixel((targetXpx, targetYpx), source.getpixel((int(sourceXpx), int(sourceYpx))))
        pass
        
target.save(targetName)
