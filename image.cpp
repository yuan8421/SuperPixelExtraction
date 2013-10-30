#include "image.h"
Image::Image(unsigned char *input, int width, int height):
    imagePixel(),seedPos(),activeList(),deleteEnd(),expandQueue(),sumSD(),sumGray(),
    xres(width), yres(height), size(0),
    XRES(0), YRES(0), SIZE(0),
    SuperPixelSize(0),StillNeedProcess(-1) {
        size = xres * yres;
        imagePixel.resize(yres);
        for( int row = 0; row < yres; ++row) {
            imagePixel[row].reserve(xres);
            for( int col = 0; col < xres; ++col) {
                Pixel singlePixel( *(input + (row * xres + col)*3), *(input + (row * xres + col)*3+1),*(input + (row * xres + col)*3+2), row, col);
                singlePixel.colorToGray();
                imagePixel[row].push_back(singlePixel);
            }
        }
       #ifdef DEBUG
       std::cout<<"2. Image pixel initial complete. <image.cpp>"<<std::endl;
       #endif
}

void Image::calSdValue(){
    for(int row = 0; row < yres; ++row) {
        for(int col =0; col < xres; ++col) {
            int mean,sd;
            mean = sd = 0;
            for(int x = -2; x <= 2; ++x){
                for(int y = -2; y <=2; ++y){
                    mean += imagePixel[posCheckRow(row+y)][posCheckCol(col+x)].Gray() ;   //add 25 pixels gray to calculate avg
                }
            }
            mean /= 25;
            for(int x = -2; x <= 2; ++x){
                for(int y = -2; y <=2; ++y){
                    int diff = imagePixel[posCheckRow(row+y)][posCheckCol(col+x)].Gray() - mean;
                    sd += diff * diff;   //add 25 pixels gray to calculate avg
                }
            }
            imagePixel[row][col].SdValue(sqrt(sd/25) );
        }
    }
    #ifdef DEBUG
    std::cout<<"3. SD calculate complete. <image.cpp>"<<std::endl;
    #endif
}

void Image::calEightNeighborDiff(){
    for(int row =0; row < yres; ++row) {  // Y direction loop
        for(int col = 0; col < xres; ++col) { // X direction loop
            int index = 0;
            for(int x = -2; x <= 2; ++x){   // 8 neighbor pixel X direction loop
                for(int y = -2; y <=2; ++y){    // 8 neighbor pixel Y direction loop
                    int diff = imagePixel[posCheckRow(row+y)][posCheckCol(col+x)].Gray() - imagePixel[row][col].Gray();
                    imagePixel[row][col].diffPushBack(diff);
                    if(std::abs(diff) > THRESHOLD){
                        imagePixel[row][col].thresholdPushBack();  // record the index ONLY above threshold to avoid search in vector to check index of element above the threshold
                    }else{
                        ;// do nothing, just show the complete if-else
                    }
                    ++index;
                }   // end 8 neighbor pixel X direction loop
            }   // end 8 neighbor pixel Y direction loop
        }   // end X direction loop
    }   // end Y direction loop
    #ifdef DEBUG
    std::cout<<"4. 8 neighbor pixel diffrence calculate complete. <image.cpp>"<<std::endl;
    #endif
}

void Image::calGValue() {
    for(int row =0; row < yres; ++row) {  // Y direction loop
        for(int col = 0; col < xres; ++col) { // X direction loop
            imagePixel[row][col].fairIssue();
            imagePixel[row][col].composeGValue();
         }   // end X direction loop
    }   // end Y direction loop
    #ifdef DEBUG
    std::cout<<"5. GValue calculate complete. <image.cpp>"<<std::endl;  
    #endif  
}

void Image::initialGrid(){
    SuperPixelSize = sqrt(std::min(xres,yres));    //get WIDTH of super pixel
    #ifdef FORCESUPERPIXESIZE
        SuperPixelSize = ForceSuperPixSize;
    #endif
    float startPosition = SuperPixelSize / 2.0;
    XRES=static_cast<int>(static_cast<float>(xres)/SuperPixelSize + 0.5);
    YRES=static_cast<int>(static_cast<float>(yres)/SuperPixelSize + 0.5);
    SIZE=XRES*YRES;   // Superpixel SIZE
    std::cout<<"XRES = "<<XRES<<std::endl;
    std::cout<<"YRES = "<<YRES<<std::endl;
    seedPos.resize(YRES);
    for(int row=0; row<YRES; ++row) {
        seedPos[row].reserve(XRES);
        for(int col=0; col<XRES; ++col) {
            int rowPos = static_cast<int>( (2*(row+1)-1) * startPosition) - 1;
            int colPos = static_cast<int>( (2*(col+1)-1)*startPosition)-1 ;
            seedPos[row].push_back(std::make_pair(rowPos, colPos) );
        }
    }
    #ifdef DEBUG
    std::cout<<"6. Initial Grid complete. <image.cpp>"<<std::endl;
    #endif
}   
   
void Image::replaceSeed(){
    for(int row = 0; row < YRES; ++row) {
        for(int col = 0; col < XRES; ++col) {
            int nbrCount, nbrIntensity, nbrSd; 
            nbrCount = nbrIntensity = nbrSd = 0;
            if(row > 0){
                ++nbrCount;
                nbrIntensity += imagePixel[seedPos[row-1][col].first][seedPos[row-1][col].second].Gray();
                nbrSd += imagePixel[seedPos[row-1][col].first][seedPos[row-1][col].second].SdValue();
            }
            if(row<YRES-1) {
                ++nbrCount;
                nbrIntensity += imagePixel[seedPos[row+1][col].first][seedPos[row+1][col].second].Gray();
                nbrSd += imagePixel[seedPos[row+1][col].first][seedPos[row+1][col].second].SdValue();
            }
            if(col > 0) {
                ++nbrCount;
                nbrIntensity += imagePixel[seedPos[row][col-1].first][seedPos[row][col-1].second].Gray();
                nbrSd += imagePixel[seedPos[row][col-1].first][seedPos[row][col-1].second].SdValue();
            }
            if(col<XRES-1) {
                ++nbrCount;
                nbrIntensity += imagePixel[seedPos[row][col+1].first][seedPos[row][col+1].second].Gray();
                nbrSd += imagePixel[seedPos[row][col+1].first][seedPos[row][col+1].second].SdValue();
            }
            nbrIntensity /= nbrCount;
            nbrSd /= nbrCount;
            int minRow, minCol;
            minRow = minCol = 0;
            for(int step = SuperPixelSize*SuperpixelSeedDisplacement/4; step > 0; step /=2) {
                int oldRow = seedPos[row][col].first;
                int oldCol = seedPos[row][col].second;
                double minscore = 1e10;
                
                for(int p = -1; p <=1; ++p){
                    for(int q = -1; q <=1; ++q) {
                        int newRow = oldRow + p * step;
                        int newCol = oldCol + q * step;
                        if(newRow < 0) {
                            newRow = 0;
                        }else if(newRow > yres-1) {
                            newRow = yres-1;
                        }
                        if(newCol < 0){
                            newCol = 0;
                        }else if(newCol > xres-1) {
                            newCol = xres-1;
                        }
                        int diffIntensity = (nbrIntensity - imagePixel[newRow][newCol].Gray() );
                        if(diffIntensity < 0){
                            diffIntensity = -diffIntensity;
                        }
                        int diffSd = (nbrSd - imagePixel[newRow][newCol].SdValue());
                        if(diffSd < 0){
                            diffSd = -diffSd;
                        }
                        int score = imagePixel[newRow][newCol].SdValue()*DISPLACE_W_SD                        //key to determine seed displacement
                                     - diffIntensity*DISPLACE_W_INTENSITY_DIFF
                                     - diffSd*DISPLACE_W_SD_DIFF;
                        if(score<minscore) {
                            minscore = score;
                            minRow = newRow;
                            minCol = newCol;
                        }
                    }
                }
                seedPos[row][col].first = minRow;
                seedPos[row][col].second = minCol;
            }
            imagePixel[ seedPos[row][col].first][seedPos[row][col].second].Flag(SEED);  // set flag of SEED
            imagePixel[ seedPos[row][col].first][seedPos[row][col].second].idRow(row);  // set flag of idRow
            imagePixel[ seedPos[row][col].first][seedPos[row][col].second].idCol(col);  // set flag of idCol
        }
    }
    #ifdef DEBUG
    std::cout<<"7. Seed Replacement compelete. <image.cpp>"<<std::endl;
    #endif
}

void Image::colorSeed() {
    for(int row=0; row<YRES; ++row) {
        for(int col=0; col<XRES; ++col) {
            imagePixel[seedPos[row][col].first][seedPos[row][col].second].R(SEED_R);
            imagePixel[seedPos[row][col].first][seedPos[row][col].second].G(SEED_G);
            imagePixel[seedPos[row][col].first][seedPos[row][col].second].B(SEED_B);
        }
    }
}

void Image::extractImg(){
    initialActiveListAndDeleteEnd();
    StillNeedProcess = size - SIZE; // all pixel but SEED still need process
    int loopCount = 0;
    while(StillNeedProcess){    // loop 1
        ++loopCount;
        for( int i = 0; i < YRES; ++i) {    // Superpixel Row loop 2
            for( int j =0; j < XRES; ++j) { // Superpixel Col loop 3
                deletePixelInActiveList(i,j);   // delete those pixel no more used to test edge
                updateExpandQueue(i,j);     // add new pixel which will be tested
            }   //end loop 3
        }   // end loop 2
        updateActiveList(loopCount*GRADIENT_FACTOR);    // delete pixel from ExpandQueue and insert it into ActiveList as new edge
    }   // end while loop 1
}

void Image::drawEdge() {
    for(int row =1; row < yres-1; ++row) {
        for(int col = 1; col < xres-1; ++col) {
            int IDROW = imagePixel[row][col].idRow();
            int IDCOL = imagePixel[row][col].idCol();
        
            if((IDROW != imagePixel[(row-1)][col].idRow()) || (IDCOL != imagePixel[(row-1)][col].idCol()) ){
                imagePixel[row][col].R(EDGE_R);
                imagePixel[row][col].G(EDGE_G);
                imagePixel[row][col].B(EDGE_B);
                continue;
            }
            if((IDROW != imagePixel[(row+1)][col].idRow()) || (IDCOL != imagePixel[(row+1)][col].idCol()) ){
                imagePixel[row][col].R(EDGE_R);
                imagePixel[row][col].G(EDGE_G);
                imagePixel[row][col].B(EDGE_B);
                continue;
            }
            if((IDROW != imagePixel[row][(col-1)].idRow()) || (IDCOL != imagePixel[row][(col-1)].idCol()) ){
                imagePixel[row][col].R(EDGE_R);
                imagePixel[row][col].G(EDGE_G);
                imagePixel[row][col].B(EDGE_B);
                continue;
            }
            if((IDROW != imagePixel[row][(col+1)].idRow()) || (IDCOL != imagePixel[row][(col+1)].idCol()) ){
                imagePixel[row][col].R(EDGE_R);
                imagePixel[row][col].G(EDGE_G);
                imagePixel[row][col].B(EDGE_B);
                continue;
            }
        }
    }
}
   
void Image::outputImage() {    //out put the middle result, a gray scale image
    const char *filename = "output.jpg";
    unsigned char *outputBuffer = new unsigned char[xres*yres*3];
    int index = 0;
    for(int row = 0; row < yres; ++row) {
        for(int col = 0; col < xres; ++col) {
            *(outputBuffer+index)=imagePixel[row][col].R();
            *(outputBuffer+index+1)=imagePixel[row][col].G();
            *(outputBuffer+index+2)=imagePixel[row][col].B();
            index += 3;
        }
    }

    ImageOutput *colorOut = ImageOutput::create(filename);
    if(!colorOut) throw "Can't creat output image!";
    ImageSpec specColorOut (xres, yres, 3, TypeDesc::UINT8);
    colorOut->open (filename, specColorOut);
    colorOut->write_image (TypeDesc::UINT8, outputBuffer);
    colorOut->close ();
    delete colorOut;
    delete[] outputBuffer;
} 
   
void Image::deBug(){
    std::cout<<"X : "<< imagePixel[0].size() << std::endl;
    std::cout<<"Y : "<< imagePixel.size() << std::endl;
    for( int row = 0; row < yres; ++row){
        for( int col = 0; col < xres; ++col){
            std::cout<<"ROW "<<imagePixel[row][col].Row()<<'\t'<<
            "Col "<<imagePixel[row][col].Col()<<'\t'<<
            "Gray : "<<imagePixel[row][col].Gray() <<'\t'<<
            "SdValue : "<<imagePixel[row][col].SdValue() <<
            std::endl;
        }
    }
}

void Image::deBugGValue(){
    int max = 0;
    for( int row = 0; row < yres; ++row){
        for( int col = 0; col < xres; ++col){
            if(max<imagePixel[row][col].GValue()){
                max = imagePixel[row][col].GValue();
            }
        }
    }
    std::cout<<"max GValue = "<<max<<std::endl;
}

void Image::deBugFlag() {
    int untest, seed;
    untest=seed=0;
    for(int row = 0; row < yres; ++row){
        for( int col =0; col<xres;++col){
            if(UNTEST==imagePixel[row][col].Flag())
                ++untest;
            if(SEED==imagePixel[row][col].Flag())
                ++seed;
        }
    }
    std::cout<<"UNTEST size\t"<<untest<<std::endl;
    std::cout<<"SEED size\t"<<seed<<std::endl;
    std::cout<<"size\t"<<size<<std::endl;
    std::cout<<"XRES\t"<<XRES<<std::endl;
    std::cout<<"YRES\t"<<YRES<<std::endl;
    std::cout<<"SIZE\t"<<SIZE<<std::endl;
    if(size==untest+seed)
        std::cout<<"true"<<std::endl;
}

void Image::deBugQueue(){
    while(!expandQueue.empty()){
        std::cout<<expandQueue.top()->GValue()<<'\t';
        expandQueue.pop();
    }
}

void Image::initialActiveListAndDeleteEnd(){
    activeList.resize(YRES);
    deleteEnd.resize(YRES);
    sumSD.resize(YRES);
    sumGray.resize(YRES);
    childPixelNum.resize(YRES);
    for( int row = 0; row < YRES; ++row ) {
        activeList[row].resize(XRES);
        deleteEnd[row].reserve(XRES);
        sumSD.reserve(XRES);
        sumGray.reserve(XRES);
        childPixelNum.reserve(XRES);
        for( int col =0; col < XRES; ++col ) {
            activeList[row][col].push_back(imagePixel[seedPos[row][col].first][seedPos[row][col].second].getSelf() );
            deleteEnd[row].push_back(activeList[row][col].begin() );
            sumSD[row].push_back(imagePixel[seedPos[row][col].first][seedPos[row][col].second].SdValue() );
            sumGray[row].push_back(imagePixel[seedPos[row][col].first][seedPos[row][col].second].Gray() );
            childPixelNum[row].push_back(1);
        }
    }
    #ifdef DEGUB
    std::cout<<"8. ActiveList & DeleteEnd initialize complete. <image.cpp>"<<std::endl;
    #endif
}

void Image::deletePixelInActiveList(int row, int col){
    std::list<Pixel *>::iterator iter = activeList[row][col].begin();
    while((iter != ++deleteEnd[row][col]) && (iter != activeList[row][col].end())){
        if(noMoreNeighborToBeTest((*iter)->Row(), (*iter)->Col()) ){
            iter = activeList[row][col].erase(iter);
        }else{
            ++iter;
        }
    }
}

void Image::updateExpandQueue(int Row, int Col){
    std::list<Pixel *>::iterator iter =  deleteEnd[Row][Col];
    while(iter != activeList[Row][Col].end()){  //while loop 1
        int row = (*iter) -> Row();
        int col = (*iter) -> Col();
        int IDROW = (*iter) -> idRow();
        int IDCOL = (*iter) -> idCol();
        for(int i = -1; i <= 1; ++i) {  //for loop 2
            
            for(int j = -1; j <= 1;++j) {   //for loop 3
                int newRow = posCheckRow(row+i);//posCheckY
                int newCol = posCheckCol(col+j);//posCheckX                    
                if((row==newRow)&&(col==newCol)){   // comapre with itself
                    continue;
                }
                if(UNTEST==imagePixel[newRow][newCol].Flag()){  // a new pixel waiting to be tested
                    imagePixel[newRow][newCol].Flag(TESTING);
                    imagePixel[newRow][newCol].idRow(IDROW);
                    imagePixel[newRow][newCol].idCol(IDCOL);
                    expandQueue.push(&imagePixel[newRow][newCol]);
                    continue;
                }else{
                    continue;
                }
             }  //end for loop 3
         }  //end for loop 2
    ++iter;   
    }   // end while loop 1
} // end function

void Image::updateActiveList(int FACTOR) {
    while( !expandQueue.empty()){
        Pixel* temp = expandQueue.top();
        int row = temp->idRow();
        int col = temp->idCol();
        int pixelSD = temp->SdValue();
        int pixelGray = temp->Gray();
        int diffSD=pixelSD-(sumSD[row][col]/childPixelNum[row][col]);
        int diffGray=pixelGray-(sumGray[row][col]/childPixelNum[row][col]);
        if(temp->GValue() + diffSD*diffSD + diffGray*diffGray<=FACTOR){
            --StillNeedProcess;
            expandQueue.pop();
            temp->Flag(TESTED);
            sumSD[row][col] += pixelSD;
            sumGray[row][col] += pixelGray;
            ++childPixelNum[row][col];
            activeList[temp->idRow()][temp->idCol()].push_back(temp);
            #ifdef DEBUG
            if(!(StillNeedProcess%1000)){
                std::cout << "still need calculte: " << StillNeedProcess<<std::endl;
            }
            #endif
        }else{
            break;
        }
    }
}

int Image::posCheckCol(int col){
    if(col<0){
        return 0;
    }else if(col>xres-1){
        return xres-1;
    }else{
        return col;
    }
}
    
int Image::posCheckRow(int row){
    if(row<0){
        return 0;
    }else if(row>yres-1){
        return yres-1;
    }else{
        return row;
    }
}

bool Image::noMoreNeighborToBeTest(int row, int col) {   
    for( int i = -1; i <= 1; ++i) {
        for( int j = -1; j <= 1; ++j ) {
            if( imagePixel[posCheckRow(row+i)][posCheckCol(col+j)].Flag() == UNTEST ){
                return false;    
            }
        }
    }
    return true;
}
