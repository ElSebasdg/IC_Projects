#include <iostream>
#include <vector>
#include "audio_codec.h"

int main() {
    // Lossless Example
    unsigned int mLossless = 8;  
    unsigned int blockSizeLossless = 4;  

    // Updated: Provide a default targetBitrate for lossless encoding
    AudioEncoder encoderLossless(mLossless, blockSizeLossless, 1.0);
    AudioDecoder decoderLossless(mLossless, blockSizeLossless, &encoderLossless);  // Pass the encoder instance


    std::vector<std::pair<int, int>> audioLossless = {{2, -3}, {1, 0}, {5, -2}, {4, 7}};

    // Encode audio
    std::vector<bool> encodedBitsLossless = encoderLossless.encode(audioLossless);

    // Print encoded bits
    std::cout << "Lossless Encoded Bits: ";
    for (bool bit : encodedBitsLossless) {
        std::cout << bit;
    }
    std::cout << std::endl;

    // Decode audio
    unsigned int indexLossless = 0;  
    std::vector<std::pair<int, int>> decodedAudioLossless;

    // Debugging: Print Golomb-decoded bits for each channel
    while (indexLossless < encodedBitsLossless.size()) {
        int decodedSample1 = decoderLossless.getGolomb().decode(encodedBitsLossless, indexLossless);
        int decodedSample2 = decoderLossless.getGolomb().decode(encodedBitsLossless, indexLossless);

        std::cout << "Lossless Decoded Sample 1: " << decodedSample1 << std::endl;
        std::cout << "Lossless Decoded Sample 2: " << decodedSample2 << std::endl;

        decodedAudioLossless.emplace_back(decodedSample1, decodedSample2);
    }

    // Print original and decoded audio for verification
    std::cout << "Lossless Original Audio: ";
    for (const auto& sample : audioLossless) {
        std::cout << "(" << sample.first << ", " << sample.second << ") ";
    }
    std::cout << std::endl;

    std::cout << "Lossless Decoded Audio: ";
    for (const auto& sample : decodedAudioLossless) {
        std::cout << "(" << sample.first << ", " << sample.second << ") ";
    }
    std::cout << std::endl;

    // Lossy Example
    unsigned int mLossy = 8;  
    unsigned int blockSizeLossy = 4;  

    AudioEncoder encoderLossy(mLossy, blockSizeLossy, 2);
    AudioDecoder decoderLossy(mLossy, blockSizeLossy, &encoderLossy); 

    std::vector<std::pair<int, int>> audioLossy = {{2, -3}, {1, 0}, {5, -2}, {4, 7}};

    // Encode audio
    std::vector<bool> encodedBitsLossy = encoderLossy.encode(audioLossy);

    // Print encoded bits
    std::cout << "Lossy Encoded Bits: ";
    for (bool bit : encodedBitsLossy) {
        std::cout << bit;
    }
    std::cout << std::endl;

    // Decode audio
    unsigned int indexLossy = 0; 
    std::vector<std::pair<int, int>> decodedAudioLossy;

    // Debugging: Print Golomb-decoded bits for each channel
    while (indexLossy < encodedBitsLossy.size()) {
        int decodedSample1 = decoderLossy.getGolomb().decode(encodedBitsLossy, indexLossy);
        int decodedSample2 = decoderLossy.getGolomb().decode(encodedBitsLossy, indexLossy);

        std::cout << "Lossy Decoded Sample 1: " << decodedSample1 << std::endl;
        std::cout << "Lossy Decoded Sample 2: " << decodedSample2 << std::endl;

        decodedAudioLossy.emplace_back(decodedSample1, decodedSample2);
    }

    // Print original and decoded audio for verification
    std::cout << "Lossy Original Audio: ";
    for (const auto& sample : audioLossy) {
        std::cout << "(" << sample.first << ", " << sample.second << ") ";
    }
    std::cout << std::endl;

    std::cout << "Lossy Decoded Audio: ";
    for (const auto& sample : decodedAudioLossy) {
        std::cout << "(" << sample.first << ", " << sample.second << ") ";
    }
    std::cout << std::endl;

    return 0;
}
