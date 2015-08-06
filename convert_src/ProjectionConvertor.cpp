#include "ProjectionConvertor.h"

#include <iostream>

#include "palette.h"

const double ProjectionConvertor::sourcePixelPerRad = 12750;
const int ProjectionConvertor::resultHeight = 1000;

ProjectionConvertor::ProjectionConvertor(const Image& source, const point& earthCenterDeg, const point& sourceCenter):
    source_(source),
    result_(),
    earthCenterDeg_(earthCenterDeg),
    sourceCenter_(sourceCenter)
{
    transformProjection();
}

void ProjectionConvertor::transformProjection() {
    std::cout << "sourcePixelPerRad = " << sourcePixelPerRad << std::endl;
    double sourcePixelPerDeg = sourcePixelPerRad / 180 * M_PI;
    point earthCenterRad{
        earthCenterDeg_.x / 180*M_PI, 
        earthCenterDeg_.y / 180*M_PI
    };

    
    projPJ earthProj = pj_init_plus("+proj=latlong");
    if (!earthProj) {
        std::cout << "Can't create earthProj!" << std::endl;
        exit(1);
    }
    std::stringstream sourceCode;
    sourceCode.precision(15);
    sourceCode << "+proj=aeqd +R=1 +x_0=0 +y_0=0 +lon_0=" << earthCenterDeg_.x << " +lat_0=" << earthCenterDeg_.y;
    projPJ sourceProj = pj_init_plus(sourceCode.str().c_str());
    if (!sourceProj) {
        std::cout << "Can't create sourceProj!" << std::endl;
        exit(1);
    }
    projPJ resultProj = pj_init_plus("+init=epsg:3857");
    if (!resultProj) {
        std::cout << "Can't create resultProj!" << std::endl;
        exit(1);
    }

    std::cout << "Center@source_: " << transform(earthProj, sourceProj, earthCenterRad) << std::endl;
    std::cout << "Center@result_: " << transform(earthProj, resultProj, earthCenterRad) << std::endl;

    point earthRadiusRad{
        source_.cols/2.0 / sourcePixelPerRad / cos(earthCenterRad.y), 
        source_.rows/2.0 / sourcePixelPerRad
    };
    point earthTopLeftRad{
        earthCenterRad.x - earthRadiusRad.x,
        earthCenterRad.y - earthRadiusRad.y
    };
    point earthBotRightRad{ 
        earthCenterRad.x + earthRadiusRad.x, 
        earthCenterRad.y + earthRadiusRad.y
    };

    point resultTopLeft = transform(earthProj, resultProj, earthTopLeftRad);
    point resultBotRight = transform(earthProj, resultProj, earthBotRightRad);
    int resultWidth = int(resultHeight / (resultBotRight.y - resultTopLeft.y) * (resultBotRight.x - resultTopLeft.x));

    std::cout << "TargetTopLeft: " << resultTopLeft << std::endl;
    std::cout << "TargetBotRight: " << resultBotRight << std::endl;
    
    std::cout << "Earth radius: " << earthRadiusRad << std::endl;
    std::cout << "EarthTL: " << earthTopLeftRad << std::endl;
    std::cout << "EarthBR: " << earthBotRightRad << std::endl;

    std::cout << "SourceTopLeft: " << transform(resultProj, sourceProj, resultTopLeft) << std::endl;
    std::cout << "SourceBotRight: " << transform(resultProj, sourceProj, resultBotRight) << std::endl;

    std::cout << "result_ size: " << resultWidth << " " << resultHeight << std::endl;

    result_ = Image(resultHeight, resultWidth, transparent);
    for (int resultYpx=0; resultYpx<result_.rows; resultYpx++)
        for (int resultXpx=0; resultXpx<result_.cols; resultXpx++) {
            point resultXY {
                resultTopLeft.x + ((resultBotRight.x-resultTopLeft.x) * resultXpx) / resultWidth,
                resultTopLeft.y + ((resultBotRight.y-resultTopLeft.y) * (resultHeight-resultYpx)) / resultHeight
            };
            point sourceXY = transform(resultProj, sourceProj, resultXY);
            int sourceXpx = int(sourceXY.x * sourcePixelPerRad + sourceCenter_.x + 0.5);
            int sourceYpx = int(-sourceXY.y * sourcePixelPerRad + sourceCenter_.y + 0.5);
            if (goodPoint(source_, sourceXpx, sourceYpx)) {
                result_(resultYpx, resultXpx) = source_(sourceYpx, sourceXpx);
            }
        }
/*
    point resultCenter = transform(sourceProj, resultProj, point{0,0});
    int resultXpx = int((resultCenter.x - resultTopLeft.x)/(resultBotRight.x-resultTopLeft.x)*resultWidth + 0.5);
    int resultYpx = int((resultBotRight.y-resultCenter.y)/(resultBotRight.y-resultTopLeft.y)*resultHeight + 0.5);
    std::cout << "resultCenter: " << resultXpx << " " << resultYpx << std::endl;
    result_(resultYpx, resultXpx) = Vec4b(255,0,255,255);
*/        
    std::cout << "Corner-coordinates of result: " << resultTopLeft << " " << resultBotRight << std::endl;
}

const Image& ProjectionConvertor::getResult() const {
    return result_;
}

point ProjectionConvertor::transform(const projPJ& from, const projPJ& to, point xy) {
    pj_transform(from, to, 1, 1, &xy.x, &xy.y, NULL);
    return xy;
}
