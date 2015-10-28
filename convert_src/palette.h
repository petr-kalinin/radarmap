#ifndef palette_h
#define palette_h

#include "misc.h"

const Vec4b backgroundInner(204,204,204,255);

const Vec4b boundaryColor(128,0,0,255);

const Vec4b transparent(0,0,0,0);
const Vec4b black(0,0,0,255);

const std::vector<Vec4b> palette{ // generated using make_palette
    {10, 235, 255, 255},
    {17, 152, 255, 255},
    {120, 194, 71, 255},
    {193, 83, 97, 255},
    {255, 89, 232, 255},
    {255, 225, 155, 255},
//    {255, 255, 255, 255},
    {6, 76, 168, 255},
    {17, 5, 194, 255},
    {53, 63, 255, 255},
    {66, 255, 88, 255},
    {143, 235, 155, 255},
    {163, 147, 255, 255},
    {249, 147, 71, 255},
    {255, 28, 190, 255},
    {255, 89, 12, 255},
    {255, 168, 221, 255}
};

inline bool inColors(const Vec4b& color, const std::vector<Vec4b>& pal, int eps = colorEps) {
    for(auto palc: pal)
        if (eqColor(color, palc, eps)) return true;
    return false;
}

#endif