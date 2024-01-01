#include "golomb.h"
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "BitStream.h"

using namespace std;
using namespace cv;

//video parameters needed for codec
int width, height, colorspace;
string interlace;                   
vector<int> pixelAspectRatio(2);
vector<int> frameRate(2);
int num_frames = 0;

void parseY4MHeader(const string& filename) {
    ifstream file;
    file.open(filename, ios::in | ios::binary);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't open the file " << filename << std::endl;
        return; 
    }
    string line;
    getline(file, line);
    
    sscanf(line.c_str(), "YUV4MPEG2 W%d H%d F%d:%d", &width, &height, &frameRate[0], &frameRate[1]);
    // check if 'C' is in the line
    if (strchr(line.c_str(), 'C') == NULL) {
        colorspace = 420;
    } else {
        // extract the color space after the 'C'
        colorspace = stoi(line.substr(line.find('C') + 1));
    }
    // check if 'I' is in the line
    if (strchr(line.c_str(), 'I') == NULL) {
        interlace = "p";
    } else {
        // extract the interlace after the 'I' 
        interlace = line.substr(line.find('I') + 1, 1);   
    }
    // check if 'A' is in the line
    if (strchr(line.c_str(), 'A') == NULL) {
        pixelAspectRatio[0] = 1;
        pixelAspectRatio[1] = 1;
    } else {
        // extract the aspect ratio after the 'A'
        string aspect_ratio = line.substr(line.find('A') + 1);
        // extract the first number
        pixelAspectRatio[0] = stoi(aspect_ratio.substr(0, aspect_ratio.find(':')));
        // extract the second number
        pixelAspectRatio[1] = stoi(aspect_ratio.substr(aspect_ratio.find(':') + 1));
    }
    
    if (line.find("FRAME") != string::npos) {
        num_frames++;
    }
    
    while (getline(file, line)) {
          if (line.find("FRAME") != std::string::npos) {
              num_frames++;
          }
    }
      
}

int main(int argc, char *argv[]) {

    //function to calculate m based on p
    auto calc_m = [](int p) {
        //p = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) p / (1 + p)));
    };

    auto predict = [](int a, int b, int c) {
        int p;
        if (c >= std::max(a, b)) {
          p = std::min(a, b);
        } else if (c <= std::min(a, b)) {
          p = std::max(a, b);
        } else {
          p = a + b - c;
        }
        
        return p;
    };

    //start a timer
    clock_t start = clock();

    // check if the number of arguments is correct
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> \n";
        return 1;
    }

    //output file
    string output = argv[2];
    
    //open input file
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        cout << "Error: Could not open input file" << endl;
        return 1;
    }
    
    parseY4MHeader(argv[1]);

    int blockSize = width; //? 
    
    if (colorspace != 420 && colorspace != 422 && colorspace != 444) {
        cout << "Error: Color space not supported" << endl;
        return 1;
    }

    vector<int> Y, U, V;
    vector<short> Ym;
    vector<int> encoded_bits;

    vector<int> m_vector;
    vector<int> valuesToBeEncoded;

    int numFrames = 0;
    Y = vector<int>(width * height);  //for Y values

    if(colorspace == 420){     //for U and V values (color info)
        U = vector<int>(width * height / 4);
        V = vector<int>(width * height / 4);
    } else if(colorspace == 422){
        U = vector<int>(width * height / 2);
        V = vector<int>(width * height / 2);
    } else if(colorspace == 444){
        U = vector<int>(width * height);
        V = vector<int>(width * height);
    }

    char line[100]; //Read the line after the header
    fgets(line, 100, input);
    
    //cout << "working" <<endl;

    while(!feof(input)){      //loop through each frame
        //reset for each frame
        m_vector = vector<int>();
        valuesToBeEncoded = vector<int>();
        
        numFrames++;
        //read the frame line
        fgets(line, 100, input);
        //read the Y data  (Height x Width)
        for(int i = 0; i < width * height; i++){
            Y[i] = fgetc(input);   
            if(Y[i] < 0) {
                numFrames--;
                break;
            }
        }
        
        if (colorspace == 420) { //extract the U and V values from the frames (but not use them !!)
            for(int i = 0; i < width * height / 4; i++) U[i] = fgetc(input); //read the U data (Height/2 x Width/2)
            for(int i = 0; i < width * height / 4; i++) V[i] = fgetc(input); //read the V data (Height/2 x Width/2)
        } else if(colorspace == 422){
            for(int i = 0; i < width * height / 2; i++) U[i] = fgetc(input); //read the U data (Height x Width/2)
            for(int i = 0; i < width * height / 2; i++) V[i] = fgetc(input); //read the V data (Height x Width/2)
        } else if(colorspace == 444){
            for(int i = 0; i < width * height; i++) U[i] = fgetc(input); //read the U data (Height x Width)
            for(int i = 0; i < width * height; i++) V[i] = fgetc(input); //read the V data (Height x Width)
        }
        
        //Create Mat objects for only for Y
        Mat YMat = Mat(height, width, CV_8UC1);
        
        //copy the Y data into the Mat object
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++) YMat.at<uchar>(i, j) = Y[i * width + j];
        }
        
        int pixel_index = 0;
        
        //go pixel by pixel through the Y Mat object to make predictions
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                int Y = YMat.at<uchar>(i, j);
                //if its the first pixel of the image, do not use prediction
                if (i == 0 && j == 0) {
                    valuesToBeEncoded.push_back(Y);
                    pixel_index++;
                } else if(i==0){
                    //if the pixel is in the first line of the image, use only the pixel to its left
                    valuesToBeEncoded.push_back(Y - YMat.at<uchar>(i, j-1));
                    pixel_index++;
                } else if(j==0){
                    //if the pixel is the first of the line, use only the pixel above
                    valuesToBeEncoded.push_back(Y - YMat.at<uchar>(i-1, j));
                    pixel_index++;
                } else {
                    //for all other pixels, use 3 pixels (to the left, above and to the top left)
                    valuesToBeEncoded.push_back(Y - predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1)));
                    pixel_index++;
                }
                
                //m_vector calculation
                if (pixel_index % width == 0 and pixel_index != 0) {
                    int sum = 0;
                    for (int j = pixel_index - width; j < pixel_index; j++) {
                        sum += abs(valuesToBeEncoded[j]);
                    }
                    
                    int p = round(sum / width);
                    
                    int m = calc_m(p);
                    
                    if (m < 1) m = 1;
                    
                    m_vector.push_back(m);                
                }
            }
        }

        string encodedString = "";

        Golomb g;

        int size = width * height;

        int m_index = 0;
        for (int i = 0; i < size; i++) {
            if (i % width == 0 && i != 0) {
                Ym.push_back(m_vector[m_index]);
                m_index++;
            }
            encodedString += g.encode(valuesToBeEncoded[i], m_vector[m_index]);
            if (i == valuesToBeEncoded.size() - 1) Ym.push_back(m_vector[m_index]);   //!
        }
        
        for (long unsigned int i = 0; i < encodedString.length(); i++)
                encoded_bits.push_back(encodedString[i] - '0');

    } //end of loop for each frame

    BitStream bitStream(output, true);
   
    bitStream.writeNBits(width, 16); // Write image width (16 bits)
    bitStream.writeNBits(height, 16); // Write image height (16 bits)
    bitStream.writeNBits(numFrames, 16); // Write the number of frames (16 bits)
    bitStream.writeNBits(pixelAspectRatio[0], 16); // Write the first number in the pixel aspect ratio (16 bits)
    bitStream.writeNBits(pixelAspectRatio[1], 16); // Write the second number in the pixel aspect ratio (16 bits)
    bitStream.writeNBits(frameRate[0], 16); // Write the first number in the frame rate (16 bits)
    bitStream.writeNBits(frameRate[1], 16); // Write the first number in the frame rate (16 bits)
    bitStream.writeString(interlace); // Write interlace (string ->of just one char)
    bitStream.writeNBits(blockSize, 16); // Write the block size(16 bits)
    bitStream.writeNBits(encoded_bits.size(), 32); // Write the encoded_bits.size() (32 bits)
    bitStream.writeNBits(Ym.size(), 32); // Write the Ym.size() (32 bits)
    
    
    // Write Ym values (8 bits each)
    for (size_t i = 0; i < Ym.size(); i++) {
        bitStream.writeNBits(Ym[i], 8);
    }
    
    // Write the encoded bits
    for (size_t i = 0; i < encoded_bits.size(); i++) {
        bitStream.writeBit(encoded_bits[i]);
    }
    


    bitStream.close();
    

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time: " << elapsed_secs << " ms" << endl;
 
    return 0;
}

