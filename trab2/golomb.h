#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <cmath>

class Golomb {
    private:
        int minBitsCount;
        int maxBitsCount;
        int valuesWithMinBitsCount;

    // Determine the minimum bit count (minBitsCount) and the count of values with the minimum bit count (valuesWithMinBitsCount)
    void analyzeBits(int m) {
        if (m != 0){
            // Calculate the maximum bit count (maxBitsCount): log2(m) rounded up to the nearest integer
            maxBitsCount = ceil(log2(m));
            // Calculate the minimum bit count (minBitsCount): maxBitsCount - 1
            minBitsCount = maxBitsCount - 1;
            // Calculate the count of values with the minimum bit count (valuesWithMinBitsCount): (2^maxBitsCount) - m
            valuesWithMinBitsCount = pow(2, maxBitsCount) - m;
        } else {
            // If m is 0, then all counts are set to 0
            maxBitsCount = 0;
            minBitsCount = 0;
            valuesWithMinBitsCount = 0;
        }
    }


    // Convert an integer to a binary string with a specified number of bits (nBitsRepresentation)
    std::string convertToBinaryString(int num, int nBitsRepresentation){
        std::string result = "";
        for (int i = 0; i < nBitsRepresentation; i++) {
            result = std::to_string(num % 2) + result;
            num /= 2;
        }

        return result;
    }


        int bitStringToInteger(std::string bitString) {
            int result = 0;
            for (size_t i = 0; i < bitString.length(); i++)
                result = result * 2 + (bitString[i] - '0');

            return result;
        }

    public:
        // Constructor
        Golomb() { }

        // Decode function for a given M value
        std::vector<int> interpretEncodedData(std::string encodedData, int m) {
            analyzeBits(m);
            std::vector<int> result;
            // Bit position in the encoded data
            int currentIndex = 0;

            // Iterate through the encoded data
            while((size_t) currentIndex < encodedData.length()) {
                int quotient = 0;
                // Count the number of 0s in the encoded data to determine the quotient (encoded in unary code)
                while (encodedData[currentIndex] == '0') {
                    quotient++;
                    // Move to the next bit
                    currentIndex++;
                }
                // Skip a bit (the 1 in the unary code, indicating the end of the quotient)
                currentIndex++;
                // Initialize the remainder 
                int remainder = 0;
                // Counter for the number of bits read from the encoded data in the remainder part (binary code)
                int bitCounter = 0;
                // Temporary string to store the remainder
                std::string tempString = "";
                
                // If m is 1, the remainder is 0
                // Otherwise, if m is 0, it's set to 1 (to prevent division by 0)
                if (m != 1){
                    // While the number of bits of the remainder doesn't reach minBitsCount
                    while (bitCounter < minBitsCount) {
                        // Add the bit to the temporary string (to obtain the remainder)
                        tempString += encodedData[currentIndex];
                        // Move to the next bit
                        currentIndex++;
                        // Move to the next bit of the remainder part (binary code)
                        bitCounter++;
                    }

                    // Convert the temporary string to an integer to obtain the remainder
                    int result1 = bitStringToInteger(tempString);

                    // If the remainder has a value greater than the number of values with minBitsCount (corresponding to the maximum value with minBitsCount)
                    // The next bit must be read as part of the remainder
                    // If the value is smaller, the remainder is the value read so far
                    if (result1 < valuesWithMinBitsCount) {
                        remainder = result1;
                    } else {
                        tempString += encodedData[currentIndex];
                        currentIndex++;
                        // Convert the temporary string to an integer to obtain the remainder
                        remainder = bitStringToInteger(tempString) - valuesWithMinBitsCount;
                    }
                } else {
                    remainder = 0;
                    currentIndex++;
                }

                // Resulting value without sign
                int decodedValue = quotient * m + remainder;

                // If the encoded data has a 1 at the end of the remainder, the result is negative; otherwise, it's positive
                if (encodedData[currentIndex] == '1') {
                    result.push_back(-(decodedValue));
                } else {
                    result.push_back(decodedValue);
                }

                currentIndex++;
            }

            return result;
        }


        // Decode function for changing M values within blocks
        std::vector<int> interpretMultipleM(std::string encodedData, std::vector<int> mValues, int blockSize) {
            std::vector<int> result;
            // Bit position in the encoded data
            int currentIndex = 0;
            // Index for the current M value in the vector
            int mIndex = 0;
            // Block size bits counter
            int bitCount = 0;
            analyzeBits(mValues[mIndex]);

            while((size_t) currentIndex < encodedData.length()) {
                int quotient = 0;
                // Count the number of 0s in the encoded data to determine the quotient (encoded in unary code)
                while (encodedData[currentIndex] == '0') {
                    quotient++;
                    // Move to the next bit
                    currentIndex++;
                }
                // Skip a bit (the 1 in the unary code, indicating the end of the quotient)
                currentIndex++;
                int remainder = 0;
                // Counter for the number of bits read from the encoded data in the remainder part (binary code)
                int bitCounter = 0;
                std::string tempString = "";

                // If the current M value is not 1, the remainder is 0;
                // Otherwise, if the M value is 0, it's set to 1 (to prevent division by 0)
                if (mValues[mIndex] != 1){
                    while (bitCounter < minBitsCount) {
                        tempString += encodedData[currentIndex];
                        // Move to the next bit
                        currentIndex++;
                        // Move to the next bit of the remainder part (binary code)
                        bitCounter++;
                    }
                    
                    // Convert the temporary string to an integer to obtain the remainder
                    int result1 = bitStringToInteger(tempString);

                    // If the remainder has a value greater than the number of values with minBitsCount (corresponding to the maximum value with minBitsCount)
                    // The next bit must be read as part of the remainder
                    // If the value is smaller, the remainder is the value read so far
                    if (result1 < valuesWithMinBitsCount) {
                        remainder = result1;
                    } else {
                        tempString += encodedData[currentIndex];
                        currentIndex++;
                        // Convert the temporary string to an integer to obtain the remainder
                        remainder = bitStringToInteger(tempString) - valuesWithMinBitsCount;
                    }
                } else {
                    remainder = 0;
                    currentIndex++;
                }

                // Resulting value without sign
                int decodedValue = quotient * mValues[mIndex] + remainder;

                // If the encoded data has a 1 at the end of the remainder, the result is negative; otherwise, it's positive
                if (encodedData[currentIndex] == '1') {
                    result.push_back(-(decodedValue));
                } else {
                    result.push_back(decodedValue);
                }

                // Move to the next bit
                currentIndex++;

                bitCount++;
                // If the block size is reached, increment the M index
                if (bitCount == blockSize) {
                    // Move to the next M index
                    mIndex++;
                    // Reset block size bits counter
                    bitCount = 0;
                    analyzeBits(mValues[mIndex]);
                }  
            }
            
            return result;
        }

    // Encode an integer using Golomb coding with a specified M value
    std::string performGolombEncoding(int number, int mValue){
        computeBits(mValue);
        // Resulting string
        std::string encodedResult = "";
        int quotient = 0;
        int remainder = 0;
        // If mValue isn't 0, calculate the quotient and the remainder with Golomb coding
        if (mValue != 0){
            quotient = abs(number) / mValue;
            remainder = abs(number) % mValue;
        }
        // Concatenate the quotient in unary code 
        for (int i = 0; i < quotient; i++) {
            encodedResult += "0";
        }

        // Use a bit (1) to represent the end of the quotient and the beginning of the remainder
        encodedResult += "1";

        // If mValue is 1, the remainder is 0; otherwise, calculate the remainder in binary code
        if (mValue != 1){
            if (remainder < valuesWithMinBitsCount) {
                encodedResult += convertToBinaryString(remainder, minBitsCount);
            } else {
                encodedResult += convertToBinaryString(remainder + valuesWithMinBitsCount, maxBitsCount);
            }
        } else {
            encodedResult += "0";
        }

        // If the number is negative, add a 1 at the end of the remainder to indicate the sign
        number < 0 ? encodedResult += "1" : encodedResult += "0";

        return encodedResult;
    }

};
