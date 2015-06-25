#include <iostream>
#include <sstream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <proj_api.h>

using namespace std;
using namespace cv;

typedef Mat_<Vec4b> Image;

struct point {
    double x,y;
    point(double _x, double _y): x(_x), y(_y) {}
};

ostream& operator<<(ostream& s, point p) {
    s << "(" << p.x << " " << p.y << ")";
    return s;
}

point transform(const projPJ& from, const projPJ& to, point xy) {
    pj_transform(from, to, 1, 1, &xy.x, &xy.y, NULL);
    return xy;
}

Image transformProjection(const Image& source, point earthCenterDeg, point sourceCenter, double sourcePixelPerRad, int targetHeight) {
    double sourcePixelPerDeg = sourcePixelPerRad / 180 * M_PI;
    point earthCenterRad{
        earthCenterDeg.x / 180*M_PI, 
        earthCenterDeg.y / 180*M_PI
    };

    
    projPJ earthProj = pj_init_plus("+proj=latlong");
    if (!earthProj) {
        cout << "Can't create earthProj!" << endl;
        exit(1);
    }
    stringstream sourceCode;
    sourceCode.precision(15);
    sourceCode << "+proj=aeqd +R=1 +x_0=0 +y_0=0 +lon_0=" << earthCenterDeg.x << " +lat_0=" << earthCenterDeg.y;
    projPJ sourceProj = pj_init_plus(sourceCode.str().c_str());
    if (!sourceProj) {
        cout << "Can't create sourceProj!" << endl;
        exit(1);
    }
    projPJ targetProj = pj_init_plus("+init=epsg:3857");
    if (!targetProj) {
        cout << "Can't create targetProj!" << endl;
        exit(1);
    }

    cout << "Center@source: " << transform(earthProj, sourceProj, earthCenterRad) << endl;
    cout << "Center@target: " << transform(earthProj, targetProj, earthCenterRad) << endl;

    point earthRadiusRad{
        source.cols/2.0 / sourcePixelPerRad / cos(earthCenterRad.y), 
        source.rows/2.0 / sourcePixelPerRad
    };
    point earthTopLeftRad{
        earthCenterRad.x - earthRadiusRad.x,
        earthCenterRad.y - earthRadiusRad.y
    };
    point earthBotRightRad{ 
        earthCenterRad.x + earthRadiusRad.x, 
        earthCenterRad.y + earthRadiusRad.y
    };

    point targetTopLeft = transform(earthProj, targetProj, earthTopLeftRad);
    point targetBotRight = transform(earthProj, targetProj, earthBotRightRad);
    int targetWidth = int(targetHeight / (targetBotRight.y - targetTopLeft.y) * (targetBotRight.x - targetTopLeft.x));

    cout << "TargetTopLeft: " << targetTopLeft << endl;
    cout << "TargetBotRight: " << targetBotRight << endl;
    
    cout << "Earth radius: " << earthRadiusRad << endl;
    cout << "EarthTL: " << earthTopLeftRad << endl;
    cout << "EarthBR: " << earthBotRightRad << endl;

    cout << "SourceTopLeft: " << transform(targetProj, sourceProj, targetTopLeft) << endl;
    cout << "SourceBotRight: " << transform(targetProj, sourceProj, targetBotRight) << endl;

    cout << "target size: " << targetWidth << " " << targetHeight << endl;

    Image target(targetHeight, targetWidth, Vec4b(0,0,0,0));
    for (int targetYpx=0; targetYpx<target.rows; targetYpx++)
        for (int targetXpx=0; targetXpx<target.cols; targetXpx++) {
            point targetXY {
                targetTopLeft.x + ((targetBotRight.x-targetTopLeft.x) * targetXpx) / targetWidth,
                targetTopLeft.y + ((targetBotRight.y-targetTopLeft.y) * (targetHeight-targetYpx)) / targetHeight
            };
            point sourceXY = transform(targetProj, sourceProj, targetXY);
            int sourceXpx = int(sourceXY.x * sourcePixelPerRad + sourceCenter.x + 0.5);
            int sourceYpx = int(-sourceXY.y * sourcePixelPerRad + sourceCenter.y + 0.5);
            if (sourceXpx>=0 && sourceXpx<source.cols && sourceYpx>=0 && sourceYpx<source.rows)
                target(targetYpx, targetXpx) = source(sourceYpx, sourceXpx);
        }
    
/*
    point targetCenter = transform(sourceProj, targetProj, point{0,0});
    int targetXpx = int((targetCenter.x - targetTopLeft.x)/(targetBotRight.x-targetTopLeft.x)*targetWidth + 0.5);
    int targetYpx = int((targetBotRight.y-targetCenter.y)/(targetBotRight.y-targetTopLeft.y)*targetHeight + 0.5);
    cout << "targetCenter: " << targetXpx << " " << targetYpx << endl;
    target(targetYpx, targetXpx) = Vec4b(255,0,255,255);
*/        
    cout << "Corner-coordinates of result: " << targetTopLeft << " " << targetBotRight << endl;

    return target;
}

std::vector<int> makeCumulative(const std::vector<int>& v) {
    static const int avgStep = 50;

    std::vector<int> res(v.size(), 0);
    for (int i=0; i<v.size(); i++)
        for (int j=-avgStep; j<=avgStep; j++) { 
            int pos = i+j;
            if (pos<0) pos=0;
            if (pos>=v.size()) pos=v.size()-1;
            res[i]+=v[pos];
        }
    return res;
}

point detectCenter(const Image& im) {
    std::vector<int> cntLineX(im.cols, 0);
    std::vector<int> cntLineY(im.rows, 0);
    std::vector<int> cntBgX(im.cols, 0);
    std::vector<int> cntBgY(im.rows, 0);
    for (int y=0; y<im.rows; y++)
        for (int x=0; x<im.cols; x++) {
                if (im(y, x) == Vec4b(164,160,160,255)) { // if this is background
                    cntBgX[x]++;
                    cntBgY[y]++;
                }
                if (im(y, x) == Vec4b(115,115,115,255)) {
                    cntLineX[x]++;
                    cntLineY[y]++;
                }
        }
    std::vector<int> cumBgX = makeCumulative(cntBgX);
    std::vector<int> cumBgY = makeCumulative(cntBgY);
    
    point result{0,0};
    for(int x=0; x<cntLineX.size(); x++)
        if (cntLineX[x] > im.rows *7/10) {
            cout << "X-candidate: " << x << " " << double(cntLineX[x]) / im.rows << " " << cumBgX[x] << endl;
            if (cumBgX[x] < cumBgX[result.x])
                result.x = x;
        }
    for(int y=0; y<cntLineY.size(); y++)
        if (cntLineY[y] > im.cols *7/10) {
            cout << "Y-candidate: " << y << " " << double(cntLineY[y]) / im.cols << " " << cumBgY[y] << endl;
            if (cumBgY[y] < cumBgY[result.y])
                result.y = y;
        }
    cout << "Detected center @ " << result << endl;
    return result;
}

int main(int argc, char* argv[]) {
    cout.precision(10);
    /*
    #earthCenterDeg = (43.97838, 56.293779) #lat, lon
    #sourceName = 'UVKNizhny-crop.png'
    #targetName = 'UVKNizhny-merc.png'
    #sourceCenter = (588, 500) # x,y

    #earthCenterDeg = (41.016018, 57.809240) #lon, lat
    #sourceName = 'UVKKostroma-crop.png'
    #targetName = 'UVKKostroma-merc.png'
    #sourceCenter = (583, 493) # x,y

    point earthCenterDeg {37.549006,55.648246};
    std::string sourceName = "UVKProfsoyuz-crop.png";
    std::string targetName = "UVKProfsoyuz-merc.png";
    point sourceCenter {576, 459};
    */
    point earthCenterDeg {stof(argv[1]), stof(argv[2])};
    std::string sourceName(argv[3]);
    std::string targetName(argv[4]);

    double sourcePixelPerRad = 12750;
    int targetHeight = 1000;
    
    // Setup a rectangle to define your region of interest
    Rect cropArea(185, 54, 1365-185, 1014-54);
    //------------------------

    Image source = imread(sourceName, -1);
    source = source(cropArea);
    
    point sourceCenter = detectCenter(source);
    
    Image result = transformProjection(source, earthCenterDeg, sourceCenter, sourcePixelPerRad, targetHeight);

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    imwrite(targetName, result, compression_params);
    
    return 0;
}