#include <vector>
#include <cmath>

class Golomb {
private:
    unsigned int m;  // divisor
    unsigned int b;  // binary encoding of the remainder r.

public:
    Golomb(unsigned int m) : m(m) {
        b = ceil(log2(m));
    }

    std::vector<bool> encode(int number) {
        std::vector<bool> bits;
        unsigned int q, r;
        bool sign = false;


        // Check if the number is negative, and if so, set the sign flag and make the number positive
        if (number < 0) {
            number = -number;
            sign = true;
        }


        // Calculate quotient (q)
        q = number / m;
        // Calculate remainder (r)
        r = number % m;


        // Append q true values to the bits vector, followed by a false value
        for (unsigned int i = 0; i < q; i++) {
            bits.push_back(true);
        }
        bits.push_back(false);


        // Append the binary representation of r using b bits to the bits vector
        for (unsigned int i = 0; i < b; i++) {
            bits.push_back((r >> (b - i - 1)) & 1);
        }

        // Append the sign bit to the bits vector
        if (sign) {
            bits.push_back(true);
        } else {
            bits.push_back(false);
        }

        return bits;
    }
    
int decode(const std::vector<bool>& bits, unsigned int& index) {
    unsigned int q = 0;
    unsigned int i = index;

    // Count the number of consecutive true values, which represents the quotient q
    while (bits[i]) {
        q++;
        i++;
    }

    // Extract the next b bits to represent the remainder r
    unsigned int r = 0;
    for (unsigned int j = 0; j < b; j++) {
        r = (r << 1) | bits[i + j];
    }

    // Update the index parameter to the new position
    index = i + b;

    // Calculate the original number
    int number = q * m + r;

    // Adjust the sign based on the last bit in the input vector
    if (bits[index - 1]) {
        number = -number;
    }

    return number;
}

};
