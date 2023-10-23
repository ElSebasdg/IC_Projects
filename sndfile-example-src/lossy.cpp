#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sndfile.h>
#include "BitStream.h"

class AudioCodec {
private:
    BitStream& bitStream;
    const double quantizationFactor;
    const int blockSize;

public:
    AudioCodec(BitStream& stream, double quantizationFactor, int blockSize)
        : bitStream(stream), quantizationFactor(quantizationFactor), blockSize(blockSize) {}

    // Function to perform the Discrete Cosine Transform (DCT) on an audio block
    void DCT(std::vector<double>& block) {
        int N = block.size();
        std::vector<double> result(N);

        for (int k = 0; k < N; k++) {
            double sum = 0.0;
            for (int n = 0; n < N; n++) {
                double angle = (M_PI * k * (2 * n + 1)) / (2 * N);
                sum += block[n] * cos(angle);
            }
            if (k == 0) {
                result[k] = sum / sqrt(N);
            } else {
                result[k] = sum * sqrt(2.0 / N);
            }
        }

        block = result;
    }

    // Function to quantize the DCT coefficients
    void Quantize(std::vector<double>& block) {
        for (double& coefficient : block) {
            coefficient = std::round(coefficient / quantizationFactor);
        }
    }

    // Function to encode audio and write it to the BitStream
    void EncodeAudio(const std::vector<double>& audioBlock) {
        std::vector<double> transformedBlock = audioBlock;
        DCT(transformedBlock);
        Quantize(transformedBlock);

        for (const double coefficient : transformedBlock) {
            bitStream.writeNBits(static_cast<uint64_t>(coefficient), 16); // Write each coefficient individually
        }
    }

    bool EndOfInputAudio(int blockCount, int totalBlocks) {
        // Check if the number of processed blocks equals the total number of blocks
        return blockCount >= totalBlocks;
    }

    // Function to perform the Inverse DCT on an audio block
    void IDCT(std::vector<double>& block) {
        int N = block.size();
        std::vector<double> result(N, 0.0);

        for (int n = 0; n < N; ++n) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                double ck = (k == 0) ? 1.0 / std::sqrt(2.0) : 1.0;
                sum += ck * block[k] * std::cos((M_PI / N) * (n + 0.5) * k);
            }

            result[n] = (1.0 / std::sqrt(N)) * sum;
        }

        block = result;
    }

    // Function to decode audio from the BitStream
    void DecodeAudio(std::vector<double>& audioBlock) {
        std::vector<double> transformedBlock;

        for (int i = 0; i < blockSize; ++i) {
            uint64_t quantizedCoefficient = bitStream.readNBits(16); // Read each coefficient individually
            transformedBlock.push_back(static_cast<double>(quantizedCoefficient));
        }

        // Apply Inverse DCT
        IDCT(transformedBlock);

        audioBlock = transformedBlock;
    }

    void SaveWavFile(const std::string& fileName, const std::vector<double>& audioData) {
        SF_INFO sfinfo;
        sfinfo.channels = 1; // Mono
        sfinfo.samplerate = 44100; // Sample rate
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // WAV file format, 16-bit PCM

        SNDFILE* file = sf_open(fileName.c_str(), SFM_WRITE, &sfinfo);
        if (!file) {
            std::cerr << "Error opening WAV file for writing" << std::endl;
            return;
        }

        // Write audio data
        sf_writef_double(file, audioData.data(), audioData.size());

        // Close the file
        sf_close(file);
    }
};


int main() {
    double quantizationFactor = 10.0;
    std::string inputFileName = "input_audio.bin";
    std::string outputFileName = "out.wav";
    int blockSize = 1024;

    // Create a simple binary audio file for testing
    const int sampleRate = 44100;  // Sample rate in Hz
    const double duration = 5.0;  // Duration in seconds
    const int numSamples = static_cast<int>(sampleRate * duration);
    const double frequency = 440.0;  // Frequency of the sine wave in Hz
    const double amplitude = 0.5;  // Amplitude of the sine wave

    std::ofstream outputFile(inputFileName, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening the output file." << std::endl;
        return 1;
    }

    for (int i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        double sampleValue = amplitude * std::sin(2.0 * M_PI * frequency * t);

        // Convert the sample to a 16-bit signed integer and write it to the binary file
        int16_t sample16bit = static_cast<int16_t>(sampleValue * INT16_MAX);
        outputFile.write(reinterpret_cast<char*>(&sample16bit), sizeof(int16_t));
    }

    outputFile.close();
    std::cout << "Binary audio file '" << inputFileName << "' created successfully." << std::endl;

    // Now, continue with the rest of your audio codec code (encoding and decoding)

    std::ifstream inputFile(inputFileName, std::ios::binary);
    BitStream bitStream(inputFileName, true);
    AudioCodec encoder(bitStream, quantizationFactor, blockSize);

    // Determine the total number of blocks
    int totalBlocks = 1024;

    // Encoding
    int blockCount = 0;
    while (!encoder.EndOfInputAudio(blockCount, totalBlocks)) {
        std::vector<double> audioBlock(blockSize);

        // Read audio samples from inputFile and store them in audioBlock
        for (int i = 0; i < blockSize; ++i) {
            int16_t sample16bit;
            inputFile.read(reinterpret_cast<char*>(&sample16bit), sizeof(int16_t));
            audioBlock[i] = static_cast<double>(sample16bit) / INT16_MAX;
        }

        // Process and encode audioBlock
        encoder.EncodeAudio(audioBlock);

        // Display the countdown
        int remainingBlocks = totalBlocks - blockCount - 1;
        std::cout << "Remaining blocks: " << remainingBlocks << "\r";
        std::cout.flush();

        blockCount++;
    }

    // Clear the countdown message
    std::cout << "                       " << std::endl;
    bitStream.close(); // Close the BitStream after encoding

    // Decoding
    BitStream readStream(inputFileName, false);
    AudioCodec decoder(readStream, quantizationFactor, blockSize);

    std::vector<double> decodedAudio;
    while (!readStream.eof()) {
        std::vector<double> audioBlock;
        decoder.DecodeAudio(audioBlock);
        decodedAudio.insert(decodedAudio.end(), audioBlock.begin(), audioBlock.end());
    }

    // Save the decoded audio to a WAV file
    decoder.SaveWavFile(outputFileName, decodedAudio);

    std::cout << "Decoded audio saved as '" << outputFileName << "'" << std::endl;

    return 0;
};