#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "BitStream.h" 
#include <sndfile.h>



class AudioCodec {
private:
    BitStream& bitStream;
    const double quantizationFactor;
    const int blockSize;

public:
    AudioCodec(BitStream& stream, double quantizationFactor, int blockSize)
    : bitStream(stream), quantizationFactor(quantizationFactor), blockSize(blockSize) {}


    // Função para realizar a Transformada Cosseno Discreta (DCT) num bloco de áudio
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


    // Função para quantizar os coeficientes da DCT
    void Quantize(std::vector<double>& block) {
        for (double& coefficient : block) {
            coefficient = std::round(coefficient / quantizationFactor);
        }
    }

    // Função para codificar áudio e escrever no BitStream
    void EncodeAudio(const std::vector<double>& audioBlock) {
        std::vector<double> transformedBlock = audioBlock;
        DCT(transformedBlock);
        Quantize(transformedBlock);

    // Escrever coeficientes DCT quantizados no BitStream
        for (double coefficient : transformedBlock) {
            bitStream.writeNBits(static_cast<uint64_t>(coefficient), 16); // Nº bits ajustável
        }
    }

    // Função para realizar a DCT inversa num bloco de áudio
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

    // Função para descodificar áudio a partir do BitStream
    void DecodeAudio(std::vector<double>& audioBlock) {
        std::vector<double> transformedBlock;
    // Lê coeficientes quantizados DCT do BitStream
        for (int i = 0; i < blockSize; ++i) {
            uint64_t quantizedCoefficient = bitStream.readNBits(16); // Nº bits ajustável
            transformedBlock.push_back(quantizedCoefficient);
        }

        // Dequantize
        for (double& coefficient : transformedBlock) {
            coefficient *= quantizationFactor;
        }

        // Transformação inversa da DCT
        IDCT(transformedBlock);

        audioBlock = transformedBlock;
    }

bool EndOfInputAudio(std::ifstream& inputFile) {
    // Verifica se chegou ao fim do ficheiro de áudio
    return inputFile.eof();
}

void SaveWavFile(const std::string& fileName, const std::vector<double>& audioData) {
    SF_INFO sfinfo;
    sfinfo.channels = 1; // Mono ou 2 pra Stereo
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



int main() {
    
    double quantizationFactor = 10.0;
    std::string inputFileName = "input_audio.bin";
    std::string outputFileName = "output_audio.wav"; 

    std::ifstream inputFile(inputFileName, std::ios::binary);
    BitStream bitStream(inputFileName, true);
    AudioCodec encoder(bitStream, quantizationFactor, blockSize);

    // Codificar áudio em blocos
    while (!EndOfInputAudio(inputFile)) {
        std::vector<double> audioBlock; // Carregar um bloco de áudio
        // TODO:
        // Ler amostras de áudio para audioBlock
        //Atualizar audioBlock e a condição de fim
        encoder.EncodeAudio(audioBlock);
    }

    bitStream.close(); // Fechar o BItStream após a codificação

    // Decoding
    BitStream readStream(inputFileName, false); // Gera novo BitStream pra leitura
    AudioCodec decoder(readStream, quantizationFactor, blockSize);

    std::vector<double> decodedAudio;
    while (!readStream.eof()) {
        std::vector<double> audioBlock;
        decoder.DecodeAudio(audioBlock);
        decodedAudio.insert(decodedAudio.end(), audioBlock.begin(), audioBlock.end());
    }

    // Guarda o audio descodificado pra um WAV 
    SaveWavFile(outputFileName, decodedAudio);

    return 0;
};
};