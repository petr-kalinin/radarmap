#include <iostream>
#include <sstream>
#include <cmath>
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <proj_api.h>

using namespace std;
using namespace cv;

typedef Mat_<Vec4b> Image;

struct VecLess {
    bool operator()(const Vec4b& a, const Vec4b& b) const {
        for (int i=0; i<4; i++)
            if (a[i]!=b[i])
                return a[i]<b[i];
        return false;
    }
};

int main(int argc, char* argv[]) {
    Image source = imread("palette.png", -1);
    
    map<Vec4b, int, VecLess> colors;
    
    for (int y=0; y<source.rows; y++)
        for (int x=0; x<source.cols; x++) {
            Vec4b color = source(y,x);
            if (colors.find(color) == colors.end())
                colors[color] = 0;
            colors[color]++;
        }
        
    for(auto x: colors)
        cout << x.first << " " << x.second << endl;
    
    return 0;
}