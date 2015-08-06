#include "BackgroundRemover.h"

#include <iostream>

#include "palette.h"

BackgroundRemover::BackgroundRemover(const Image& source, const Image& stencil) :
    source_(source),
    stencil_(stencil),
    result_(source.rows, source.cols, transparent)
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
    if (boundaryPoint(x, y)) 
        return boundaryColor;

    if (!inColors(source_(y,x),palette) || isBadPaletteColor(x, y)) 
        return neibColor(x, y);
    
    return source_(y,x);
}


bool BackgroundRemover::boundaryPoint(int x, int y) {
    int cntOuter = 0, cntInner=0;
    for (int dy=-2; dy<=2; dy++) 
        for (int dx=-2; dx<=2; dx++) {
            int cx = x + dx;
            int cy = y + dy;
            if (goodPoint(source_, cx, cy)) {
                if (eqColor(source_(cy,cx), backgroundOuter, 10)||(source_(cy,cx)==transparent)) cntOuter++;
                if (eqColor(source_(cy,cx), backgroundInner, 10)) cntInner++;
            } else cntOuter++;
        }
    return ((cntOuter > 5) && (cntInner > 5));
}

Vec4b BackgroundRemover::neibColor(int x, int y) {
    Vec4b res = transparent;
    int mindist = 10;
    for (int dx=-3; dx<3; dx++) 
        for (int dy=-3; dy<3; dy++) {
            int cx = x + dx;
            int cy = y + dy;
            if (goodPoint(source_, cx, cy)) {
                if (inColors(source_(cy,cx),palette) && (!isBadPaletteColor(cx, cy)) && (abs(dx)+abs(dy)<mindist)) {
                    mindist = abs(dx)+abs(dy);
                    res = source_(cy,cx);
                }
            }
        }
    return res;
}

bool BackgroundRemover::isBadPaletteColor(int x, int y) {
    if ((!eqColor(source_(y,x),black)) && (!inColors(source_(y,x),badPaletteColors)))
        return false;
    
    bool wasInStencil = false;
    for (int dx=-2; dx<2; dx++) 
        for (int dy=-2; dy<2; dy++) {
            int cx = x + dx;
            int cy = y + dy;
            if (goodPoint(stencil_, cx, cy)) {
                wasInStencil = wasInStencil || eqColor(stencil_(cy, cx), source_(y,x), 10);
                if (inColors(source_(y,x),badPaletteColors)) // badPaletteColors are modified road colors, so check for unmodified road too
                    wasInStencil = wasInStencil || inColors(stencil_(cy, cx), roadColors, 10);
            }
        }
        
    return wasInStencil;
}

const Image& BackgroundRemover::getResult() const {
    return result_;
}
