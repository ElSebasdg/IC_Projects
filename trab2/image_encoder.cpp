#include "new_golomb.h"
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
    // read the image in grayscale
    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    
    // check if the image is loaded
    if (img.empty()) {
        cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    //test image type
    //int type = img.type();  
    //cout << "image type: " << type  << endl;

    vector<int> m_vector;
    vector<int> valuesToBeEncoded;
    
    int pixel_index = 0;

    // for each pixel in the image
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            // get the pixel (i,j)
            int pixel = img.at<uchar>(i,j);

            //if its the first pixel of the image, do not use prediction
            if (i == 0 && j == 0) {
                valuesToBeEncoded.push_back(pixel);
                pixel_index++;
            } else if(i==0){
                //if the pixel is in the first line of the image, use only the pixel to its left
                int error = pixel - img.at<uchar>(i,j-1);
                valuesToBeEncoded.push_back(error);
                pixel_index++;
            }else if(j==0){
                //if the pixel is the first of the line, use only the pixel above
                int error = pixel - img.at<uchar>(i-1,j);
                valuesToBeEncoded.push_back(error);
                pixel_index++;
            } else {
                //for all other pixels, use 3 pixels (to the left, above and to the top left)
                int error = pixel - int(predict(img.at<uchar>(i,j-1), img.at<uchar>(i-1,j), img.at<uchar>(i-1,j-1)));
                valuesToBeEncoded.push_back(error);
                pixel_index++;
            }

            if(pixel_index % img.cols == 0 && pixel_index != 0) {
                int sum = 0;
                
                for(int j = pixel_index - img.cols; j < pixel_index; j++) {
                    sum += abs(valuesToBeEncoded[j]);
                }
                int p = round(sum / img.cols);

                int m = calc_m(p);

                if (m < 1) m = 1;

                m_vector.push_back(m);
            }
        }
    }

    string encodedString = "";

    Golomb g;

    //size = rows * cols
    int size = img.rows * img.cols;

    int m_index = 0;
    for (int i = 0; i < size; i++) {
        if (i % img.cols == 0 && i != 0) m_index++;
        //cout << "values to be encoded: " << valuesToBeEncoded[i] << endl;
        
        encodedString += g.encode(valuesToBeEncoded[i], m_vector[m_index]);
    }

    BitStream bitStream(output, true);
   
    bitStream.writeNBits(img.cols, 16); // Write image width (16 bits)
    bitStream.writeNBits(img.rows, 16); // Write image height (16 bits)
    bitStream.writeNBits(encodedString.size(), 32); // Write encodedString size (32 bits)
    bitStream.writeNBits(m_vector.size(), 16); // Write m_vector size (16 bits)
    
    
    // Write m_vector values (16 bits each)
    for (size_t i = 0; i < m_vector.size(); i++) {
        bitStream.writeNBits(m_vector[i], 16);
    }

    //contains all the image's encoded values
    vector<int> encoded_bits;

    for (long unsigned int i = 0; i < encodedString.length(); i++)
        encoded_bits.push_back(encodedString[i] - '0');


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

