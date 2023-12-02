#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat createNegativeImage(const Mat& original);
void saveImage(const Mat& image, const string& filename);
Mat createMirroredImage(const Mat& original, bool horizontally);
void rotateImage(const Mat& original, int rotationAngle);
void adjustIntensity(const Mat& original, int intensityValue, Mat& result);


int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <image_path> <rotation_angle> <intensity_value>" << endl;
        return -1;
    }

    Mat originalImage = imread(argv[1], IMREAD_COLOR);

    if (!originalImage.data) {
        cout << "No image data." << endl;
        return -1;
    }

    int rotationAngle = stoi(argv[2]);
    if (rotationAngle % 90 != 0) {
        cout << "Rotation angle must be a multiple of 90 degrees." << endl;
        return -1;
    }

    int intensityValue = stoi(argv[3]);
    if (intensityValue < 0 || intensityValue > 255) {
        cout << "The intensity value must be between 0 and 255." << endl;
        return -1;
    }

    // namedWindow("Original Image", WINDOW_NORMAL);
    // imshow("Original Image", originalImage);

    // a)
    Mat negativeImage = createNegativeImage(originalImage);
    // namedWindow("Negative Image", WINDOW_NORMAL);
    // imshow("Negative Image", negativeImage);
    saveImage(negativeImage, "negative_image.jpg");

    // b)
    Mat horizontallyMirroredImage = createMirroredImage(originalImage, true);
    Mat verticallyMirroredImage = createMirroredImage(originalImage, false);

    // namedWindow("Horizontally Mirrored Image", WINDOW_NORMAL);
    // imshow("Horizontally Mirrored Image", horizontallyMirroredImage);
    // namedWindow("Vertically Mirrored Image", WINDOW_NORMAL);
    // imshow("Vertically Mirrored Image", verticallyMirroredImage);

    saveImage(horizontallyMirroredImage, "horizontally_mirrored_image.jpg");
    saveImage(verticallyMirroredImage, "vertically_mirrored_image.jpg");


    // c) 
    rotateImage(originalImage, rotationAngle);

    // d)
    Mat lightenedImage, darkenedImage;

    adjustIntensity(originalImage, intensityValue, lightenedImage);
    adjustIntensity(originalImage, -intensityValue, darkenedImage);

    string lightenedOutputFileName = "lightened.jpg";
    string darkenedOutputFileName = "darkened.jpg";

    imwrite(lightenedOutputFileName, lightenedImage);
    imwrite(darkenedOutputFileName, darkenedImage);

    // namedWindow("Lightened Image", WINDOW_NORMAL);
    // imshow("Lightened Image", lightenedImage);

    // namedWindow("Darkened Image", WINDOW_NORMAL);
    // imshow("Darkened Image", darkenedImage);

    cout << "Lightened image saved as: " << lightenedOutputFileName << endl;
    cout << "Darkened image saved as: " << darkenedOutputFileName << endl;

    waitKey(0);
    destroyAllWindows();

    return 0;
}


void saveImage(const Mat& image, const string& filename) {
    imwrite(filename, image);
    cout << "Image saved as: " << filename << endl;
}


Mat createNegativeImage(const Mat& original) {
    Mat negativeImage = original.clone();
    
    for (int y = 0; y < original.rows; ++y) {
        for (int x = 0; x < original.cols; ++x) {
            Vec3b& pixel = negativeImage.at<Vec3b>(y, x);

            // Invert RGB values
            pixel[0] = 255 - pixel[0]; // Blue
            pixel[1] = 255 - pixel[1]; // Green
            pixel[2] = 255 - pixel[2]; // Red
        }
    }

    return negativeImage;
}

Mat createMirroredImage(const Mat& original, bool horizontally) {
    Mat mirroredImage = Mat::zeros(original.size(), original.type());

    for (int y = 0; y < original.rows; ++y) {
        for (int x = 0; x < original.cols; ++x) {
            int newX, newY;

            if (horizontally) {
                newX = original.cols - x - 1;
                newY = y;
            } else { // vertically
                newX = x;
                newY = original.rows - y - 1;
            }

            mirroredImage.at<Vec3b>(newY, newX) = original.at<Vec3b>(y, x);
        }
    }

    return mirroredImage;
}


void rotateImage(const Mat& original, int rotationAngle) {
    int rows = original.rows;
    int cols = original.cols;

    Mat rotatedImage(rows, cols, original.type());

    switch (rotationAngle) {
        case 90:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(x, rows - y - 1) = original.at<Vec3b>(y, x);
                }
            }
            break;
        case 180:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(rows - y - 1, cols - x - 1) = original.at<Vec3b>(y, x);
                }
            }
            break;
        case 270:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(cols - x - 1, y) = original.at<Vec3b>(y, x);
                }
            }
            break;
        case -90:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(cols - x - 1, y) = original.at<Vec3b>(y, x);
                }
            }
            break;
        case -180:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(rows - y - 1, cols - x - 1) = original.at<Vec3b>(y, x);
                }
            }
            break;
        case -270:
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < cols; ++x) {
                    rotatedImage.at<Vec3b>(x, rows - y - 1) = original.at<Vec3b>(y, x);
                }
            }
            break;
        default:
            cout << "Invalid rotation angle." << endl;
            return;
    }

    string outputFileName = "rotated_" + to_string(rotationAngle) + ".jpg";
    imwrite(outputFileName, rotatedImage);

    namedWindow("Rotated Image", WINDOW_NORMAL);
    imshow("Rotated Image", rotatedImage);

    cout << "Rotated image saved as: " << outputFileName << endl;
}


void adjustIntensity(const Mat& original, int intensityValue, Mat& result) {
    result = Mat::zeros(original.size(), original.type());

    for (int y = 0; y < original.rows; ++y) {
        for (int x = 0; x < original.cols; ++x) {
            for (int c = 0; c < original.channels(); ++c) {
                int newValue = original.at<Vec3b>(y, x)[c] + intensityValue;
                result.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(newValue);
            }
        }
    }
}
