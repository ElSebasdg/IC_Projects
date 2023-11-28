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
            // Ler código Golomb do fluxo de bits com comprimento dinâmico
            std::string golombCode = bitStream.readStringDynamicLength();

            // Verificar se o código Golomb está vazio (indicando EOF inesperado)
            if (golombCode.empty()) {
                std::cerr << "Unexpected EOF while reading Golomb code." << std::endl;
                break;
            }

            // Debug: Print código Golomb antes da descodificação
            std::cout << "Golomb Code: " << golombCode << " | Code Length: " << golombCode.length() << std::endl;

            // Descodificar o código Golomb para obter o residual de previsão            
            long residual = 0;
            char* pBits = &golombCode[0];
            pBits = golomb.decodeString(pBits, &residual, mappingOn);

            // Atualizar o preditor e adicionar a amostra decodificada aos dados de áudio
            predictedValue += residual;
            decodedAudio.push_back(predictedValue);
        }
    }

};

#endif // AUDIO_CODEC_H
