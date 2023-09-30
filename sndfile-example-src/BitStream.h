#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

class BitStream {
private:
    std::fstream file;
    uint8_t buffer;         //byte que está a ser lido
    uint8_t bufferLength;   //nº de bits no buffer
    bool eofFlag;           // Flag to track end of file

    // Lê o próximo byte do ficheiro
    void readByte() {
        if (file.is_open() && !file.eof()) {
            file.get(reinterpret_cast<char&>(buffer));
            bufferLength = 8;
        } else {
            eofFlag = true; // falg a 1 no EOF
        }
    }

public:
    BitStream(const std::string& filename, bool writeMode) : eofFlag(false) {
        if (writeMode) {    // writeMode = 1 -> escreve
            file.open(filename, std::ios::out | std::ios::binary);
            buffer = 0;
            bufferLength = 0;
        } else {            // writeMode = 0 -> lê
            file.open(filename, std::ios::in | std::ios::binary);
            buffer = 0;
            bufferLength = 0;
            readByte(); // Initialize buffer and eofFlag when opening for reading
        }
    }

    ~BitStream() {
        close();
    }

    // Escreve apenas um bit (0 ou 1)
    void writeBit(bool bit) {
        buffer |= (bit << (7 - bufferLength));
        bufferLength++;

        if (bufferLength == 8) {
            file.put(buffer);
            buffer = 0;
            bufferLength = 0;
        }
    }

    // Lê apenas um bit (0 ou 1)
    bool readBit() {
        if (bufferLength == 0 && !eofFlag) {
            readByte();
        }

        bool bit = (buffer >> (bufferLength - 1)) & 1; //do + significativo para o -
        bufferLength--;
        return bit;
    }

    // Escreve N bits (0 <= N <= 64)
    void writeNBits(uint64_t data, uint8_t numBits) {
        if (numBits <= 64) {
            for (int i = numBits - 1; i >= 0; i--) {
                writeBit((data >> i) & 1);  //Escreve um bit de cada vez do mais significativo para o menos
            }
        } else {
            std::cerr << "Numero de bits para escrever excede 64" << std::endl;
        }
    }

    // Lê N bits (0 <= N <= 64)
    uint64_t readNBits(uint8_t numBits) {
        if (numBits <= 64) {
            uint64_t result = 0;
            for (int i = numBits - 1; i >= 0; i--) {
                result = (result << 1) | readBit(); //Lê um bit de cada vez do mais significativo para o menos
            }
            return result;
        } else {
            std::cerr << "Numero de bits para ler excede 64" << std::endl;
            return 0;
        }
    }

    // Escreve um String
    void writeString(const std::string& str) {
        for (char c : str) {
            writeNBits(c, 8);
        }
    }

    // Lê uma String
    std::string readString(size_t length) {
        std::string result;
        for (size_t i = 0; i < length; i++) {
            char c = static_cast<char>(readNBits(8));
            result += c;
        }
        return result;
    }

    // Fecha o ficheiro
    void close() {
        if (file.is_open()) {
            if (bufferLength > 0) {
                file.put(buffer);
                buffer = 0;
                bufferLength = 0;
            }
            file.close();
        }
    }

    // chekckar se foiu aberto
    bool is_open() {
        return file.is_open();
    }

    // EOF
    bool eof() {
        return eofFlag;
    }
};
