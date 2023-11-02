#include <vector>
#include <string> // Add this line
#include <fstream>
#include <cmath>

class WavQuant {
public:
    WavQuant(int numBits);

    // Quantize a single sample
    short quantizeSample(short sample);

    // Quantize a vector of samples
    std::vector<short> quantizeSamples(const std::vector<short>& samples);

    // Get the quantization level
    short getQuantizationLevels() const;

    // Save quantized samples to a text file
    void saveQuantizedSamplesToTextFile(const std::vector<short>& quantizedSamples, const std::string& filename);

private:
    int numBits;
    short quantizationLevels;
};

WavQuant::WavQuant(int numBits) {
    this->numBits = numBits;
    this->quantizationLevels = static_cast<short>(pow(2, numBits) - 1);
}

short WavQuant::quantizeSample(short sample) {
    return static_cast<short>(round(static_cast<double>(sample) * quantizationLevels / 32767.0));
}

std::vector<short> WavQuant::quantizeSamples(const std::vector<short>& samples) {
    std::vector<short> quantizedSamples;
    quantizedSamples.reserve(samples.size());

    for (short sample : samples) {
        quantizedSamples.push_back(quantizeSample(sample));
    }

    return quantizedSamples;
}

short WavQuant::getQuantizationLevels() const {
    return quantizationLevels;
}

void WavQuant::saveQuantizedSamplesToTextFile(const std::vector<short>& quantizedSamples, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (short sample : quantizedSamples) {
            outFile << sample << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Error: Unable to open the output text file for writing." << std::endl;
    }
}

