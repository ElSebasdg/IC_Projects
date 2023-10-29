#include <iostream>
#include <vector>
#include <fstream>
#include <sndfile.hh>
#include "wav_quant.h" 

constexpr size_t FRAMES_BUFFER_SIZE = 65536;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: wav2txt <inputFile> <outputFile> <numBits>\n";
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    int numBits = atoi(argv[3]);

    SndfileHandle sfh(inputFile);

    if (sfh.error()) {
        std::cerr << "Error: Invalid input file\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        std::cerr << "Error: File is not in WAV format\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        std::cerr << "Error: File is not in PCM_16 format\n";
        return 1;
    }

    WavQuant quantizer(numBits);

    std::vector<short> samples(FRAMES_BUFFER_SIZE * sfh.channels());

    std::ofstream outFile(outputFile);

    while (size_t nFrames = sfh.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        for (size_t i = 0; i < nFrames; ++i) {
            short quantizedSample = quantizer.quantizeSample(samples[i]);
            outFile << quantizedSample << '\n';
        }
    }

    std::cout << "Conversion completed with " << numBits << " bits per sample. Data saved to " << outputFile << '\n';

    return 0;
}
