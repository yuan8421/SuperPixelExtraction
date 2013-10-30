#include "pixel.h"
Pixel::Pixel(unsigned char R, unsigned char G, unsigned char B, int row, int col):
    r(R), g(G), b(B), gray(0), 
    ROW(row), COL(col),
    IDROW(-1), IDCOL(-1),   // indicate this pixel belong to which super-pixel
    sdValue(0), gValue(0),
    flag(UNTEST),
    eightNeighborDiff(),aboveThreshold() {
        eightNeighborDiff.reserve(8);   // for calculate GValue
        aboveThreshold.reserve(8);      // for calculate GValue
}
     
void Pixel::fairIssue(){
    int size = aboveThreshold.size();
    int weight = fairWeight(size );
    if(size){
        std::vector<int>::iterator iter = eightNeighborDiff.begin();
        while( iter != eightNeighborDiff.end() ) {
            *iter *= *iter;
            ++iter;
        }
        std::vector<int>::iterator iterMulti = aboveThreshold.begin();
        while( iterMulti != aboveThreshold.end() ) {
            eightNeighborDiff[*iterMulti] *= weight;
            ++iterMulti;
        }
    }else{
        std::vector<int>::iterator iter = eightNeighborDiff.begin(); 
        while( iter != eightNeighborDiff.end() ) {
            *iter *= *iter;
            ++iter;
        }
    }
}
    
int Pixel::fairWeight(int aboveThresNum) {
    switch (aboveThresNum){
        case 1:
            return HIGHWEIGHT;
        case 2:
            return MIDWEIGHT;
        case 3:
            return LOWWEIGHT;
        default:
            return NOWEIGHT;
    }
}

void Pixel::composeGValue() {
    GValue(GValue() + 8 * SdValue()*SdValue() );
}
