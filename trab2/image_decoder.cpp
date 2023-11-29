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

    int imgwidth = bs.readNBits(16);
    int imgheight = bs.readNBits(16);
    int enc = bs.readNBits(32);
    int msize = bs.readNBits(16); 
    //cout << "msize:" << msize << endl;

    Mat new_image = Mat::zeros(imgheight, imgwidth, CV_8U);
    
    vector<int> m_vector;
for (int i = 0; i < msize; i++) {
    int m = bs.readNBits(16); // Read 16 bits for m

    m_vector.push_back(m);
    //cout << "m:" << m << endl;
}


    
    vector<int> encoded_values;
    for (int i = 0; i < enc; i++) {
        encoded_values.push_back(bs.readBit());
    }

    string encodedString = "";
    for(long unsigned int i = 0; i < encoded_values.size(); i++) {
        encodedString += to_string(encoded_values[i]);
    }
    
    Golomb g;
    vector<int> decoded;

    if (msize == 1) {
        decoded = g.decode(encodedString, m_vector[0]);
    } else {
        decoded = g.decodeMultiple(encodedString, m_vector, imgwidth);
    }

    //undo the predictions
    int pixel_idx = 0;
    for (int i = 0; i < imgheight; i++) {
        for (int j = 0; j < imgwidth; j++) {
            if (i == 0 && j == 0) {
                //create a new pixel with the decoded values at the current pixel index and add it to the image 
                new_image.at<uchar>(j, i) = uchar(decoded[pixel_idx]);
                pixel_idx++;
            } else if (i == 0) {
                //if its the first line of the image, use only the previous pixel (to the left)
                int pixel = new_image.at<uchar>(i, j-1) + decoded[pixel_idx];
                new_image.at<uchar>(i, j) = uchar(pixel);
                pixel_idx++;
            } else if (j == 0) {
                //if its the first pixel of the line, use only the pixel above
                int pixel = new_image.at<uchar>(i-1, j) + decoded[pixel_idx];
                new_image.at<uchar>(i, j) = uchar(pixel);
                pixel_idx++;
            } else {
                //if its not the first pixel of the image nor the first line, use the 3 pixels to the left, above and to the left top
                int pixel = int(predict(new_image.at<uchar>(i, j-1), new_image.at<uchar>(i-1, j), new_image.at<uchar>(i-1, j-1))) + decoded[pixel_idx];
                new_image.at<uchar>(i, j) = uchar(pixel);
                pixel_idx++;
            }
        }
    }

    cout << "foi aqui?" << endl;
 
    cout << "foi" << endl;
  
    //save the image
    imwrite(output, new_image);

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Execution time: " << elapsed_secs << " ms" << endl;

    return 0;
}
