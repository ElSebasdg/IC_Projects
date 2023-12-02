#include "new_golomb.h"
#include "BitStream.h"
#include <iostream>
#include <string>
#include <sndfile.hh>


using namespace std;

int main( int argc, char** argv ) {

    auto predict = [](int a, int b, int c) {    //x(n) = 3*x(n-1) - 3*x(n-2) + x(n-3)
        return 3*a - 3*b + c;
    };

    //start a timer
    clock_t start = clock();

    if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
    }

    int sampleRate = 44100;
    BitStream bs (argv[1], 0);
    
    int nChannels = bs.readNBits(16);
    int nFrames = bs.readNBits(32);
    int blockSize = bs.readNBits(16);
    int enc = bs.readNBits(32);
    int m_size = bs.readNBits(16);
    
    vector<int> m_vector;
    for(int i = 0; i < m_size; i++) {
        int m = bs.readNBits(16); // Read 16 bits for m

        m_vector.push_back(m);
    }

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, nChannels, sampleRate };
    
    
    vector<int> encoded_values;
    for (int i = 0; i < enc; i++) {
        encoded_values.push_back(bs.readBit());
    }
    
    string encodedString = "";
    for( int  i = 0; i < encoded_values.size(); i++) {
        encodedString += to_string(encoded_values[i]);
    }


    Golomb g;
      
    vector<int> decoded = g.decode(encodedString, m_vector, blockSize);

    vector<short> samplesVector;

    if (nChannels < 2) {   //mono 
        for ( int i = 0; i < decoded.size(); i++) {
            if (i >= 3) {
                //create a new sample, using the 3 previous samples
                int sample = decoded[i] + predict(samplesVector[i-1], samplesVector[i-2], samplesVector[i-3]);
                samplesVector.push_back(sample);
            }
            //create a new sample with the decoded value at the current sample index 
            else samplesVector.push_back(decoded[i]);
        }
    } else {    // stereo
        for (int i = 0; i < nFrames; i++) {   //left channel
            if (i >= 3) {
                //create a new sample, using the 3 previous samples
                int sample = decoded[i] + predict(samplesVector[i-1], samplesVector[i-2], samplesVector[i-3]);
                samplesVector.push_back(sample);
            }
            //create a new sample with the decoded value at the current sample index 
            else samplesVector.push_back(decoded[i]); 
        }

        for ( int i = nFrames; i < decoded.size(); i++) {   //right channel
            if (i >= nFrames + 3) {
                //create a new sample, using the 3 previous samples
                int sample = decoded[i] + predict(samplesVector[i-1], samplesVector[i-2], samplesVector[i-3]);
                samplesVector.push_back(sample);
            }
            //create a new sample with the decoded value at the current sample index 
            else samplesVector.push_back(decoded[i]); 
        }


        vector<short> merged;
        vector<short> firstChannel(samplesVector.begin(), samplesVector.begin() + nFrames); // first nFrames samples -> left channel
        vector<short> secondChannel(samplesVector.begin() + nFrames, samplesVector.end()); //second nFrames samples -> right channel
        for(int i = 0; i < nFrames; i++) {
            merged.push_back(firstChannel[i]);
            merged.push_back(secondChannel[i]);
        }
        samplesVector = merged;
    }


    //write to wav file
    sfhOut.write(samplesVector.data(), samplesVector.size());
    bs.close();

    //end timer
    clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    elapsed_secs = elapsed_secs * 1000;
    cout << "Time: " << elapsed_secs << " ms" << endl;

    return 0;
}
