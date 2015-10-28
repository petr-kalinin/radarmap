#ifndef BackgroundRemover_h
#define BackgroundRemover_h

#include "misc.h"

class BackgroundRemover {
    public:
        BackgroundRemover(const Image& source);
        const Image& getResult() const;
        
        const point& getResultCenter() const;
        
    private:
        
        int imageRadius_;
        point imageCenter_;
        
        const Image& source_;
        Image result_;
        
        void removeBackground();
        Vec4b replacementColor(int x, int y);
        Vec4b getSimilarPaletteColor(Vec4b col);
        float getCorrectedColorDistance(Vec4b a, Vec4b b);
};

#endif
