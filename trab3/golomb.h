#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <cmath>

using namespace std;

class Golomb {
    private:
        int m;
        int b;
        int stopper;

    
        void getBits(int m) {
            if (m != 0){
                b = ceil(log2(m));
                // stopper: (2^b) - m
                stopper = pow(2, b) - m;
            } else {
                b = 0;
                stopper = 0;
            }
        }

    public:
        Golomb() { }
        
        string encode(int num, int m){

            int q = 0;
            int r = 0;

            if (m != 0){
                q = abs(num) / m;
                r = abs(num) % m;
            }
            
            string encodedString = "";
            
           // Apply Golomb encoding
            for (int i = 0; i < q; i++) {
                encodedString += "0";
            }
            // use a bit (1) to represent the end of the quotient and the begginning of the r
            encodedString += "1";
            
            //int b = ceil(log2(m));
            //int stopper = pow(2, b) - m;
            getBits(m);


            if (m != 1) {
                std::string result = "";
                if (r < stopper) {
                    int binary = r;
                    for (int i = 0; i <= b - 2; i++) {
                        //encodedString += ((binary & (1 << i)) != 0) ? '1' : '0';
                         result = to_string(binary % 2) + result;
                         binary /= 2;
                         
                    }
                    encodedString += result;
                } else {
                    int binary = r + stopper;   // r + (2^b)-m
                    for (int i = 0; i <= b - 1; i++) {
                        //encodedString += ((binary & (1 << i)) != 0) ? '1' : '0';
                        result = to_string(binary % 2) + result;
                        binary /= 2;
                    }
                    encodedString += result;
                }
            } else {
                encodedString += '0';
            }

            // if the number is negative, add a 1 at the end of the remainder to indicate the sign
            num < 0 ? encodedString += "1" : encodedString += "0";


            return encodedString;
        }


        vector<int> decode(string encodedString, vector<int> m_vector, int blockSize) {
            vector<int> decodedValues;
            int i = 0, m_i = 0, count = 0;    //current index, current m index, blockSize counter
            getBits(m_vector[m_i]);
            while(i < encodedString.length()) {
                    int q = 0;

                    while (encodedString[i] == '0') { //get the count of quotient ( unary code )
                        q++;
                        i++;    // next bit
                    }

                    i++;  //skip the '1'
                    int r = 0;
                    // /*
                  
                    string aux = "";
                    
                    if (m_vector[m_i] != 1){      // m!=1
                        
                        for (int j = 0; j < b-1; j++){  //get the count of remainder ( binary code )
                            aux += encodedString[i];
                            i++;
                        }

                        int res = 0;
                        for (int i = 0; i < aux.length(); i++)
                          res = res * 2 + (aux[i] - '0');   //convert the remainder to int


                        if (res < stopper) {
                            r = res;    // binary codeword of b-1 bits
                        } else {
                            aux += encodedString[i];
                            i++;    // next bit -> binary codeword of b bits
                            
                            int res = 0;
                            for (int i = 0; i < aux.length(); i++)
                              res = res * 2 + (aux[i] - '0'); //convert the remainder to int
                              
                            r = res - stopper;    //res = r + (2^b)-m
                        }
                    } else {                      // m = 1
                        r = 0;
                        i++;    // next bit
                    }
                    
                    // */
                    
                    /*int bitsToRead = ceil(log2(m_vector[m_i] + 1));

                    if (m_vector[m_i] != 1) {
                        std::string remainderBits = encodedString.substr(i, bitsToRead - 1);
                        int nValues = pow(2, bitsToRead - 1);

                        if (std::stoi(remainderBits, nullptr, 2) >= nValues / 2) {
                            bitsToRead++;
                            remainderBits = encodedString.substr(i, bitsToRead - 1);
                        }

                        r = std::stoi(remainderBits, nullptr, 2);
                        if (r >= nValues / 2) {
                            r += nValues / 2;
                        }
                    }

                    i += bitsToRead - 1;
                    
                    */

                    // result value without sign
                    int result = q * m_vector[m_i] + r;

                    // if encoded string has a 1 in the end of the remainder, the result is negative, otherwise it's positive
                    if (encodedString[i] == '1') {
                        result = -result;
                    } 
                    
                    decodedValues.push_back(result);
                    i++;    // next bit



                    //blockSize--;
                    //if (blockSize == 0) {
                    count++;
                    if (count == blockSize) {
                        m_i++;    // next m 
                        // reset blocksize 
                        count = 0;
                        //blockSize = blockSize;
                        getBits(m_vector[m_i]);
                    }  
                }
            return decodedValues;
        }
};
