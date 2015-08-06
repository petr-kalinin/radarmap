#ifndef palette_h
#define palette_h

#include "misc.h"

const Vec4b backgroundOuter(164,160,160,255);
const Vec4b lineColor(115,115,115,255);
const Vec4b backgroundInner(208,208,208,255);
const Vec4b boundaryColor(128,0,0,255);
const Vec4b transparent(0,0,0,0);
const Vec4b black(0,0,0,255);
const std::vector<Vec4b> palette{ // generated using make_palette
                        {0, 0, 95, 255},
                        {0, 0, 255, 255},
                        {0, 68, 136, 255},
                        {0, 102, 204, 255},
                        {0, 152, 0, 255},
                        {90, 194, 0, 255},
                        {95, 63, 63, 255},
                        {116, 0, 0, 255},
                        {127, 85, 255, 255},
                        {127, 170, 255, 255},
                        {128, 255, 255, 255},
                        {147, 255, 70, 255},
                        {177, 170, 156, 255},
                        {199, 0, 199, 255},
                        {255, 56, 1, 255},
                        {255, 85, 255, 255},
                        {255, 136, 62, 255},
                        {255, 170, 255, 255},
                        {255, 198, 162, 255}
};
const std::vector<Vec4b> badPaletteColors{ // these are palette colors that sometimes coincide with a color of a road
    {0, 68, 136, 255},
    {0, 102, 204, 255}
};
const std::vector<Vec4b> roadColors{
    {0, 85, 170, 255},
    {0, 85, 255, 255}
};

inline bool inColors(const Vec4b& color, const std::vector<Vec4b>& pal, int eps = colorEps) {
    for(auto palc: pal)
        if (eqColor(color, palc, eps)) return true;
    return false;
}

#endif