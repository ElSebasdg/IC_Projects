#include "golomb.h"
#include <cassert>

int main() {
    // Test 1: Fixed M value decoding
    {
        Golomb golomb;
        int mValue = 5;
        int testNumber = 23;

        std::string encodedString = golomb.performGolombEncoding(testNumber, mValue);
        std::vector<int> decodedValues = golomb.interpretEncodedData(encodedString, mValue);

        assert(decodedValues.size() == 1);
        assert(decodedValues[0] == testNumber);
    }

    // Test 2: Dynamic M values decoding
    {
        Golomb golomb;
        std::vector<int> mValues = {3, 5, 2, 4};
        int blockSize = 8;
        int testNumbers[] = {12, -7, 5, -20, 8, 15, -4, 10};

        std::string encodedString = "";
        for (int i = 0; i < 8; ++i) {
            encodedString += golomb.performGolombEncoding(testNumbers[i], mValues[i % mValues.size()]);
        }

        std::vector<int> decodedValues = golomb.interpretMultipleM(encodedString, mValues, blockSize);

        assert(decodedValues.size() == 8);
        for (int i = 0; i < 8; ++i) {
            assert(decodedValues[i] == testNumbers[i]);
        }
    }

    // Test 3: Edge case with M = 0
    {
        Golomb golomb;
        int mValue = 0;
        int testNumber = 42;

        std::string encodedString = golomb.performGolombEncoding(testNumber, mValue);
        std::vector<int> decodedValues = golomb.interpretEncodedData(encodedString, mValue);

        assert(decodedValues.size() == 1);
        assert(decodedValues[0] == 0);
    }

    // Test 4: Decode multiple values with varying M values
    {
        Golomb golomb;
        std::vector<int> mValues = {2, 3, 4, 5, 6};
        int testNumbers[] = {8, -12, 15, 7, -6, 25, -30, 18};
        std::string encodedString = "";

        for (int i = 0; i < 8; ++i) {
            encodedString += golomb.performGolombEncoding(testNumbers[i], mValues[i % mValues.size()]);
        }

        std::vector<int> decodedValues = golomb.interpretMultipleM(encodedString, mValues, 4);

        assert(decodedValues.size() == 8);
        for (int i = 0; i < 8; ++i) {
            assert(decodedValues[i] == testNumbers[i]);
        }
    }

    // Test 5: Encode and decode a large set of numbers with random M values
    {
        Golomb golomb;
        int testNumbers[] = {-500, 1000, -750, 300, -200, 800, -900, 1200, -600, 450};
        std::vector<int> mValues = {3, 5, 2, 4, 6, 8, 10, 15, 20, 12};
        std::string encodedString = "";

        for (int i = 0; i < 10; ++i) {
            encodedString += golomb.performGolombEncoding(testNumbers[i], mValues[i]);
        }

        std::vector<int> decodedValues = golomb.interpretMultipleM(encodedString, mValues, 2);

        assert(decodedValues.size() == 10);
        for (int i = 0; i < 10; ++i) {
            assert(decodedValues[i] == testNumbers[i]);
        }
    }

    std::cout << "All tests passed successfully!" << std::endl;

    return 0;
}
