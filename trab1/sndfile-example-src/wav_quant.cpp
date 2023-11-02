#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_quant.h"
#include <fstream>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: wavquant <inputFile> <outputFile> <numBits>\n";
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    int numBits = atoi(argv[3]);

    SndfileHandle sfh(inputFile);

    if (sfh.error()) {
        cerr << "Error: Invalid input file\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: File is not in WAV format\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: File is not in PCM_16 format\n";
        return 1;
    }

    WavQuant quantizer(numBits);

    vector<short> samples(FRAMES_BUFFER_SIZE * sfh.channels());
    vector<short> quantizedSamples;

    SndfileHandle sfhOut(outputFile, SFM_WRITE, sfh.format(), sfh.channels(), sfh.samplerate());

    while (size_t nFrames = sfh.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        // Quantize the samples
        quantizedSamples = quantizer.quantizeSamples(samples);

        // Save the quantized samples to a text file
        const std::string textOutputFile = "quantized_samples.txt";
        quantizer.saveQuantizedSamplesToTextFile(quantizedSamples, textOutputFile);

        // Write the quantized samples to the WAV file
        sfhOut.writef(quantizedSamples.data(), nFrames);
    }

    cout << "Quantization completed with " << numBits << " bits per sample.\n";

    return 0;
}
