#include "BackgroundRemover.h"

#include <iostream>
#include <cmath>

#include "palette.h"

BackgroundRemover::BackgroundRemover(const Image& source) :
    source_(source(cv::Rect(0,0,500,479))),
    result_(source.rows, source.cols, transparent),
    imageRadius_(233),
    imageCenter_(259, 239)
{
    removeBackground();
}

void BackgroundRemover::removeBackground() {
    std::cout << "Removing background" << std::endl;
    for (int y=0; y<source_.rows; y++)
        for (int x=0; x<source_.cols; x++) {
            result_(y, x) = replacementColor(x, y);
        }
}

Vec4b BackgroundRemover::replacementColor(int x, int y) {
    float dist = sqrt(1.0*pow(x-imageCenter_.x,2) + pow(y-imageCenter_.y,2));
    
    if ((abs(x -imageCenter_.x)<0.5) && (abs(y-imageCenter_.y)<0.5)) return boundaryColor;
    
    if (abs(dist - imageRadius_) < 0.1)
        return boundaryColor;
/*    if (dist > imageRadius_)
        return transparent;*/
    
    Vec4b result = getSimilarPaletteColor(source_(y, x));
    
    if ((result == transparent)&&(source_(y,x)!=backgroundInner)) {
        int mindist = 10;
        for (int dx = -2; dx<2; dx++)
            for(int dy = -2; dy<2; dy++) 
                if (goodPoint(source_, x+dx, y+dy)) {
                    Vec4b thisColor = getSimilarPaletteColor(source_(y+dy, x+dx));
                    int dist = abs(dx) + abs(dy);
                    if ((thisColor != transparent) && (dist < mindist)) {
                        mindist = dist;
                        result = thisColor;
                    }
                }
    }
    
    return result;
}

Vec4b BackgroundRemover::getSimilarPaletteColor(Vec4b col) {
    float minDiff = 0.03; // we will assume that any color further than 10 is bad
    Vec4b result = transparent;
    for (auto x: palette) {
        float diff = getCorrectedColorDistance(col, x);
        if (diff < minDiff) {
            result = x;
            minDiff = diff;
        }
    }
    return result;
}

float BackgroundRemover::getCorrectedColorDistance(Vec4b a, Vec4b b) {
    if (eqColor(a, black, 10)) // for almost black pixels the precision of calculation is bad
        return 1e20;
    
    // try to match "b" as alpha * a with some unknown alpha
    //  x0 y0 z0
    //  a0 a1 a2
    //  b0 b1 b2
    int c0 = (int)a[1]*b[2]-a[2]*b[1],
        c1 = (int)a[2]*b[0]-a[0]*b[2],
        c2 = (int)a[0]*b[1]-a[1]*b[0];
    float crossVal = sqrt(c0*c0 + c1*c1 + c2*c2);
                 
    float scal = (int)a[0]*b[0] + (int)a[1]*b[1] + (int)a[2]*b[2];
    
    float angle = atan2(crossVal, scal);
    
    return angle;
}

const Image& BackgroundRemover::getResult() const {
    return result_;
}

const point& BackgroundRemover::getResultCenter() const {
    return imageCenter_;
}