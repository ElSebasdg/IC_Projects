#include "golomb.h"

int main() {
    // Create an instance of Golomb with a specific value of m (e.g., 10)
    Golomb golombInstance(10);

    // Test encoding and decoding
    int originalNumbers[] = {7, -5, 12, -8, 0};
    int numTestCases = sizeof(originalNumbers) / sizeof(originalNumbers[0]);

    for (int i = 0; i < numTestCases; ++i) {
        int originalNumber = originalNumbers[i];

        // Encode the original number
        string golombCode = golombInstance.encodeNumber(originalNumber, 1); // Enable mapping for negative numbers

        // Decode the Golomb code
        long decodedNumber = 0;
        char* remainingBits = golombInstance.decodeString(&golombCode[0], &decodedNumber, 1);

        // Verify if the decoding is correct
        cout << "Original: " << originalNumber << " | Encoded: " << golombCode << " | Decoded: " << decodedNumber << endl;
    }

    return 0;
}
