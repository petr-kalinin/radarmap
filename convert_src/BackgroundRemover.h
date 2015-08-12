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
};

#endif
