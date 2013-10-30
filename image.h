#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <OpenImageIO/imageio.h>
#include "pixel.h"
OIIO_NAMESPACE_USING
class Great {   // To help build min heap(priority queue)
public:
    bool operator()(const Pixel *lh, const Pixel *rh){
        return lh->GValue() > rh->GValue();
    }
};
class Image {
public:
    Image(unsigned char *input, int width, int height);
    void calSdValue();  // calculate standard deviation with its 25 neighbors
    void calEightNeighborDiff();    // calcualte diff of intensity with its 8 neighbots
    void calGValue();   // The GValue is composed by SD and intensity with diff weights
    void initialGrid(); // Initial seed position as a grid
    void replaceSeed(); // replace seed position
    void colorSeed();   // colorize seed to different color
    void extractImg();  // Extraction
    void drawEdge();    // show you edge
    void outputImage(); // output image
    
    void deBug();   // Debug basic pixel information such as: intensity, sd, position,
    void deBugGValue(); // Show Gvalue
    void deBugFlag();   // Show pixel flag
    
private:
    std::vector< std::vector<Pixel> > imagePixel;   // two dimention vector of pixle
    std::vector< std::vector<std::pair<int, int> > > seedPos;   // two dimention vector of seed position
    std::vector< std::vector<std::list<Pixel *> > > activeList; // every super-pixel has a activeList to indicate the edge 
    std::vector< std::vector<std::list<Pixel *>::iterator> > deleteEnd; // this two dimention hold a iterator to indicate the pos in active list, before that is old edge, behind it is the new edge
    std::priority_queue<Pixel *, std::vector<Pixel *>, Great> expandQueue;  // a priority queue to hold all pixel wait to be tested
    std::vector< std::vector<int> > sumSD;  //  for every super-pixel it contains a sumSD to indicate current pixel SD it has 
    std::vector< std::vector<int> > sumGray;    // same th the sunSD but instead of Gray value
    std::vector< std::vector<int> > childPixelNum;  // indicate how many pixels does super-pixel has
    int xres, yres, size;   // image width, height, number of pixels
    int XRES, YRES, SIZE;   // super-pixel width, height, number of super-pixels
    int SuperPixelSize;     // indicate the original distance between two seed
    int StillNeedProcess;   // number of pixel which still needs to be tested
    
    void deBugQueue();  
    void initialActiveListAndDeleteEnd();
    void deletePixelInActiveList(int ROW, int COL); // if a pixel's 8 neighbor pixel has been determined ower super-pixel, then delete it from activeList
    void updateExpandQueue(int ROW, int COL);   // if a pixel still has non-tested pixel, then push the pixel into ExpandQueue
    void updateActiveList(int FACTOR);  // if a pixel is determined belong to a super-pixel, then this pixel will be the edge of pixel, then it should be push into activeList
    
    int posCheckRow(int row);   // Boundray test
    int posCheckCol(int col);
    
    bool noMoreNeighborToBeTest(int row, int col);  // Test if its 8 neighbors are all tested
};
