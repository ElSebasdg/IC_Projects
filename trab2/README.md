
# IC - Informação e Codificação

Lab class number 2.

In this work, besides dealing with audio files, you will also need to manipulate image files.


# Table of Contents

1. [Getting Started](#getting-started)
   - [Prerequisites](#prerequisites)
   - [Opencv installation](#opencv_installation)
   - [Project installation](#project_installation)

2. [How to run example 1](#example_1)

3. [How to run example 2](#example_2)

4. [How to run example 6](#example_4)

5. [How to run example 6](#example_5)

9. [Documentation](#documentation)

## Getting Started: <a name="getting-started"></a>

This is an example of how you may give instructions on setting up your project locally using the material given in classes.
To get a local copy up and running follow these simple example steps.


### Prerequisites:<a name="prerequisites"></a>


This work will be done using the OpenCV library (https://opencv.org/).
Therefore, we are going to start by installing this library and take a look at https://docs.opencv.org/4.x/de/d7a/tutorial_table_of_content_core.html 

### Opencv installation:<a name="opencv_installation"></a>

To get the Opencv software running you should folow the sext steps:

1. Install required packages and tools
   ```sh
   sudo apt install -y g++ cmake make git libgtk2.0-dev pkg-config
   ```
2. Clone the latest version of OpenCV
   ```sh
    git clone https://github.com/opencv/opencv.git
   ```
3. Create the build directory and go into it
   ```sh
   mkdir -p build && cd build
   ```
4. Generate the build scripts using cmake
   ```sh
   cmake ../opencv
   ```
5. Build the source using make:
   ```sh
   make -j4
   ```
   `⚠️ Note: that it will take some time depending upon your CPU power`

6. Finally, install the OpenCV package
   ```sh
   sudo make install
   ```

ℹ️`Instalation source: https://www.geeksforgeeks.org/how-to-install-opencv-in-c-on-linux/`ℹ️


### Setup Project:<a name="project_installation"></a>


1. Create build directory and go to it: 
   ```sh
   mkdir -p build && cd build
   ```
2. Execute "cmake .." to configure project using the CMakeLists.txt file
   ```sh
   cmake ..
   ```
3. Finally do "make" to compile the project
   ```sh
   make
   ```


## How to run example 1: <a name="example_1"></a>


1. To compile the `color_channel.cpp` you just need to do `make` inside the build directory:
   ```sh
   make
   ```
2.  Finally to execute the following command `./color_channel` followed by image path and the channel number.
   ```sh
   ./color_channel ../horacio 1
   ```


`ℹ️ Note: Use 0 for Blue channel, 1 for Green channel and 2 for Red.`

`⚠️ Attention: Be careful with the image path.`

`📁 The color channel image is stored inside the build directory`



## How to run example 2: <a name="example_2"></a>


1. To compile the `ex2.cpp` you just need to do `make` inside the build directory:
   ```sh
   make
   ```
2.  Finally to execute the following command `./ex2` followed by image path, the rotation_angle and light intensity value.
   ```sh
   ./ex2 ../horacio.png 90 50
   ```


`ℹ️ Note: The rotation angle must be multiple of 90 degrees and the light intensity value must be between 0 and 255`

`⚠️ Attention: Be careful with the image path.`


`📁 The images are stored inside the build directory`

## How to run example 4: <a name="example_4"></a>

1. To compile the `audio_encoder.cpp` and `audio_decoder.cpp` you just need to do `make` inside the build directory:
   ```sh
   make
   ```
2.  Execute the following command `./audio_encoder` followed by image path and binary output file to encode the image.
   ```sh
   ./audio_encoder ../sample.wav encoded-sample.bin
   ```
3.  Execute the following command `./audio_decoder` followed by binary file and the output image to decode the image.
   ```sh
   ./audio_decoder encoded-sample.bin decoded-sample.wav
   ```
   
`⚠️ Attention: Be careful with the audio path.`


`📁 The output binary and audio files are stored inside the build directory`
  

## How to run example 5: <a name="example_5"></a>

1. To compile the `image_encoder.cpp` and `image_decoder.cpp` you just need to do `make` inside the build directory:
   ```sh
   make
   ```
2.  Execute the following command `./image_encoder` followed by image path and binary output file to encode the image.
   ```sh
   ./image_encoder ../bartolomeu.jpg bart.bin
   ```
3.  Execute the following command `./image_decoder` followed by binary file and the output image to decode the image.
   ```sh
   ./image_decoder bart.bin reconstructed-bart.jpg
   ```
   
   
`⚠️ Attention: Be careful with the image path.`


`📁 The output binary and image files are stored inside the build directory`




## Documentation <a name="documentation"></a>

[Lab work n◦2](https://elearning.ua.pt/pluginfile.php/4438659/mod_resource/content/2/trab2.pdf)

