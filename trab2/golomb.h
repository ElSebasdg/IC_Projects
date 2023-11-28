#ifndef GOLOMB_H
#define GOLOMB_H

#include <iostream>
#include <cmath>
#include <bitset>

using namespace std;

class Golomb {
private:
    uint32_t mEncode, mDecode;

    // Retorna a representação unária de um número natural
    string encodeUnary(uint32_t n) {
        uint32_t q = n / mEncode;
        string unary = "";

        for (uint32_t i = 0; i < q; i++) {
            unary += "1";
        }
        unary += "0";
        return unary;
    }

    // Retorna a representação binária da parte decimal de um número natural
    string encodeDecimal(uint32_t n) {
        uint32_t r = n % mEncode;
        uint32_t b = floor(log2(mEncode));
        string binary;

        if (r < (pow(2, b + 1) - mEncode)) {
            bitset<32> binR(r);
            binary = binR.to_string().substr(32 - b);
        } else {
            r = r + pow(2, b + 1) - mEncode;
            bitset<32> binR(r);
            binary = binR.to_string().substr(32 - (b + 1));
        }

        return binary;
    }

public:
    // Construtor que recebe o valor de m para codificação e descodificação
    Golomb(uint32_t m) : mEncode(m), mDecode(m) {}

    // Construtor padrão com valores de m predefinidos
    Golomb() : mEncode(1000), mDecode(1000) {}

    // Descodifica uma string binária para um número decimal
    // O parâmetro mappingOn indica se deve ser aplicado mapeamento para números negativos
    char *decodeString(char *pBits, long *resultN, int mappingOn) {
        uint32_t count = 0;
        uint32_t index = 0;

        // Read the unary part
        while (pBits[index] != '0' && pBits[index] != '\0') {
            count++;
            index++;
        }

        uint32_t b = floor(log2(mDecode));
        string key = "";

        // Read the binary part
        for (uint32_t i = 0; i < b; i++) {
            index++;
            key += pBits[index];
        }

        if (!key.empty()) {
            uint32_t decimal = static_cast<uint32_t>(bitset<32>(key).to_ulong());

            if (!(decimal < (pow(2, (b + 1)) - mDecode))) {
                index++;
                key += pBits[index];
                decimal = static_cast<uint32_t>(bitset<32>(key).to_ulong());
                decimal = decimal - pow(2, (b + 1)) + mDecode;
            }

            uint32_t result = count * mDecode + decimal;
            *resultN = static_cast<long>(result);

            // Apply the mapping for negative numbers, if necessary
            if (mappingOn) {
                if (result % 2 == 0) {
                    *resultN = static_cast<long>(*resultN / -2);
                } else {
                    *resultN = static_cast<long>((*resultN - 1) / 2);
                }
            }
        } else {
            // Handle the case where the key is empty
            *resultN = 0;
        }

        pBits += (index + 1);
        return pBits;
    }

    // Codifica um número decimal para uma string no formato Golomb
    // O parâmetro mappingOn indica se deve ser aplicado mapeamento para números negativos
    string encodeNumber(int n, int mappingOn) {
            uint32_t mapped = (mappingOn && n < 0) ? -2 * n : (2 * n) + 1;
            string unary = encodeUnary(mapped);
            string binary = encodeDecimal(mapped);

            return unary + binary;
        }




    // Retorna o valor de m para codificação
    uint32_t getEncodeParameter() {
        return this->mEncode;
    }

    // Altera o valor de m para codificação
    void setEncodeParameter(uint32_t m) {
        this->mEncode = m;
    }

    // Retorna o valor de m para descodificação
    uint32_t getDecodeParameter() {
        return this->mDecode;
    }

    // Altera o valor de m para descodificação
    void setDecodeParameter(uint32_t m) {
        this->mDecode = m;
    }

};

#endif
