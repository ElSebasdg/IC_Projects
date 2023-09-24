
# IC - Informação e Codificação

Lab class number 1


## Getting Started:

This is an example of how you may give instructions on setting up your project locally using the material given in classes.
To get a local copy up and running follow these simple example steps.

### Prerequisites:
Download zip file(available from elearning): [sndfile-example-src.tar.gz](https://elearning.ua.pt/mod/resource/view.php?id=1143438)

Note that the `libsndfile` is written in C, but that there is a C++ wrapper (sndfile.hh).
### Installation:

To get the software running you should folow the sext steps:

1. Decompress `.gz` file
   ```sh
   tar xzvf sndfile-example-src.tar.gz
   ```
2. Change to directory
   ```sh
    cd sndfile-example-src/
    mkdir build
    cd build
   ```
3. Do cmake and make 
   ```sh
   cmake ..
   make
   ```

 
### In case of error you should follow the next commands to install library `libsndfile`:

 ```sh
 sudo apt-get install -y libsndfile1-dev
 
 sudo apt-get install libfftw3-dev libfftw3-doc

 make
 ```

### To test:
Inside: `../sndfile-example-src` you should follow the next commands:

1. Copies "sample.wav" into "copy.wav"
```sh
../sndfile-example-bin/wav_cp sample.wav copy.wav 
```
2.  Outputs the histogram of channel 0
```sh
../sndfile-example-bin/wav_hist sample.wav 0
```

### How to run:

Inside: `../sndfile-example-src` you should follow the next commands:

1. Create executable file:
```sh
g++ -o wav_hist wav_hist.cpp -lsndfile 
```
2. Execute using`.wav`file:
```s
./wav_hist out.wav 1 
```
## Documentation



[Lab work n◦1](https://elearning.ua.pt/pluginfile.php/4438659/mod_resource/content/2/trab1.pdf)

