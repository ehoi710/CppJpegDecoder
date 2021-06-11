#pragma once

#include <vector>

#include "utils.h"

class Bitstream {
public:
	Bitstream() : data(), pos(0) { }
	Bitstream(std::vector<uint8> data) {
		this->data = data;
		this->pos = 0;
	}

public:
	inline int getPos() const {
		return pos;
	}

	inline uint8  getBit() {
		uint8 value = (data[pos >> 3] >> (7 - (pos & 0x7))) & 0x01; pos++;
		return value;
	}
	inline uint32 getBitN(int n) {
		uint32 value = 0;
		while ((8 - (pos % 8)) < n) {
			value = (value << (8 - (pos % 8))) | 
				(data[pos >> 3] & ((1 << (8 - (pos % 8))) - 1));
			n -= (8 - (pos % 8));
			pos += (8 - (pos % 8));
		}
		while (n > 0) {
			value = (value << 1) | getBit(); n--; 
		}

		return value;
	}

private:
	std::vector<uint8> data;
	int pos;
};