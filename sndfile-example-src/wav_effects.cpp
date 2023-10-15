#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;

// Function to display the menu and get the user's choice
int showMenu() {
    int choice;
    cout << "Select an effect:" << endl;
    cout << "1. Single Echo" << endl;
    cout << "2. Multiple Echo" << endl;
    cout << "3. Amplitude Modulation" << endl;
    cout << "4. Time Varying Delays" << endl;
    cout << "5. Slow Motion" << endl;
    cout << "6. Fast Forward" << endl;
    cout << "Enter your choice (1-6): ";
    cin >> choice;
    return choice;
}

// Aplica um único eco
void applySingleEcho(SndfileHandle& sfhIn, SndfileHandle& sfhOut, int delay, float gain) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> echoBuffer;

    while (size_t nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        for (int i = 0; i < (int)samples.size(); i++) {
            if (i >= delay) {   //y(n) = x(n) + a * x(n - k)
                echoBuffer.push_back((samples.at(i) + gain * samples.at(i - delay)) / (1 + gain));
            } else {
                echoBuffer.push_back(samples.at(i));
            }
        }
    }

    sfhOut.writef(echoBuffer.data(), echoBuffer.size() / sfhIn.channels());
}

// Aplica ecos múltiplos
void applyMultipleEchoes(SndfileHandle& sfhIn, SndfileHandle& sfhOut, int delay, float gain) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> echoBuffer;

    while (size_t nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        for (int i = 0; i < (int)samples.size(); i++) {
            if (i >= delay) {   // y(n) = x(n) + a * y(n - k)
                echoBuffer.push_back((samples.at(i) + gain * echoBuffer.at(i - delay)) / (1 + gain));
            } else {
                echoBuffer.push_back(samples.at(i));
            }
        }
    }

    sfhOut.writef(echoBuffer.data(), echoBuffer.size() / sfhIn.channels());
}

    

// Aplica o efeito de modulação de amplitude
void applyAmplitudeModulation(SndfileHandle& sfhIn, SndfileHandle& sfhOut, float freq) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> outBuffer;

    while (size_t nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        for (int i = 0; i < (int)samples.size(); i++) {
            // y(n) = x(n) * cos(2*pi * (f/fa) * n)
            outBuffer.push_back(samples.at(i) * cos(2 * M_PI * (freq / sfhIn.samplerate()) * i));
        }
    }

    sfhOut.writef(outBuffer.data(), outBuffer.size() / sfhIn.channels());
}

// Aplica o efeito de atrasos variáveis 
void applyTimeVaryingDelays(SndfileHandle& sfhIn, SndfileHandle& sfhOut, vector<double> delayTimesSeconds, double sampleRate) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    size_t nFrames;

    while (nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        // Atrasos Variáveis 
        vector<vector<short>> delayedSamples(delayTimesSeconds.size(), vector<short>(nFrames, 0));

        for (size_t i = 0; i < delayTimesSeconds.size(); ++i) {
            size_t delaySamples = static_cast<size_t>(delayTimesSeconds[i] * sampleRate);

            for (size_t j = delaySamples; j < nFrames; ++j) {
                delayedSamples[i][j] = samples[j - delaySamples];
            }
        }

        // Misturar as amostras com atraso
        for (size_t i = 0; i < nFrames; ++i) {
            for (size_t j = 0; j < delayTimesSeconds.size(); ++j) {
                samples[i] += delayedSamples[j][i];
            }
        }

        sfhOut.writef(samples.data(), nFrames);
    }    
}

// Aplica o efeito de Slow motion 
void applySlowMotion(SndfileHandle& sfhIn, SndfileHandle& sfhOut, double slowdownFactor) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> slowedSamples;

    size_t nFrames;

    while (nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        // Reduz a taxa de amostragem
        for (size_t i = 0; i < nFrames; ++i) {
            slowedSamples.push_back(samples[i]);

        // Adicionar amostras extras para prolongar o áudio
            for (int j = 1; j < slowdownFactor; ++j) {
                short interpolatedSample = (samples[i] + samples[i + 1]) / 2;
                slowedSamples.push_back(interpolatedSample);
            }
        }
    }

    sfhOut.writef(slowedSamples.data(), slowedSamples.size() / sfhIn.channels());
}

// Acelera 
void applyFastForward(SndfileHandle& sfhIn, SndfileHandle& sfhOut, double speedupFactor) {
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    vector<short> spedUpSamples;

    size_t nFrames;
    while (nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        samples.resize(nFrames * sfhIn.channels());

        // Salta amostras com base no fator de aceleração
        for (size_t i = 0; i < nFrames; i += speedupFactor) {
            spedUpSamples.push_back(samples[i]);
        }
    }


    sfhOut.writef(spedUpSamples.data(), spedUpSamples.size() / sfhIn.channels());
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output_file>\n";
        return 1;
    }

    SndfileHandle sfhIn{argv[1]}; // Open the input file
    if (sfhIn.error()) {
        cerr << "Error: invalid input file\n";
        return 1;
    }

    SndfileHandle sfhOut{argv[2], SFM_WRITE, sfhIn.format(), sfhIn.channels(), sfhIn.samplerate()};
    if (sfhOut.error()) {
        cerr << "Error: invalid output file\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: File is not in WAV format\n";
        return 1;
    }

    if ((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: File is not in PCM_16 format\n";
        return 1;
    }

    vector<double> delayTimesSeconds = {0.2, 0.5, 0.8};

    int effectType = showMenu();

    if (effectType == 1 || effectType == 2) {
        int delay;
        float gain;

        try {
            cout << "Enter the delay (e.g., 44100): ";
            cin >> delay;
            cout << "Enter the gain (e.g., 0.8): ";
            cin >> gain;
        } catch (exception &err) {
            cerr << "Error: invalid gain or delay\n";
            return 1;
        }

        if (effectType == 1) {
            applySingleEcho(sfhIn, sfhOut, delay, gain);
        } else {
            applyMultipleEchoes(sfhIn, sfhOut, delay, gain);
        }
    } else if (effectType == 3) {
        float freq;

        try {
            cout << "Enter the frequency (e.g., 1Hz): ";
            cin >> freq;
        } catch (exception &err) {
            cerr << "Error: invalid frequency\n";
            return 1;
        }

        applyAmplitudeModulation(sfhIn, sfhOut, freq);
    } else if (effectType == 4) {
        applyTimeVaryingDelays(sfhIn, sfhOut, {0.2, 0.5, 0.8}, sfhIn.samplerate());
    } else if (effectType == 5) {
        applySlowMotion(sfhIn, sfhOut, 2.0);
    } else if (effectType == 6) {
        applyFastForward(sfhIn, sfhOut, 2.0);
    }
    cout << "Effect applied successfully.\n";

    return 0;
}
