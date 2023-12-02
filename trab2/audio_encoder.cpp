#include "new_golomb.h"
#include "BitStream.h"
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fftw3.h>
#include <sndfile.hh>


using namespace std;

int main(int argc, char *argv[]) {

    //function to calculate m based on p
    auto calc_m = [](int p) {
        //p = alpha / 1 - alpha
        //m = - (1 / log(alpha))
        return (int) - (1/log((double) p / (1 + p)));
    };

    auto predict = [](int a, int b, int c) {    //x(n) = 3*x(n-1) - 3*x(n-2) + x(n-3)
        return 3*a - 3*b + c;
    };
    
    size_t bs{1024};
    int br{0};

    if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		cerr << "               [ -bs blockSize (def 1024) ]\n";
                cerr << "               [ -br bitrate (def x) ]\n";
		return 1;
	}


    SndfileHandle sfhIn { argv[1] };
    string output = argv[2];
    
    if(sfhIn.error()) {
	cerr << "Error: invalid input file\n";
	return 1;
    }
    if((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
	cerr << "Error: file is not in WAV format\n";
	return 1;
    }
    if((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
	    cerr << "Error: file is not in PCM_16 format\n";
	    return 1;
    }
    
    for (int n = 1; n < argc; n++)
        if (string(argv[n]) == "-bs") {
            bs = atoi(argv[n + 1]);
            break;
        }

    for (int n = 1; n < argc; n++)
        if (string(argv[n]) == "-br") {
            br = atof(argv[n + 1]);
            break;
        }
	

    
    

    //start a timer
    clock_t start = clock();
    

    size_t nFrames { static_cast<size_t>(sfhIn.frames()) };
    
    // Ensure nFrames is a multiple of bs
    nFrames = (nFrames / bs) * bs;

    if (nFrames < bs) {
        cerr << "Error: Input file is too short for processing\n";
        return 1;
    }
    
    size_t nChannels { static_cast<size_t>(sfhIn.channels()) };
    vector<short> samples(nChannels * nFrames);
    sfhIn.readf(samples.data(), nFrames);
    size_t nBlocks{static_cast<size_t>((nFrames) / bs)};

    // Do zero padding, if necessary
    samples.resize(nBlocks * bs * nChannels);

    vector<short> left(samples.size() / 2);
    vector<short> right(samples.size() / 2);
    

    if (nChannels > 1) {    //if stereo
        //get samples of each channel
        for (int i = 0; i < samples.size()/2; i++) {
            left[i] = samples[i * nChannels];
            right[i] = samples[i * nChannels + 1];
        }
    }

    vector<int> m_vector;
    vector<int> valuesToBeEncoded;
    
    if (nChannels < 2) {  //mono audio
        for(int i = 0; i < samples.size(); i++) {
            if (i >= 3) {
                int error = samples[i] - predict(samples[i-1], samples[i-2], samples[i-3]);    //x(n) = 3*x(n-1) - 3*x(n-2) + x(n-3)
                valuesToBeEncoded.push_back(error);
            } else {
                valuesToBeEncoded.push_back(samples[i]);
            }
        }
    } else {    //stereo audio
        std::vector<std::vector<short>> channels = {left, right}; // Store both channels
        cout << "Number of channels: " << channels.size() << endl;
        for (int channel = 0; channel < channels.size(); ++channel) {   // first for one channel, then for the other
            for (int i = 0; i < channels[channel].size(); ++i) {
                if (i >= 3) {
                    int error = channels[channel][i] - predict(channels[channel][i-1], channels[channel][i-2], channels[channel][i-3]);   //x(n) = 3*x(n-1) - 3*x(n-2) + x(n-3)
                    valuesToBeEncoded.push_back(error);
                } else {
                    valuesToBeEncoded.push_back(channels[channel][i]);
                }

                // Calculate 'm' every bs samples
                if (i % bs == 0 && i != 0) {
                    int sum = 0;
                    for (int j = i - bs; j < i; ++j) {
                        sum += abs(valuesToBeEncoded[j]);
                    }
                    int p = round(sum / bs);
                    int m = calc_m(p);
                    if (m < 1) m = 1;
                    m_vector.push_back(m);
                }
                if (i == channels[channel].size() - 1) {
                    int sum = 0;
                    for (int j = i - (i % bs); j < i; ++j) {
                        sum += abs(valuesToBeEncoded[j]);
                    }
                    int p = round(sum / (i % bs));
                    int m = calc_m(p);
                    if (m < 1) m = 1;
                    m_vector.push_back(m);
                }
            }
        }
    }

    string encodedString = "";
    Golomb g;

    
    int m_index = 0;
    for (int i = 0; i < valuesToBeEncoded.size(); i++) {  
        if (i % bs == 0 && i != 0) m_index++;
        encodedString += g.encode(valuesToBeEncoded[i], m_vector[m_index]);    
    }
    

    BitStream bitStream(output, 1);

    bitStream.writeNBits(sfhIn.channels(), 16); // Write the number of channels (16 bits)
    bitStream.writeNBits(samples.size()/2, 32); // Write the number of frames (32 bits)
    bitStream.writeNBits(bs, 16); // Write the block size (16 bits)
    bitStream.writeNBits(encodedString.size(), 32); // Write encodedString size (32 bits)    
    bitStream.writeNBits(m_vector.size(), 16); // Write m_vector size (16 bits)


    // Write m_vector values converted to binary (16 bits each) 
    for (int i = 0; i < m_vector.size(); i++) {
        bitStream.writeNBits(m_vector[i], 16);
    }
    
    vector<int> encoded_bits;
    
    //the next bits will be the encoded string
    for(int i = 0; i < encodedString.length(); i++)
        encoded_bits.push_back(encodedString[i] - '0');
    
    
    //the next bits are the encoded bits
    for (int i = 0; i < encoded_bits.size(); i++)
        bitStream.writeBit(encoded_bits[i]);

    
    bitStream.close();

    //end the timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time: " << elapsed_secs << " ms" << endl;

    return 0;
}
