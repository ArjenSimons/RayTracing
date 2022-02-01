#pragma once


class Accumulator {
private:
	uint sample_amount = 0;
	// Accumulates energy.
	Color accumulated_energy[SCRWIDTH][SCRHEIGHT];
public:
	inline Color accumulateEnergy(Color& energy, int x, int y) {
		Color total_energy = accumulated_energy[x][y];
		// If this is the first sample, we just return the current energy.
		if (sample_amount == 1) {
			accumulated_energy[x][y] = energy;
			return energy;
		}

		// Calculate the new average.
		Color new_energy = total_energy + ((energy - total_energy) / static_cast<float>(sample_amount));
		accumulated_energy[x][y] = new_energy;

		return new_energy;
	}
	inline void incrementAccumulation() { sample_amount += 1; }
	inline Accumulator() {}
	inline void resetAccumulator() {
		sample_amount = 0;
		memset(&accumulated_energy, 0, (size_t)(SCRWIDTH * SCRHEIGHT) * sizeof(Color));
	}
};
