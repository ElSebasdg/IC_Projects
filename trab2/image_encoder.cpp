#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "BitStream.h" 
#include "golomb.h"

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

    //function to calculate m based on u
    auto calc_m = [](int u) {
        //u = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) u / (1 + u)));
    };

    if (argc < 3 || argc > 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output file> \n";
        return 1;
    }
    
    string output = argv[2];
    Mat img = imread(argv[1], IMREAD_GRAYSCALE); // Load image in grayscale
    
    // Apply the JPEG-LS predictor function to generate prediction errors
    Mat predictionErrors(img.rows, img.cols, CV_8U);
    JPEG_LS_predictor(img, predictionErrors);

    // Check if the image is loaded
    if (img.empty()) {
        cout << "Could not load image: " << argv[1] << endl;
        return -1;
    }

    vector<int> valuesToBeEncoded;
    vector<int> m_vector;

    // For each pixel in the image
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            int predicted;
            if (i == 0 || j == 0) {
                predicted = 0; // Maintain separation at image edges
            } else {
                int a = img.at<uchar>(i, j - 1);
                int b = img.at<uchar>(i - 1, j);
                int c = img.at<uchar>(i - 1, j - 1);
                predicted = predict(a, b, c);
            }

            int error = prediction_residual(img.at<uchar>(i, j), predicted);
            predictionErrors.at<uchar>(i, j) = error;
            
            // Store prediction errors in valuesToBeEncoded vector
            valuesToBeEncoded.push_back(error);

            // Calculate 'm' based on prediction errors and store in m_vector
            int m = calc_m(abs(error));
            m_vector.push_back(m);
        }
    }
    
    Golomb golomb(8);

    // Perform Golomb encoding on valuesToBeEncoded vector and write to output file
    BitStream bitStream(output, true);

    // Write image dimensions to the encoded file
    bitStream.writeNBits(img.cols, 32); // Write image width
    bitStream.writeNBits(img.rows, 32); // Write image height
  
    for (int i = 0; i < valuesToBeEncoded.size(); ++i) {
        // Golomb encode 'valuesToBeEncoded[i]' using Golomb encoding function
        vector<bool> encoded_bits = golomb.encode(valuesToBeEncoded[i]);

        // Write the encoded bits to the output file using BitStream
        for (bool bit : encoded_bits) {
            bitStream.writeBit(bit);
        }
    }
    
    bitStream.close(); // Close the BitStream after writing

    return 0;
}
