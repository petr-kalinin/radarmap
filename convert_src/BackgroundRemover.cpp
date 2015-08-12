#include "BackgroundRemover.h"

#include <iostream>
#include <cmath>

#include "palette.h"

BackgroundRemover::BackgroundRemover(const Image& source) :
    source_(source),
    result_(source.rows, source.cols, transparent),
    imageRadius_(source.cols*250/300/2),
    imageCenter_(source.cols/2, source.rows/2)
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
    float dist = sqrt(pow(x-imageCenter_.x,2) + pow(y-imageCenter_.y,2));
    
    if (abs(dist - imageRadius_) < 1)
        return boundaryColor;
    if (dist > imageRadius_)
        return transparent;

    if (eqColor(source_(y,x),backgroundInner))
        return transparent;
    
    return source_(y,x);
}

const Image& BackgroundRemover::getResult() const {
    return result_;
}

const point& BackgroundRemover::getResultCenter() const {
    return imageCenter_;
}