// Set pixel flag
enum TestFlag {
    UNTEST = 0,
    TESTING = 1,
    TESTED = 2,
    EDGE = 3,
    SEED = 4
};

// Set fairness issue 
enum FairWeight {
    HIGHWEIGHT = 6,
    MIDWEIGHT = 3,
    LOWWEIGHT = 2,
    NOWEIGHT = 1
};
const int THRESHOLD = 128;


//#define DEBUG         //turn on/off debug
//#define FORCESUPERPIXESIZE
const int ForceSuperPixSize = 30;

// Set formula parameter
const float DISPLACE_W_SD = 1.0;
const float DISPLACE_W_INTENSITY_DIFF = 0.3;
const float DISPLACE_W_SD_DIFF = 0.3;

// Set seed displacement parameter
// 1.0 will be the just right in its own area. 0.8 is little smaller
const float SuperpixelSeedDisplacement = 0.8;   

const int GRADIENT_FACTOR = 100;

// Edge color
const unsigned char EDGE_R = 255;
const unsigned char EDGE_G = 0;
const unsigned char EDGE_B = 0;
// Seed color
const unsigned char SEED_R = 0;
const unsigned char SEED_G = 0;
const unsigned char SEED_B = 255;
