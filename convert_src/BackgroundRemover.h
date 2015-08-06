#ifndef BackgroundRemover_h
#define BackgroundRemover_h

#include "misc.h"

class BackgroundRemover {
    public:
        BackgroundRemover(const Image& source, const Image& stencil);
        const Image& getResult() const;
        
    private:
        const Image& source_;
        const Image& stencil_;
        Image result_;
        
        void removeBackground();
        Vec4b replacementColor(int x, int y);
        bool boundaryPoint(int x, int y);
        Vec4b neibColor(int x, int y);
        bool isBadPaletteColor(int x, int y);
};

#endif
