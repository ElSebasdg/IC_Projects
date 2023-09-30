
# IC - Informação e Codificação

Lab class number 1

# Table of Contents

1. [Getting Started](#getting-started)
   - [Prerequisites](#prerequisites)
   - [Installation](#installation)
     - [In case of error](#in-case-of-error)
   - [To test](#to-test)
2. [How to run example 1](#how-to-run-example-1)
3. [How to run example 2](#how-to-run-example-2)
4. [How to run example 3](#how-to-run-example-3)
5. [How to run example 4](#how-to-run-example-4)
6. [How to run example 5](#how-to-run-example-5)
7. [How to run example 6](#how-to-run-example-6)
8. [Documentation](#documentation)

## Getting Started: <a name="getting-started"></a>

This is an example of how you may give instructions on setting up your project locally using the material given in classes.
To get a local copy up and running follow these simple example steps.

### Prerequisites:<a name="prerequisites"></a>
Download zip file(available from elearning): [sndfile-example-src.tar.gz](https://elearning.ua.pt/mod/resource/view.php?id=1143438)

Note that the `libsndfile` is written in C, but that there is a C++ wrapper (sndfile.hh).
### Installation:<a name="installation"></a>

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

 
### In case of error you should follow the next commands to install library `libsndfile`: <a name="in-case-of-error"></a>

 ```sh
 sudo apt-get install -y libsndfile1-dev
 
 sudo apt-get install libfftw3-dev libfftw3-doc

 make
 ```

### To test: <a name="to-test"></a>
Inside: `../sndfile-example-src` you should follow the next commands:

1. Copies "sample.wav" into "copy.wav"
```sh
../sndfile-example-bin/wav_cp sample.wav copy.wav 
```
2.  Outputs the histogram of channel 0
```sh
../sndfile-example-bin/wav_hist sample.wav 0
```

## How to run example 1: <a name="how-to-run-example-1"></a>

Inside: `../sndfile-example-src` you should follow the next commands:

1. Create executable file:
```sh
g++ -o wav_hist wav_hist.cpp -lsndfile 
```
2. Execute using`.wav`file:
```s
./wav_hist out.wav 1 
```

## How to run example 2: <a name="how-to-run-example-2"></a>

Inside: `../sndfile-example-src` you should follow the next commands:

1. Create executable file:
```sh
g++ -o wavcmp wav_cmp.cpp -lsndfile
```
2. Execute using`.wav`files:
```s
./wavcmp -v sample.wav out.wav
```

## How to run example 3: <a name="how-to-run-example-3"></a>

Inside: `../sndfile-example-src` you should follow the next commands:

1. Create executable file:
```sh
g++ -o wavquant wav_quant.cpp -lsndfile
```
2. Execute using`.wav`files:
```s
./wavquant sample.wav out.wav 4
```
3. Test with different values and listen to out.wav : 
```s
./wavquant sample.wav out.wav 3
```
```s
./wavquant sample.wav out.wav 2
```
```s
./wavquant sample.wav out.wav 1
```


## How to run example 4: <a name="how-to-run-example-4"></a>

Inside: `../sndfile-example-src` you should follow the next commands:

1. Compile the program:
```sh
g++ -o wav_effects wav_effects.cpp -lsndfile -lm
```

2. Execute using`.wav`files and choose an effect (1-6): 
```s
./wav_effects sample.wav out.wav 1
```
3. Test with different values and listen to out.wav : 
```s
../wav_effects  sample.wav out.wav 3
```
```s
./wav_effects  sample.wav out.wav 6
```
```s
./wav_effects  sample.wav out.wav 5

```


## How to run example 5: <a name="how-to-run-example-5"></a>

1. Create executable test file:
```sh
g++ test_ex5.cpp -o test_ex5
```
2. Execute:
```s
./test_ex5
```


## How to run example 6: <a name="how-to-run-example-6"></a>

1. Create executable test file:
```sh
g++ decoder_encoder.cpp -o decoder_encoder
```
2. To use encoder:
```s
./decoder_encoder encoder 01_file.txt encoded.bin
```

2. To use decoder:
```s
./decoder_encoder decoder encoded.bin 01_file.txt
```



## Documentation <a name="documentation"></a>



[Lab work n◦1](https://elearning.ua.pt/pluginfile.php/4438659/mod_resource/content/2/trab1.pdf)

