#include <iostream>
#include <string>

#include "misc.h"
#include "ProjectionConvertor.h"
#include "BackgroundRemover.h"

using namespace cv;

int main(int argc, char* argv[]) {
    std::cout.precision(10);

    point earthCenterDeg {std::stof(argv[1]), std::stof(argv[2])};
    std::string sourceName(argv[3]);
    std::string targetName(argv[4]);

    Image source = imread(sourceName, -1);
    
    BackgroundRemover br(source);
    
    ProjectionConvertor pc(br.getResult(), earthCenterDeg, br.getResultCenter());
    
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    imwrite(targetName, pc.getResult(), compression_params);
    
    std::cout << "Wrote to " << targetName << std::endl;
    
    return 0;
}