// audio_codec.cpp
#include "audio_codec.h"

std::vector<bool> AudioEncoder::encode(const std::vector<std::pair<int, int>>& audio) {
    std::vector<bool> encodedBits;

    for (size_t i = 0; i < audio.size(); i++) {
        // Apply quantization based on the target bitrate
        int quantizedSample1 = static_cast<int>(audio[i].first / targetBitrate);
        int quantizedSample2 = static_cast<int>(audio[i].second / targetBitrate);

        auto bits1 = golomb.encode(quantizedSample1);
        auto bits2 = golomb.encode(quantizedSample2);

        encodedBits.insert(encodedBits.end(), bits1.begin(), bits1.end());
        encodedBits.insert(encodedBits.end(), bits2.begin(), bits2.end());
    }

    return encodedBits;
}


std::vector<std::pair<int, int>> AudioDecoder::decode(const std::vector<bool>& bits, unsigned int& index) {
    std::vector<std::pair<int, int>> decodedAudio;

    while (index < bits.size()) {
        int decodedSample1 = golomb.decode(bits, index);
        int decodedSample2 = golomb.decode(bits, index);

        // Perform inverse quantization using the targetBitrate from the associated encoder
        int originalSample1 = decodedSample1 * encoder->getTargetBitrate();
        int originalSample2 = decodedSample2 * encoder->getTargetBitrate();

        decodedAudio.emplace_back(originalSample1, originalSample2);
    }

    return decodedAudio;
}
