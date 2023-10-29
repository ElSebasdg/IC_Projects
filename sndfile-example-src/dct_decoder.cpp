#include <iostream>
#include <vector>
#include <cmath>
#include <fftw3.h>
#include <sndfile.hh>
#include "BitStream.h"

using namespace std;


int main(int argc, char *argv[]) {

    int nChannels{1};
    size_t nFrames{1};
    size_t bs{1024};
    auto sr{441000};
    double dctFrac{0};

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << "number_of_channels\n";
        cerr << "                        coded_wav\n";
        cerr << "                        wavOut\n";
        return 1;
    }
    BitStream bitStream = BitStream{argv[argc - 2], false}; //read
    
    nFrames = bitStream.readNBits(24);
    nChannels = bitStream.readNBits(16);
    bs = bitStream.readNBits(16);
    sr = bitStream.readNBits(16);
    dctFrac = bitStream.readNBits(16) / 100.0;

    nChannels = 1;

    cout << "NFrames: " << nFrames << endl;
    cout << "NChannels: " << nChannels << endl;
    cout << "bs: " << bs << endl;
    cout << "SR: " << sr << endl;
    cout << "DCT Frac: " << dctFrac << endl;


    vector<short> samples(nChannels * nFrames);

    size_t nBlocks{static_cast<size_t>(ceil(static_cast<double>(nFrames) / bs))};

    // Do zero padding, if necessary
    samples.resize(nBlocks * bs * nChannels);

    // Vector for holding all DCT coefficients, channel by channel
    vector <vector<double>> x_dct(nChannels, vector<double>(nBlocks * bs));

    // Vector for holding DCT computations
    vector<double> x(bs);

    cout << x_dct[0][0];

    for (size_t n = 0; n < nBlocks; n++) {
        for (size_t c = 0; c < nChannels; c++) {
            for (size_t k = 0; k < bs * dctFrac; k++) {
                int16_t value = static_cast<int16_t>(bitStream.readNBits(16));
                cout << value << endl;
                x_dct[c][n * bs + k] = static_cast<double>(value);
            }
        }
    }
    SndfileHandle sfhOut{argv[argc - 1], SFM_WRITE, 65538,
                         nChannels, sr};

    cout << "FORMAT " << sfhOut.format() << endl;
    cout << "Channels " << sfhOut.channels() << endl;
    cout << "SR " << sfhOut.samplerate() << endl;
    // Inverse DCT
    fftw_plan plan_i = fftw_plan_r2r_1d(bs, x.data(), x.data(), FFTW_REDFT01, FFTW_ESTIMATE);
    for (size_t n = 0; n < nBlocks; n++)
        for (int c = 0; c < nChannels; c++) {
            for (size_t k = 0; k < bs; k++) {
                x[k] = x_dct[c][n * bs + k];
            }
            fftw_execute(plan_i);
            for (size_t k = 0; k < bs; k++)
                samples[(n * bs + k) * nChannels + c] = static_cast<short>(round(x[k]));

        }

    sfhOut.writef(samples.data(), nFrames);

    return 0;
}