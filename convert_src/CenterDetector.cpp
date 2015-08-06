#include "CenterDetector.h"

#include <iostream>
#include <utility>

#include "palette.h"

const int CenterDetector::defaultLineDelta = 120;
const point CenterDetector::defaultSize{1100, 1100};
const point CenterDetector::defaultCenter{1100/2, 1100/2};

CenterDetector::CenterDetector(const Image& source): 
    source_(source),
    result_(defaultSize.y, defaultSize.x, transparent),
    detectedCenter_(defaultCenter),
    detectedScaling_(1)
{
    detectCenter();
    transformImage();
}

std::vector<int> CenterDetector::makeCumulative(const std::vector<int>& v) {
    static const int avgStep = 50;

    std::vector<int> res(v.size(), 0);
    for (int i=0; i<v.size(); i++)
        for (int j=-avgStep; j<=avgStep; j++) { 
            int pos = i+j;
            if (pos<0) pos=0;
            if (pos>=v.size()) pos=v.size()-1;
            res[i]+=v[pos];
        }
    return res;
}

void CenterDetector::detectCenter() {
    std::vector<int> cntLineX(source_.cols, 0);
    std::vector<int> cntLineY(source_.rows, 0);
    std::vector<int> cntBgX(source_.cols, 0);
    std::vector<int> cntBgY(source_.rows, 0);
    for (int y=0; y<source_.rows; y++)
        for (int x=0; x<source_.cols; x++) {
                if (eqColor(source_(y, x), backgroundOuter, 10)) { 
                    cntBgX[x]++;
                    cntBgY[y]++;
                }
                if (eqColor(source_(y, x), lineColor, 10)) {
                    cntLineX[x]++;
                    cntLineY[y]++;
                }
        }
    std::vector<int> cumBgX = makeCumulative(cntBgX);
    std::vector<int> cumBgY = makeCumulative(cntBgY);
    
    int maxLineX = *max_element(cntLineX.begin(), cntLineX.end());
    int maxLineY = *max_element(cntLineY.begin(), cntLineY.end());
    std::cout << "maxLineX=" << maxLineX << " of " << source_.rows << std::endl;
    std::cout << "maxLineY=" << maxLineY << " of " << source_.cols << std::endl;
    std::map<int, int> deltas;
    
    point result{0,0};
    int prev = -1;
    for(int x=0; x<cntLineX.size(); x++)
        if (cntLineX[x] > maxLineX *7/10) {
            std::cout << "X-candidate: " << x << " " << double(cntLineX[x]) / maxLineX << " " << cumBgX[x] << std::endl;
            if (cumBgX[x] < cumBgX[result.x])
                result.x = x;
            if (prev != -1)
                deltas[x - prev] ++;
            prev = x;
        }
    prev = -1;
    for(int y=0; y<cntLineY.size(); y++)
        if (cntLineY[y] > maxLineY *7/10) {
            std::cout << "Y-candidate: " << y << " " << double(cntLineY[y]) / maxLineY << " " << cumBgY[y] << std::endl;
            if (cumBgY[y] < cumBgY[result.y])
                result.y = y;
            if (prev != -1)
                deltas[y - prev] ++;
            prev = y;
        }
    std::pair<int, int> maxDelta = *deltas.begin();
    for (auto v: deltas)
        if (v.second > maxDelta.second)
            maxDelta = v;
    std::cout << "Deltas: ";
    for (auto v: deltas)
        std::cout << v.first << " " << v.second << std::endl;
    detectedCenter_ = result;
    detectedScaling_ = (float)maxDelta.first / defaultLineDelta;
    std::cout << "Detected center @ " << detectedCenter_ << std::endl;
    std::cout << "Detected scaling " << detectedScaling_ << std::endl;
}

void CenterDetector::transformImage() {
    for (int x=0; x<result_.cols; x++)
        for (int y=0; y<result_.rows; y++) {
            int sourceX = (x-defaultCenter.x) * detectedScaling_ + detectedCenter_.x;
            int sourceY = (y-defaultCenter.y) * detectedScaling_ + detectedCenter_.y;
            if (goodPoint(source_, sourceX, sourceY)) {
                result_(y, x) = source_(sourceY, sourceX);
            }
        }
}

const Image& CenterDetector::getResult() const {
    return result_;
}

const point& CenterDetector::getResultCenter() const {
    return defaultCenter;
}