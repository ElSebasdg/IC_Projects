#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames

// Cálculo da norma L2 (erro médio quadrático)
double calculateL2Norm(const vector<short>& original, const vector<short>& modified) {
    if (original.size() != modified.size()) {
        cerr << "Error: Audio files have different sizes\n";
        return -1;
    }

    double sum = 0.0;
    for (size_t i = 0; i < original.size(); ++i) {
        double diff = static_cast<double>(original[i]) - static_cast<double>(modified[i]);
        sum += diff * diff;
    }

    return sqrt(sum / original.size());
}

// Cálculo da norma L∞ (erro absoluto máximo por amostra)
double calculateLInfinityNorm(const vector<short>& original, const vector<short>& modified) {
    if (original.size() != modified.size()) {
        cerr << "Error: Audio files have different sizes\n";
        return -1;
    }

    double maxError = 0.0;
    for (size_t i = 0; i < original.size(); ++i) {
        double error = abs(static_cast<double>(original[i]) - static_cast<double>(modified[i]));
        maxError = max(maxError, error);
    }

    return maxError;
}

// Cálculo da Relação Sinal-Ruído (SNR)
double calculateSNR(const vector<short>& original, const vector<short>& modified) {
    if (original.size() != modified.size()) {
        cerr << "Error: Audio files have different sizes\n";
        return -1;
    }

    double signalPower = 0.0;
    double noisePower = 0.0;

    for (size_t i = 0; i < original.size(); ++i) {
        double signalSample = static_cast<double>(original[i]);
        double noiseSample = static_cast<double>(original[i]) - static_cast<double>(modified[i]);

        signalPower += signalSample * signalSample;
        noisePower += noiseSample * noiseSample;
    }

    return 10.0 * log10(signalPower / noisePower);
}

int main(int argc, char *argv[]) {
    bool verbose { false };

    if (argc < 3) {
        cerr << "Usage: wavcmp -v \n";
        cerr << "              originalWavFile modifiedWavFile\n";
        return 1;
    }

    for (int n = 1; n < argc; n++) {
        if (string(argv[n]) == "-v") {
            verbose = true;
            break;
        }
    }

    SndfileHandle sfhOriginal { argv[argc - 2] };
    if (sfhOriginal.error()) {
        cerr << "Error: invalid original input file\n";
        return 1;
    }

    SndfileHandle sfhModified { argv[argc - 1] };
    if (sfhModified.error()) {
        cerr << "Error: invalid modified input file\n";
        return 1;
    }

    vector<short> originalSamples(FRAMES_BUFFER_SIZE * sfhOriginal.channels());
    vector<short> modifiedSamples(FRAMES_BUFFER_SIZE * sfhModified.channels());

    size_t nFramesOriginal;
    size_t nFramesModified;

    double l2Norm = 0.0;
    double lInfinityNorm = 0.0;
    double snr = 0.0;

    while ((nFramesOriginal = sfhOriginal.readf(originalSamples.data(), FRAMES_BUFFER_SIZE)) &&
           (nFramesModified = sfhModified.readf(modifiedSamples.data(), FRAMES_BUFFER_SIZE))) {
        l2Norm += calculateL2Norm(originalSamples, modifiedSamples);
        lInfinityNorm = max(lInfinityNorm, calculateLInfinityNorm(originalSamples, modifiedSamples));
        snr += calculateSNR(originalSamples, modifiedSamples);
    }

    l2Norm /= static_cast<double>(sfhOriginal.frames());
    snr /= static_cast<double>(sfhOriginal.frames());

    if (verbose) {
        cout << "Overall Metrics:\n";
        cout << "L2 Norm: " << l2Norm << "\n";
        cout << "L∞ Norm: " << lInfinityNorm << "\n";
        cout << "SNR: " << snr << " dB\n";
    } else {
        cout << "L2 Norm: " << l2Norm << "\n";
        cout << "L∞ Norm: " << lInfinityNorm << "\n";
        cout << "SNR: " << snr << " dB\n";
    }

    return 0;
}
