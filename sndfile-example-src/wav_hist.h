#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <iomanip> 


class WAVHist {
private:
    std::vector<std::map<short, size_t>> counts;
    std::map<short, size_t> monoCounts;  // Histograma da média dos canais (MID)
    std::map<short, size_t> sideCounts;  // Histograma da diferença dos canais (SIDE)


public:
  WAVHist(const SndfileHandle& sfh) {
		counts.resize(sfh.channels());
	}


    void update(const std::vector<short>& samples) {
        size_t n { };
        for (auto s : samples) {
            counts[n % counts.size()][s]++;
            n++;

            // Calculo da média dos canais (MID) e da diferença dos canais (SIDE)
            if (counts.size() == 2) {
                short left = samples[n % counts.size()];
                short right = samples[(n + 1) % counts.size()];
                short mid = (left + right) / 2;
                short side = (left - right) / 2;
                monoCounts[mid]++;
                sideCounts[side]++;
            }
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
