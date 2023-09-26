#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;

// Aplica um único eco
void applySingleEcho(vector<short>& samples, vector<short>& echoBuffer, size_t nFrames, double echoDelaySamples) {
    for (size_t i = echoBuffer.size(); i < nFrames; ++i) {
        short originalSample = samples[i];
        samples[i] += echoBuffer[i - echoBuffer.size()];
        echoBuffer.push_back(originalSample);
    }
}

// Aplica ecos múltiplos
void applyMultipleEchoes(vector<short>& samples, size_t nFrames, vector<double> echoDelaysSamples) {
    vector<vector<short>> echoBuffers(echoDelaysSamples.size(), vector<short>(nFrames, 0));

    // Efeitos de Ecos Múltiplos
    for (size_t i = 0; i < echoDelaysSamples.size(); ++i) {
        double echoDelaySamples = echoDelaysSamples[i];
        for (size_t j = echoDelaySamples; j < nFrames; ++j) {
            short originalSample = samples[j];
            samples[j] += echoBuffers[i][j - static_cast<size_t>(echoDelaySamples)];
            echoBuffers[i][j] = originalSample;
        }
    }
}

// Aplica o efeito de modulação de amplitude
void applyAmplitudeModulation(vector<short>& samples, size_t nFrames, double modulationFrequency, double sampleRate) {
    double angularFrequency = 2.0 * M_PI * modulationFrequency / sampleRate;

    for (size_t i = 0; i < nFrames; ++i) {
        double modulation = sin(angularFrequency * i);
        samples[i] = static_cast<short>(samples[i] * modulation);
    }
}

// Aplica o efeito de atrasos variáveis 
void applyTimeVaryingDelays(vector<short>& samples, size_t nFrames, vector<double> delayTimesSeconds, double sampleRate) {
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
}

// Aplica o efeito de Slow motion 
void applySlowMotion(vector<short>& samples, size_t nFrames, double slowdownFactor) {
    vector<short> slowedSamples;

    // Reduz a taxa de amostragem
     for (size_t i = 0; i < nFrames; ++i) {
        slowedSamples.push_back(samples[i]);

    // Adicionar amostras extras para prolongar o áudio
        for (int j = 1; j < slowdownFactor; ++j) {
            short interpolatedSample = (samples[i] + samples[i + 1]) / 2;
            slowedSamples.push_back(interpolatedSample);
        }
    }

    // Substitui as amostras originais pelas amostras em slow motion
     samples = slowedSamples;
}

// Acelera 
void applyFastForward(vector<short>& samples, size_t nFrames, double speedupFactor) {
    vector<short> spedUpSamples;

    // Salta amostras com base no fator de aceleração
    for (size_t i = 0; i < nFrames; i += speedupFactor) {
        spedUpSamples.push_back(samples[i]);
    }

    // Substitui as amostras originais pelas amostras aceleradas
    samples = spedUpSamples;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: wav_effects <inputFile> <outputFile> <effectType>\n";
        cerr << "Effect Types:\n";
        cerr << "1 - Single Echo\n";
        cerr << "2 - Multiple Echoes\n";
        cerr << "3 - Amplitude Modulation\n";
        cerr << "4 - Time-Varying Delays\n";
        cerr << "5 - Slow Motion\n";
        cerr << "6 - Fast Forward\n";
        return 1;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    int effectType = atoi(argv[3]);

    SndfileHandle sfh(inputFile);

    if (sfh.error()) {
        cerr << "Error: Invalid input file\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: File is not in WAV format\n";
        return 1;
    }

    if ((sfh.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: File is not in PCM_16 format\n";
        return 1;
    }

    SndfileHandle sfhOut(outputFile, SFM_WRITE, sfh.format(), sfh.channels(), sfh.samplerate());

    vector<short> samples(FRAMES_BUFFER_SIZE * sfh.channels());

    vector<short> echoBuffer(sfh.samplerate() * 2, 0);

    vector<double> echoDelaysSeconds = {0.2, 0.4, 0.6};

    double modulationFrequency = 5.0;

    vector<double> delayTimesSeconds = {0.2, 0.5, 0.8};

    while (size_t nFrames = sfh.readf(samples.data(), FRAMES_BUFFER_SIZE)) {
        if (effectType == 1) {
            applySingleEcho(samples, echoBuffer, nFrames, 0.5 * sfh.samplerate());
        } else if (effectType == 2) {
            applyMultipleEchoes(samples, nFrames, {0.2 * sfh.samplerate(), 0.4 * sfh.samplerate(), 0.6 * sfh.samplerate()});
        } else if (effectType == 3) {
            applyAmplitudeModulation(samples, nFrames, modulationFrequency, sfh.samplerate());
        } else if (effectType == 4) {
            applyTimeVaryingDelays(samples, nFrames, {0.2, 0.5, 0.8}, sfh.samplerate());
        } else if (effectType == 5) {
            applySlowMotion(samples, nFrames, 2.0);
        } else if (effectType == 6) {
            applyFastForward(samples, nFrames, 2.0);
        }

        sfhOut.writef(samples.data(), nFrames);
    }

    cout << "Effect applied successfully.\n";

    return 0;
}
