#include <vector>
#include <cmath>

class WavQuant {
public:
    WavQuant(int numBits);

    // Só para 1 sample
    short quantizeSample(short sample);

    // Vetor de samples
    std::vector<short> quantizeSamples(const std::vector<short>& samples);

    // nivel
    short getQuantizationLevels() const;

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
