// audio_codec.h
#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include "golomb.h"

class AudioEncoder;  // Forward declaration

class AudioDecoder {
private:
    Golomb golomb;
    unsigned int blockSize;
    const AudioEncoder* encoder;  // Pointer to the associated encoder

public:
    AudioDecoder(unsigned int m, unsigned int blockSize, const AudioEncoder* encoder)
        : golomb(m), blockSize(blockSize), encoder(encoder) {}

    std::vector<std::pair<int, int>> decode(const std::vector<bool>& bits, unsigned int& index);

    // Public method to access golomb
    Golomb& getGolomb() {
        return golomb;
    }
};

class AudioEncoder {
private:
    Golomb golomb;
    unsigned int blockSize;
    double targetBitrate;  // Target average bitrate in bits per sample

public:
    AudioEncoder(unsigned int m, unsigned int blockSize, double targetBitrate)
        : golomb(m), blockSize(blockSize), targetBitrate(targetBitrate) {}

    std::vector<bool> encode(const std::vector<std::pair<int, int>>& audio);

    // Public method to access targetBitrate
    double getTargetBitrate() const {
        return targetBitrate;
    }

    // Allow AudioDecoder to access private members
    friend class AudioDecoder;
};

#endif // AUDIO_CODEC_H
