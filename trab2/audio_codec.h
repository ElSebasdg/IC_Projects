#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include "golomb.h"
#include "BitStream.h"
#include <vector>

class AudioCodec {
private:
    Golomb golomb;
    int predictedValue; // Preditor para previsão temporal

public:
    AudioCodec(uint32_t m) : golomb(m), predictedValue(0) {}

    void adjustM(double bitrate) {
        // Calcular o bitrate do código Golomb com base no parâmetro 'm' do Golomb
        // Ajustar 'm' com base no bitrate médio alvo especificado pelo usuário
        // Atualizar parâmetros Golomb para codificação e decodificação

        uint32_t adjustedM = static_cast<uint32_t>(bitrate * 1000); // Conversão pra bps
        golomb.setEncodeParameter(adjustedM);
        golomb.setDecodeParameter(adjustedM);
    }

    // Codificar e escrever dados de áudio no fluxo de bits
    void encodeAudio(const std::vector<int>& audioData, BitStream& bitStream, int mappingOn, bool lossy, double bitrate) {
        // Ajustar parâmetro 'm' do Golomb com base no bitrate para codificação com perda       
        if (lossy) {
            adjustM(bitrate);
        }

        for (int sample : audioData) {
            // Calcular residual de previsão
            int residual = sample - predictedValue;

            // Update predictor
            predictedValue = sample;

            // Codificar o residual usando codificação Golomb
            std::string golombCode = golomb.encodeNumber(residual, mappingOn);

            // Debug: Print Golomb 
            std::cout << "Original: " << sample << " | Residual: " << residual << " | Golomb Code: " << golombCode << " | Code Length: " << golombCode.length() << std::endl;

            // Escrever o código Golomb no fluxo de bits
            bitStream.writeString(golombCode);
        }
    }

   // Ler e descodificar dados de áudio do fluxo de bits
    void decodeAudio(std::vector<int>& decodedAudio, BitStream& bitStream, int mappingOn, bool lossy, double bitrate) {
        // Ajustar parâmetro 'm' do Golomb com base no bitrate para decodificação com perda
        if (lossy) {
            adjustM(bitrate);
        }

        while (!bitStream.eof()) {
            // Read Golomb code length
            size_t codeLength = 0;
            char bit = bitStream.readBit();

            // Continue reading bits until a '0' is encountered
            while (bit == '1') {
                codeLength++;
                bit = bitStream.readBit();
            }

            // Read the terminating '0'
            if (bit == '0') {
                // Read Golomb code with the determined length (including unary and binary parts)
                std::string golombCode = bitStream.readString(codeLength + golomb.getEncodeParameter());

                // Debug: Print Golomb code before decoding
                std::cout << "Golomb Code: " << golombCode << " | Code Length: " << codeLength << std::endl;

                // Decode the Golomb code to obtain the prediction residual
                long residual = 0;
                char* pBits = &golombCode[0];
                pBits = golomb.decodeString(pBits, &residual, mappingOn);

                // Update the predictor and add the decoded sample to the audio data
                predictedValue += residual;
                decodedAudio.push_back(predictedValue);

                // Debug: Print decoded residual
                std::cout << "Decoded Residual: " << residual << std::endl;
            } else {
                // Handle the case where the terminating '0' is not found
                std::cerr << "Error: Invalid Golomb code format." << std::endl;
                break;
            }
        }
    }





};

#endif // AUDIO_CODEC_H
