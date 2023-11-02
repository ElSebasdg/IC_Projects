#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string>

using namespace cv;
using namespace std;
string getOutputFileName(const string& inputImagePath, const string& channelName);

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("Usage: ./color_channel <Image_Path> <Channel_Number>\n");
        return -1;
    }

    char* inputImagePath = argv[1];
    int channelNumber = atoi(argv[2]);

    Mat image = imread(inputImagePath, 1);

    if (!image.data) {
        printf("Image not found\n");
        return -1;
    }

    if (channelNumber < 0 || channelNumber > 2) {
        printf("Invalid channel number.\nUse 0 for Blue \n1 for Green\n2 for Red.\n");
        return -1;
    }

    vector<Mat> channels;
    split(image, channels);

    Mat extractedChannel = channels[channelNumber];

    string channelName;

    switch (channelNumber) {
        case 0:
            channelName = "Blue";
            break;
        case 1:
            channelName = "Green";
            break;
        default:
            channelName = "Red";
            break;
    }

    string outputFileName = getOutputFileName(inputImagePath, channelName);
    imwrite(outputFileName, extractedChannel);

    namedWindow("Extracted Channel", WINDOW_AUTOSIZE);
    imshow("Extracted Channel", extractedChannel);

    waitKey(0);
    return 0;
}

string getOutputFileName(const string& inputImagePath, const string& channelName) {
    size_t dotPos = inputImagePath.find_last_of(".");
    if (dotPos != string::npos) {
        return "extracted_" + channelName + inputImagePath.substr(dotPos);
    } else {
        printf("Unable to determine input image format. Saving as extracted_channel.png.\n");
        return "extracted_" + channelName + ".png";
    }
}