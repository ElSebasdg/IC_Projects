#include <iostream>
#include <bitset>
#include "BitStream.h" 

int main() {

    // Escreve os 2 bytes (A7 and C0) para o ficheiro test
    BitStream writer("test.bin", true);
    writer.writeNBits(0xA7, 8);
    writer.writeNBits(0xC0, 8);
    writer.close();

    // Lê os primeiros 11 bits do ficheiro para verificar
    BitStream reader("test.bin", false);
    uint64_t bits = reader.readNBits(11);
    reader.close();

    // resultado esperado
    std::bitset<11> expected("10100111110");

    std::cout << "Read bits: " << std::bitset<11>(bits) << std::endl;
    std::cout << "Expected bits: " << expected << std::endl;

    if (bits == expected.to_ullong()) {
        std::cout << "Test passed! The first 11 bits match the expected value." << std::endl;
    } else {
        std::cout << "Test failed! The first 11 bits do not match the expected value." << std::endl;
    }

    return 0;
}