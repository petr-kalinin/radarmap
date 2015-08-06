#ifndef ProjectionConverter_h
#define ProjectionConverter_h

#include <proj_api.h>

#include "misc.h"

class ProjectionConvertor {
    public:
        ProjectionConvertor(const Image& source, const point& earthCenterDeg, const point& sourceCenter);
        const Image& getResult() const;
    
    private:
        static const double sourcePixelPerRad;
        static const int resultHeight;
        
        const Image& source_;
        Image result_;
        
        const point& earthCenterDeg_;
        const point& sourceCenter_;

        void transformProjection();
        
        point transform(const projPJ& from, const projPJ& to, point xy);
        
};

#endif