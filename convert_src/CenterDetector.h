#ifndef CenterDetector_h
#define CenterDetector_h

#include <vector>

#include "misc.h"

class CenterDetector {
    public:
        CenterDetector(const Image& source);
        const Image& getResult() const;
        const point& getResultCenter() const;
        
    private:
        static const int defaultLineDelta;
        static const point defaultSize, defaultCenter;

        std::vector<int> makeCumulative(const std::vector<int>& v);
        
        void detectCenter();
        void transformImage();
        
        point detectedCenter_;
        float detectedScaling_;
        
        const Image& source_;
        Image result_;
};


#endif
