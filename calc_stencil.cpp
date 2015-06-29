#include <iostream>
#include <sstream>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <proj_api.h>

using namespace std;
using namespace cv;

typedef Mat_<Vec4b> Image;
Vec4b transparent(0,0,0,0);
Vec4b boundaryColor(128,0,0,255);

int main(int argc, char* argv[]) {
    std::string targetName(argv[1]);
    int sourceN = argc - 2;
    std::vector<std::string> sourceName;
    for (int i=0; i<sourceN; i++)
        sourceName.emplace_back(argv[i+2]);
        
    std::vector<Image> source;
    for (int i=0; i<sourceN; i++) {
        source.emplace_back(imread(sourceName[i], -1));
        cout << "Read " << sourceName[i] << " " << source[i].cols << " " << source[i].rows << endl;
    }
    int width = source[0].cols;
    int height = source[0].rows;
    Image target(height, width, transparent);

    for (int x=0; x<width; x++)
        for (int y=0; y<height; y++) {
            bool ok = true;
            for (int j=1; j<sourceN; j++)
                ok = ok && (norm(source[j](y,x),source[0](y,x))<150);
            ok = ok && (source[0](y,x) != boundaryColor);
            if (ok)
                target(y,x) = source[0](y,x);
        }

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    imwrite(targetName, target, compression_params);
 
    return 0;
}