#ifndef cvHeaders_h
#define cvHeaders_h

#include <ostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//typedef cv::Mat_<Vec4b> Image;
using Vec4b = cv::Vec4b;
using Image = cv::Mat_<Vec4b>;

struct point {
    double x,y;
    point(double _x, double _y): x(_x), y(_y) {}
};

inline std::ostream& operator<<(std::ostream& s, point p) {
    s << "(" << p.x << " " << p.y << ")";
    return s;
}

inline bool goodPoint(const Image& im, int x, int y) {
    return (x>=0) && (x<im.cols) && (y>=0) && (y<im.rows);
}


const int colorEps = 4;
inline bool eqColor(const Vec4b& a, const Vec4b& b, const int eps=colorEps) {
    int diff = abs(a[0]-b[0]) + abs(a[1]-b[1]) + abs(a[2]-b[2]) + abs(a[3]-b[3]);
    return diff < eps;
}


#endif