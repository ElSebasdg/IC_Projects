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
    std::map<short, size_t> monoCounts;  // Histograma da média dos canais (MID)
    std::map<short, size_t> sideCounts;  // Histograma da diferença dos canais (SIDE)

    std::map<short, size_t> monoCoarseCounts;  // Barras MID
    std::map<short, size_t> sideCoarseCounts;  // Barras SIDE



public:
  WAVHist(const SndfileHandle& sfh) {
		counts.resize(sfh.channels());
	}


    void update(const std::vector<short>& samples) {
            size_t n { };
            for (auto s : samples) {
                for (size_t channel = 0; channel < counts.size(); ++channel) {
                    counts[channel][s]++;
                }
                n++;

                if (counts.size() == 2) {
                    short left = samples[n % counts.size()];
                    short right = samples[(n + 1) % counts.size()];
                    short mid = (left + right) / 2;
                    short side = (left - right) / 2;
                    monoCounts[mid]++;
                    sideCounts[side]++;

                    short coarseMid = mid / 2 * 2;
                    short coarseSide = side / 2 * 2;
                    monoCoarseCounts[coarseMid]++;
                    sideCoarseCounts[coarseSide]++;
                }
            }
    }



    void saveHistogramData(const std::string& filename, const std::map<short, size_t>& histogram) const {
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
        saveHistogramData("mid_histogram.txt", monoCoarseCounts);
        saveHistogramData("side_histogram.txt", sideCoarseCounts);
    }

    void update_mid(const std::vector<short>& samples) {
        for (long unsigned int i = 0; i < samples.size() / 2; i++) {
            short mid = (samples[2 * i] + samples[2 * i + 1]) / 2;
            monoCounts[mid]++;

            short coarseMid = mid / 2 * 2;
            monoCoarseCounts[coarseMid]++;
        }
    }

    void update_side(const std::vector<short>& samples) {
        for (long unsigned int i = 0; i < samples.size() / 2; i++) {
            short side = (samples[2 * i] - samples[2 * i + 1]) / 2;
            sideCounts[side]++;

            short coarseSide = side / 2 * 2;
            sideCoarseCounts[coarseSide]++;
        }
    }
        
    void dump(const size_t channel) const {
        for (auto [value, counter] : counts[channel])
            std::cout << value << '\t' << counter << '\n';
    }

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
