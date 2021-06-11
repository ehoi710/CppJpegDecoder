#pragma once

#include <math.h>

constexpr double PI = 3.141592;

struct IDCT {
	double table[64] = { 0.0, };
	double val = 1 / sqrt(2);

	IDCT() {
		for (int u = 0; u < 8; u++) {
			for (int x = 0; x < 8; x++) {
				table[u * 8 + x] = (u == 0 ? val : 1) * 
					cos(((2.0 * x + 1.0) * u * PI) / 16.0);
			}
		}
	}

	inline double operator[](int idx) const {
		return table[idx];
	}
	inline double& operator[](int idx) {
		return table[idx];
	}
};