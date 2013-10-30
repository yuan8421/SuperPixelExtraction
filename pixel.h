#include <iostream>
#include <cmath>
#include <vector>
#include "configure.h"

class Pixel {
public:
	Pixel(unsigned char r, unsigned char g, unsigned char b, int row, int col);
	void colorToGray() { gray = static_cast<int>(pow((pow(r,2.2) * 0.2973 + pow(g,2.2) * 0.6274 + pow(b,2.2) * 0.0753),(1.0/2.2))); }   //adobe Photoshop gray scale formula
	
	int Col() const { return COL; } // pixel column
	int Row() const { return ROW; } // pixel row
	
	int idCol() const { return IDCOL; }    // get the super-pixel cole current pixel belong to
	void idCol(int i) { IDCOL = i; }        // set 
	int idRow() const { return IDROW; }    // get the super-pixel row  current pixel belong to
	void idRow(int i) { IDROW = i; }
	
	unsigned char Gray() const { return gray; } // get gray level value
	
	void SdValue(int s) { sdValue = s; }
	int SdValue() const { return sdValue; }
	void GValue(int g) { gValue = g; }
	int GValue() const { return gValue; }
	void Flag(int f) { flag = f; }
	int Flag() const { return flag; }
	
	unsigned char R() const { return r; }
	void R(unsigned char color) { r = color; }
	unsigned char G() const { return g; }
	void G(unsigned char color) { g = color; }
	unsigned char B() const { return b; }
	void B(unsigned char color) { b = color; }
	
	Pixel* getSelf() { return this; }   // return itself pointer

	void diffPushBack(int diff) { eightNeighborDiff.push_back(diff); }  // push back diff gray value with 8 neighbot
	void thresholdPushBack() { aboveThreshold.push_back(flag); }    // record the position in vector:eightNeighborDiff which is above THRESHOLD. This is for Fairness Issue
	
	void fairIssue();
	void composeGValue();
			
private:
	unsigned char r,g,b;
	int gray;
	int ROW,COL;
	int IDROW,IDCOL;
	
	int sdValue,gValue;
	int flag;
	std::vector<int> eightNeighborDiff;
	std::vector<int> aboveThreshold;
	
	int fairWeight(int aboveThresNum);
};
