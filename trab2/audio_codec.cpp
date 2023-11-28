#include "audio_codec.h"

int main() {
    // Create an instance of AudioCodec with a specific value of m (e.g., 10)
    AudioCodec audioCodec(10);

    // Sample audio data
    std::vector<int> originalAudio = {50, 52, 48, 45, 47};

    // Encode audio data
    BitStream bitStream("encoded_audio.bin", true); // Open for writing
    bool lossyEncoding = true;  // Set to true for lossy encoding
    double bitrate = 128.0;     // Provide the bitrate value
    audioCodec.encodeAudio(originalAudio, bitStream, 1, lossyEncoding, bitrate); // Enable mapping for negative numbers
    bitStream.close();

    // Decode audio data
    std::vector<int> decodedAudio;
    BitStream readStream("encoded_audio.bin", false); // Open for reading
    bool lossyDecoding = true;  // Set to true for lossy decoding
    audioCodec.decodeAudio(decodedAudio, readStream, 1, lossyDecoding, bitrate); // Enable mapping for negative numbers
    readStream.close();

    // Display original and decoded audio data
    std::cout << "Original Audio: ";
    for (int sample : originalAudio) {
        std::cout << sample << " ";
    }
    std::cout << std::endl;

    std::cout << "Decoded Audio: ";
    for (int sample : decodedAudio) {
        std::cout << sample << " ";
    }
    std::cout << std::endl;

    return 0;
}
