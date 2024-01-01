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

int main(int argc, char *argv[]) {

    auto predict = [](int a, int b, int c){
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

    if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
	}

    BitStream bs (argv[1], false);
    string output = argv[2];

    //YUV4MPEG2 output file
    ofstream out(output, ios::out | ios::binary);
    
    vector<int> pixelAspectRatio(2);
    vector<int> frameRate(2);
    
    int width = bs.readNBits(16);
    int height = bs.readNBits(16);
    int num_frames = bs.readNBits(16);
    pixelAspectRatio[0] = bs.readNBits(16);
    pixelAspectRatio[1] = bs.readNBits(16);
    frameRate[0] = bs.readNBits(16);
    frameRate[1] = bs.readNBits(16);
    string interlace = bs.readString(1);
    //cout << "interlace: " << interlace << endl;
    int bs_size = bs.readNBits(16);
    int encoded_bits_size = bs.readNBits(32);
    int Ym_size = bs.readNBits(32);
    
    vector<int> Ym;
    for (int i = 0; i < Ym_size; i++) {
        int m = bs.readNBits(8); // Read 8 bits for m

        Ym.push_back(m);
    }
    
    //read encoded Y values
    vector<int> encoded_bits;
    for (int i = 0; i < encoded_bits_size; i++) {
        encoded_bits.push_back(bs.readBit());
    }
    
    string encodedString = "";
    for(long unsigned int i = 0; i < encoded_bits.size(); i++) {
        encodedString += to_string(encoded_bits[i]);
    }
    
    //write the header
    out << "YUV4MPEG2 W" << width << " H" << height << " F" << frameRate[0] << ":" << frameRate[1] << " I" << interlace << " A" << pixelAspectRatio[0] << ":" << pixelAspectRatio[1] << " Cmono" << endl;

    //write to the file FRAME
    out << "FRAME" << endl;

    //decode Y values
    Golomb g;
    vector<int> Ydecoded = g.decode(encodedString, Ym, bs_size);

    Mat YMat = Mat(height, width, CV_8UC1);

    //undo the predictions
    int pixel_idx = 0;
    for (int n = 0; n < num_frames; n++) {
        YMat = Mat(height, width, CV_8UC1);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 && j == 0) {
                    //create a new pixel with the decoded values at the current pixel index and add it to the image 
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx];
                    pixel_idx++;
                } else if (i == 0) {
                    //if its the first line of the image, use only the previous pixel (to the left)
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i, j-1);
                    pixel_idx++;
                } else if (j == 0) {
                    //if its the first pixel of the line, use only the pixel above
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + YMat.at<uchar>(i-1, j);
                    pixel_idx++;
                } else {
                    //if its not the first pixel of the image nor the first line, use the 3 pixels to the left, above and to the left top
                    YMat.at<uchar>(i, j) = Ydecoded[pixel_idx] + predict(YMat.at<uchar>(i, j-1), YMat.at<uchar>(i-1, j), YMat.at<uchar>(i-1, j-1));
                    pixel_idx++;
                }
            }
        }
        
        //convert the matrix back to a vector
        vector<int> Y_vector;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                Y_vector.push_back(YMat.at<uchar>(i, j));
            }
        }
        
        //write the Y_vector to the file
        for(long unsigned int i = 0; i < Y_vector.size(); i++){
            //convert the int to a byte
            char byte = (char)Y_vector[i];
            //write the byte to the file
            out.write(&byte, sizeof(byte));
        }

        if(n < num_frames - 1) out << "FRAME" << endl;
    }
        
    
    //close the file
    out.close();
    

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;

    return 0;
}
