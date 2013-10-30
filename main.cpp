/*  This project is started for 2013 Spring CpSc 863 "Multimedia", Clemson University
*   To implement "Image Colorization Using Similar Images". For more detial: http://phoenix.cs.clemson.edu/color/
*   This is the first stage in that paper. I have another website to show this project. For more information: http://phoenix.cs.clemson.edu/extract/
*   Instead of following the paper, Yuefeng Zhou and I decided to develop a new algorithm to do Super-Pixel extraction.
*   This code is third version after the original code, written by YuanWang. 
*   
*   Email:  yuan8421@gmail.com   Yuan Wang      Clemson University
*   Email:  rfwindcn@gmail.com   Yuefeng Zhou   Clemson University         
*   Oct. 30, 2013
*/

#include <iostream>
#include <string>
#include "image.h"
#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP

unsigned char* readInColorImg(const char *colorInputPath, int *xres, int *yres) {
    ImageInput *in = ImageInput::create (colorInputPath);
    if (! in){
        throw "Can't read image file";
    }
    ImageSpec spec;
    in->open (colorInputPath, spec);
    //Get information of picture
    *xres = spec.width;
    *yres = spec.height;
    int CHANNELS = spec.nchannels;
    std::cout<<"X: "<<*xres<<'\t'<<"Y:"<<*yres<<'\t'<<"Channel: "<<CHANNELS<<std::endl;
    //create pixels array
    unsigned char *ptr = new unsigned char [(*xres)*(*yres)*CHANNELS];
    //read in the pixels info
    in->read_image (TypeDesc::UINT8, ptr);
    in->close ();
    delete in;
    #ifdef DEBUG
    std::cout<<"1. Load image complete. <main.cpp>"<<std::endl;
    #endif
    return ptr;
}
int main(int argc, char **argv){
    if(argc != 2){
        std::cout<<"Usage: ./color <your image path>"<<std::endl;
        std::cout<<"For more information: ./color --help"<<std::endl;
        return 1;
    }
    std::string option = "--help";
    if(!strcmp(argv[1], option.c_str() ) ){
        std::cout<<"Usage: ./color <your image path>"<<std::endl;
        std::cout<<"This program could process .jpg .bmp image."<<std::endl;
        std::cout<<"Can't process .gif .png .svg image."<<std::endl;
        return 1;
    }
    #ifdef _OPENMP
    omp_set_num_threads(8);
    #endif // _OPENMP
    std::cout << "Loading target image." << std::endl;
    int xres, yres;
    xres = yres = 0;
    unsigned char *colorPixels = readInColorImg(argv[1],&xres,&yres);
    Image myImage(colorPixels,xres,yres);
    delete[] colorPixels;
    myImage.calSdValue();
    myImage.calEightNeighborDiff();
    myImage.calGValue();    
    myImage.initialGrid();
    myImage.replaceSeed();
    myImage.colorSeed();
    myImage.extractImg();
    myImage.drawEdge();
    #ifdef DEBUG
    myImage.deBugGValue();
    myImage.deBugFlag();
    myImage.deBug();
    #endif  
    myImage.outputImage();
    return 0;
}
