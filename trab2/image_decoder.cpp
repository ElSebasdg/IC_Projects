#include <iostream>
#include <string>
#include <vector>
#include "BitStream.h"
#include "golomb.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

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

int prediction_residual(int pixel, int prediction) {
    return pixel - prediction;
  }
  
  void JPEG_LS_predictor(cv::Mat img_in, cv::Mat &erro) {
    cv::Size s = img_in.size();
    for (int i = 0; i < s.height; i++) {
      for (int j = 0; j < s.width; j++) {
        int a, b, c;
        
        int pixel = img_in.at<uchar>(i, j);
        
        if (j == 0 && i != 0) {
          a = 0;
          b = img_in.at<uchar>(i - 1, j);
          c = 0;
        } else if (j != 0 && i == 0) {
          a = img_in.at<uchar>(i, j - 1);
          b = 0;
          c = 0;
        } else if (j == 0 && i == 0) {
          a = 0;
          b = 0;
          c = 0;
        } else {
          a = img_in.at<uchar>(i, j - 1);
          b = img_in.at<uchar>(i - 1, j);
          c = img_in.at<uchar>(i - 1, j - 1);
        }
        
        int prediction = predict(a, b, c);
        
        int error = prediction_residual(pixel, prediction);
        
        erro.at<uchar>(i, j) = error;
      }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <encoded file> <output image>\n";
        return 1;
    }

    string encodedFile = argv[1];
    string outputFile = argv[2];

    BitStream bitStream(encodedFile, false); // Open the encoded file for reading

    // Read image dimensions from the encoded file
    int imageWidth = static_cast<int>(bitStream.readNBits(32));
    int imageHeight = static_cast<int>(bitStream.readNBits(32));
    

    Golomb golomb(8);
    vector<int> decodedValues;
    while (!bitStream.eof() && decodedValues.size() < imageWidth * imageHeight) {
        vector<bool> encoded_bits;
        int b_value = golomb.getB(); // Get 'b' value for decoding
        for (int i = 0; i < b_value + 1; ++i) {
            encoded_bits.push_back(bitStream.readBit());
        }
        unsigned int index = 0; // Initialize index as unsigned int
        decodedValues.push_back(golomb.decode(encoded_bits, index)); // Decode using Golomb's decode function
    }

    bitStream.close(); // Close the BitStream after reading

    Mat reconstructedImage(imageHeight, imageWidth, CV_8U);

    int decodedIndex = 0;
    // Reconstruct the image from decoded prediction errors
    for (int i = 0; i < reconstructedImage.rows; ++i) {
        for (int j = 0; j < reconstructedImage.cols; ++j) {
            int predicted;
            int reconstructedPixel;

            if (i == 0 || j == 0) {
                predicted = 0; // Maintain separation at image edges
            } else {
                int a = reconstructedImage.at<uchar>(i, j - 1);
                int b = reconstructedImage.at<uchar>(i - 1, j);
                int c = reconstructedImage.at<uchar>(i - 1, j - 1);
                predicted = predict(a, b, c);
            }

            if (decodedIndex < decodedValues.size()) {
                reconstructedPixel = predicted + decodedValues[decodedIndex];
                reconstructedImage.at<uchar>(i, j) = saturate_cast<uchar>(reconstructedPixel);
                decodedIndex++;
            }
        }
    }

    imwrite(outputFile, reconstructedImage);

    cout << "Image successfully reconstructed and saved as " << outputFile << endl;

    return 0;
}
