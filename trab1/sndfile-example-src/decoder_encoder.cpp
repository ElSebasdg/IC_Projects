#include <iostream>
#include <fstream>
#include "BitStream.h" // Include your BitStream class header here


// encoder should be able to convert a text file containing only 0s and 1s into the binary equivalent
void encoder(const std::string& inputFileName, const std::string& outputFileName) {
    std::ifstream inputFile(inputFileName, std::ios::in);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }

    BitStream outputBitStream(outputFileName, true);

    char bit;
    while (inputFile.get(bit)) {
        if (bit != '0' && bit != '1') {
            std::cerr << "Input file should contain only 0s and 1s." << std::endl;
            return;
        }

        outputBitStream.writeBit(bit == '1');
    }

    inputFile.close();
    outputBitStream.close();
}


// converte um ficheiro de texto no seu equivalente em 0s e 1s
void decoder(const std::string& inputFileName, const std::string& outputFileName) {

    BitStream inputBitStream(inputFileName, false);
    if (!inputBitStream.is_open()) {
        std::cerr << "Error opening the input file." << std::endl;
        return;
    }

    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open()) {
        std::cerr << "Error creating the output file." << std::endl;
        return;
    }

    // escrever no ficheiro destino
    while (!inputBitStream.eof()) {
        bool bit = inputBitStream.readBit();
        if (inputBitStream.eof()) break;
        
        if (bit) {
            outputFile << '1';
        } else {
            outputFile << '0';
        }
    }

    inputBitStream.close();
    outputFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " encoder|decoder inputFileName outputFileName" << std::endl;
        return 1;
    }

    const std::string action = argv[1];
    const std::string inputFileName = argv[2];
    const std::string outputFileName = argv[3];

    if (action == "encoder") {
        encoder(inputFileName, outputFileName);
        std::cout << "Encoding completed." << std::endl;
    } else if (action == "decoder") {
        decoder(inputFileName, outputFileName);
        std::cout << "Decoding completed." << std::endl;
    } else {
        std::cerr << "Invalid action. Use 'encoder' or 'decoder'." << std::endl;
        return 1;
    }

    return 0;
}