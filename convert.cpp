#include <iostream>
#include <sstream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <proj_api.h>

using namespace std;
using namespace cv;

int main() {
    /*
    #earthCenterDeg = (43.97838, 56.293779) #lat, lon
    #sourceName = 'UVKNizhny-crop.png'
    #targetName = 'UVKNizhny-merc.png'
    #sourceCenter = (588, 500) # x,y

    #earthCenterDeg = (41.016018, 57.809240) #lon, lat
    #sourceName = 'UVKKostroma-crop.png'
    #targetName = 'UVKKostroma-merc.png'
    #sourceCenter = (583, 493) # x,y
    */

    std::pair<float, float> earthCenterDeg {37.549006,55.648246};
    std::string sourceName = "UVKProfsoyuz-crop.png";
    std::string targetName = "UVKProfsoyuz-merc.png";
    std::pair<float, float> sourceCenter {576, 459};

    float sourcePixelPerRad = 12750;
    float sourcePixelPerDeg = sourcePixelPerRad / 180 * M_PI;
    int targetHeight = 1000;
    //------------------------

    std::pair<float, float> earthCenterRad{
        earthCenterDeg.first / 180*M_PI, 
        earthCenterDeg.second / 180*M_PI};

    Mat_<Vec4b> source = imread(sourceName, -1);
    std::pair<int, int> sourceSize {source.rows, source.cols};


    projPJ earthProj = pj_init_plus("+proj=latlong");
    if (!earthProj) {
        cout << "Can't create earthProj!" << endl;
        exit(1);
    }
    stringstream sourceCode;
    sourceCode << "+proj=aeqd +R=1 +x_0=0 +y_0=0 +lon_0=" << earthCenterDeg.first << "+lat_0=" << earthCenterDeg.second;
    projPJ sourceProj = pj_init_plus(sourceCode.str().c_str());
    if (!sourceProj) {
        cout << "Can't create sourceProj!" << endl;
        exit(1);
    }
    projPJ targetProj = pj_init_plus("+epsg=3857");
    if (!targetProj) {
        cout << "Can't create targetProj!" << endl;
        exit(1);
    }

    double x = earthCenterDeg.first, y = earthCenterDeg.second;
    pj_transform(earthProj, sourceProj, 1, 1, &x, &y, NULL);
    cout << "Center@source: " << x << " " << y << endl;
/*print('Center@target: ',pyproj.transform(earthProj, targetProj, earthCenterDeg[0], earthCenterDeg[1]))

earthRadiusDeg = (sourceSize[0]/2 / sourcePixelPerDeg / cos(earthCenterRad[1]), sourceSize[1]/2 / sourcePixelPerDeg)
earthTopLeftDeg = (earthCenterDeg[0] - earthRadiusDeg[0], earthCenterDeg[1] - earthRadiusDeg[1])
earthBotRightDeg = (earthCenterDeg[0] + earthRadiusDeg[0], earthCenterDeg[1] + earthRadiusDeg[1])

targetTopLeft = pyproj.transform(earthProj, targetProj, earthTopLeftDeg[0], earthTopLeftDeg[1])
targetBotRight = pyproj.transform(earthProj, targetProj, earthBotRightDeg[0], earthBotRightDeg[1])
targetWidth = int(targetHeight / (targetBotRight[1] - targetTopLeft[1]) * (targetBotRight[0] - targetTopLeft[0]))

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

print('target size', targetWidth, targetHeight)
*/
    int targetWidth = targetHeight;

    Mat_<Vec4b> target(targetHeight, targetWidth, Vec4b(0,0,0,0));
    for (int targetYpx=0; targetYpx<target.rows; targetYpx++)
        for (int targetXpx=0; targetXpx<target.cols; targetXpx++) {
            int sourceXpx = targetXpx * 2;
            int sourceYpx = targetYpx * 2;
            if (sourceXpx>=0 && sourceXpx<source.cols && sourceYpx>=0 && sourceYpx<source.rows)
                target(targetYpx, targetXpx) = source(sourceYpx, sourceXpx);
        }
/*for targetXpx in range(targetWidth):
    for targetYpx in range(targetHeight):
        targetX = targetTopLeft[0] + (targetBotRight[0]-targetTopLeft[0]) * targetXpx / targetWidth
        targetY = targetTopLeft[1] + (targetBotRight[1]-targetTopLeft[1]) * (targetHeight - targetYpx) / targetHeight
        (sourceX, sourceY) = pyproj.transform(targetProj, sourceProj, targetX, targetY)
        sourceXpx = int(sourceX * sourcePixelPerRad + sourceCenter[0])
        sourceYpx = int(-sourceY * sourcePixelPerRad + sourceCenter[1])
        if (targetXpx % 128 == 0) and (targetYpx == 100):
            print(targetXpx, targetYpx)
        if (sourceXpx >= 0) and (sourceXpx < sourceSize[0]) and (sourceYpx >= 0) and (sourceYpx < sourceSize[1]):
            target.putpixel((targetXpx, targetYpx), source.getpixel((int(sourceXpx), int(sourceYpx))))
        pass
  */      
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    imwrite(targetName, target, compression_params);
/*print(targetTopLeft)
print(targetBotRight)
*/
    return 0;
}