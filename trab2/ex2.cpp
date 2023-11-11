#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
    
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << "path_to_image rotation_angle light_intensity_value" << std::endl;
        return -1;
    }


    // image
    cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);


    if (img.empty()) {
        std::cout << "Could not open or find the image at path: " << argv[1] << std::endl;
        return -1;
    }

    int rotationAngle = std::stoi(argv[2]);
    if (rotationAngle % 90 != 0) {
        std::cout << "Rotation angle must be a multiple of 90 degrees" << std::endl;
        return -1;
    }

    int light_intensity_value = std::stoi(argv[3]);
    if (light_intensity_value < 0 || light_intensity_value > 255) {
        std::cout << "The light intensity value must be between 0 and 255" << std::endl;
        return -1;
    }






    // criar matris para negativo
    cv::Mat img_negative = cv::Mat::zeros(img.size(), img.type());

    // matris vertical horizontal
    cv::Mat img_mirror_horizontal, img_mirror_vertical;

    // Create new matrices to hold the modified images
    cv::Mat img_light, img_dark;

   
    // a)
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            for (int c = 0; c < img.channels(); c++) { // canais (B, G, R)
                // Subtract the current pixel value from the maximum possible value
                img_negative.at<cv::Vec3b>(y, x)[c] = 255 - img.at<cv::Vec3b>(y, x)[c];
            }
        }
    }
    cv::imwrite("negative_image.jpg", img_negative);

    // b)
    cv::flip(img, img_mirror_horizontal, 1);
    cv::flip(img, img_mirror_vertical, 0);

    // Save the mirrored images
    cv::imwrite("mirror_horizontal.jpg", img_mirror_horizontal);
    cv::imwrite("mirror_vertical.jpg", img_mirror_vertical);



    // c)
    cv::Mat img_rotated_minus_90, img_rotated_minus_180, img_rotated_minus_270, img_rotated_90, img_rotated_180, img_rotated_270;
    switch(rotationAngle){
        case 90:
            // Rotate the image 90 degrees
            cv::rotate(img, img_rotated_90, cv::ROTATE_90_CLOCKWISE);
            cv::imwrite("rotated_90.jpg", img_rotated_90);
            break;
        case 180:
            // Rotate the image 180 degrees
            cv::rotate(img, img_rotated_180, cv::ROTATE_180);
            cv::imwrite("rotated_180.jpg", img_rotated_180);
            break;
        case 270:
            // Rotate the image 270 degrees clockwise
            cv::rotate(img, img_rotated_270, cv::ROTATE_90_COUNTERCLOCKWISE);
            cv::imwrite("rotated_270.jpg", img_rotated_270);
            break;

        case -90:
            // Rotate the image -90
            cv::rotate(img, img_rotated_minus_90, cv::ROTATE_90_COUNTERCLOCKWISE);
            // Save the rotated images
            cv::imwrite("rotated_minus_90.jpg", img_rotated_minus_90);
            break;
        case -180:
            // Rotate the image -180 degrees
            cv::rotate(img, img_rotated_minus_180, cv::ROTATE_180);
            cv::imwrite("rotated_minus_180.jpg", img_rotated_minus_180);
            break;
        case -270:
            // Rotate the image -270 degrees
            cv::rotate(img, img_rotated_minus_270, cv::ROTATE_90_CLOCKWISE);
            cv::imwrite("rotated_minus_270.jpg", img_rotated_minus_270);
            break;
    }

    // d)
    img.convertTo(img_light, -1, 1, light_intensity_value);
    img.convertTo(img_dark, -1, 1, -light_intensity_value);

    // Save the modified images
    cv::imwrite("ligh_image.jpg", img_light);
    cv::imwrite("dark_image.jpg", img_dark);
    return 0;
}
