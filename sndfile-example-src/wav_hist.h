#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <iomanip>
#include <fstream>
#include <limits>

class WAVHist {
private:
    std::vector<std::map<short, size_t>> counts;
    std::map<short, long> leftCounts;  // Histograma canal esquerdo (LEFT)
    std::map<short, long> rightCounts; // Histograma canal direito (RIGHT)
    std::map<short, long> monoCounts;  // Histograma da média dos canais (MID)
    std::map<short, long> sideCounts;  // Histograma da diferença dos canais (SIDE)

public:
     WAVHist(const SndfileHandle& sfh) {
         counts.resize(sfh.channels());
    }

    void update(const std::vector<short>& samples) {
        for (auto s : samples) {
            for (size_t channel = 0; channel < counts.size(); ++channel) {
                counts[channel][s]++;
            }
        }

        for (long unsigned int i = 0; i < samples.size() / 2; i++) {
            // LEFT 
            leftCounts[samples[2 * i]]++;

            // RIGHT
            rightCounts[samples[2 * i + 1]]++;

            // MID
            long mid = (samples[2 * i] + samples[2 * i + 1]) / 2;
            monoCounts[mid]++;

            // SIDE
            long side = (samples[2 * i] - samples[2 * i + 1]) / 2;
            sideCounts[side]++;
        }
    }

    void saveHistogramData(const std::string& filename, const std::map<short, long>& histogram) const {
        std::ofstream outfile(filename);
        if (outfile.is_open()) {
            for (const auto& [value, counter] : histogram) {
                outfile << value << '\t' << counter << '\n';
            }
            outfile.close();
            std::cout << "Histogram data saved to " << filename << std::endl;
        } else {
            std::cerr << "Error: Unable to open file for saving histogram data." << std::endl;
        }
    }

    void saveHistograms() {
        saveHistogramData("left_histogram.txt", leftCounts);
        saveHistogramData("right_histogram.txt", rightCounts);
        saveHistogramData("mid_histogram.txt", monoCounts);
        saveHistogramData("side_histogram.txt", sideCounts);
    }

    // void dump(const size_t channel) const {
    //     for (auto [value, counter] : counts[channel])
    //         std::cout << value << '\t' << counter << '\n';
    // }

    // Histograma da média dos canais (MID)
    void dumpMono() {
        std::cout << '\n';
        std::cout << "MID VALUES -----------------------------------" << std::endl;
        for (const auto& [value, counter] : monoCounts) {
            std::cout << "Value:" << std::setw(6) << value << " | Counter:" << std::setw(6) << counter << '\n';
        }
    }

    // Histograma da diferença dos canais (SIDE)
    void dumpSide() {
        std::cout << '\n';
        std::cout << "SIDE VALUES ----------------------------------" << std::endl;
        for (const auto& [value, counter] : sideCounts) {
            std::cout << "Value:" << std::setw(6) << value << " | Counter:" << std::setw(6) << counter << '\n';
        }
    }
};

#endif
