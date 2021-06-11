#include "JPEG.h"

namespace EJPEG {
	const IDCT JPEG::idct = IDCT();

	/********************************************************************************/

	double&		Component::operator[](int idx) {
		return table[idx];
	}
	double		Component::operator[](int idx) const {
		return table[idx];
	}

	Component&	Component::operator=(const Component& right) {
		memcpy(this, &right, sizeof(Component));
		return *this;
	}

	/********************************************************************************/

	JPEG::JPEG() : size{ 0, 0 }, quantMap{ 0, } { }
	JPEG::JPEG(std::string file_path) : JPEG() {
		FILE* fp = fopen(file_path.c_str(), "rb");

		int len = getFileSize(fp);
		uint8* data = readFile(fp);

		this->rawData.clear();
		this->rawData.insert(this->rawData.begin(), data, data + len);

		delete[] data;
		fclose(fp);
	}

	int			JPEG::getType(uint8 type) {
		switch (type) {
		case 0x00: return 0;
		case 0x01: return 1;
		case 0x10: return 2;
		case 0x11: return 3;
		}
		throw std::exception("type is wrong!");
	}
	int			JPEG::getSample() {
		// 0: 4:4:4
		// 1: 4:2:2
		// 2: 4:2:0

		if (HSubsamp[0] == 2 && VSubsamp[0] == 2) {
			return TWO_BY_TWO;
		}

		return ONE_BY_ONE;
	}

	std::vector<uint8>
				JPEG::removeFF00(std::vector<uint8>::iterator iter) {
		std::vector<uint8> ff00Removed;

		while (1) {
			if (iter[0] == 0xFF) {
				if (iter[1] == 0x00) {
					ff00Removed.push_back(0xFF);
					iter++;
				}
				else break;
			}
			else {
				ff00Removed.push_back(iter[0]);
			}
			iter++;
		}

		return ff00Removed;
	}

	void		JPEG::decodeSOF (std::vector<uint8>::iterator iter) {
		// iter[0 ~ 1]: Lf
		// iter[2]	  : P
		// iter[3 ~ 4]: Y
		// iter[5 ~ 6]: X
		// iter[7]	  : Nf, Number of Frame Component

		iter += 3; // Skip Lf and P

		size.y = getWord(iter); iter += 2;
		size.x = getWord(iter); iter += 2;

		mcuCount.y = size.y / 8;
		if (mcuCount.y * 8 < size.y) mcuCount.y++;

		mcuCount.x = size.x / 8;
		if (mcuCount.x * 8 < size.x) mcuCount.x++;

		int number_of_frame_component = iter[0]; iter++;
		for (int i = 0; i < number_of_frame_component; i++) {
			HSubsamp[i] = iter[3 * i + 1] >> 4;
			VSubsamp[i] = iter[3 * i + 1] & 0x0F;
			quantMap[i] = iter[3 * i + 2];
		}
	}
	void		JPEG::decodeDHT (std::vector<uint8>::iterator iter) {
		// iter[0 ~ 1] : Length
		// iter[2]     : HT information
		// iter[3 ~ 18]: Number of symbols
		// iter[19 ~ ] : Symbols

		int len = getWord(iter);
		iter += 2; len -= 2;

		while (len > 0) {
			uint8 type = iter[0]; iter++; len--;

			std::vector<uint8> num_of_symbols(1);
			num_of_symbols.insert(num_of_symbols.begin() + 1, iter, iter + 16);
			iter += 16; len -= 16;

			int code = 0;
			for (int code_length = 1; code_length <= 16; code_length++) {
				code <<= 1;
				for (int count = 0; count < num_of_symbols[code_length]; count++) {
					huffmanTables[getType(type)].insert(
						std::pair<HuffmanKey, uint8>(HuffmanKey(code, code_length), iter[0])
					);
					code++;
					iter++; len--;
				}
			}
		}
	}
	void		JPEG::decodeDQT (std::vector<uint8>::iterator iter) {
		// iter[0 ~ 1]: length
		// iter[2]    : QT information
		// iter[3 ~ ] : Bytes

		int length = getWord(iter); iter += 2; // Skip Length
		length -= 2;

		while (length > 0) {
			uint8 type = iter[0]; iter++;
			int table_no = type & 0x01;

			for (int i = 0; i < 64; i++) {
				if (type >> 4 == 0) {
					quantTables[table_no][i] = iter[0];
					iter++;
				}
				else {
					quantTables[table_no][i] = getWord(iter);
					iter += 2;
				}
			}

			length -= 0x41;
		}
	}
	size_t		JPEG::decodeSOS (std::vector<uint8>::iterator iter) {
		int tot_length = 0;
		while (iter[tot_length] != 0xFF || iter[tot_length + 1] == 0x00) tot_length++;

		int length = getWord(iter);

		if (scanData.size() == 0) scanData = removeFF00(iter + length);
		else {
			std::vector<uint8> tmp = removeFF00(iter + length);
			scanData.insert(scanData.end(), tmp.begin(), tmp.end());
		}

		return tot_length;
	}

	void		JPEG::decodeHeader() {
		auto iter = rawData.begin();

		while (iter != rawData.end()) {
			uint16 marker = getWord(iter);
			iter += 2;

			if (marker == 0xFFC0) {
				decodeSOF(iter);
				iter += getWord(iter);
			}
			if (marker == 0xFFC4) {
				decodeDHT(iter);
				iter += getWord(iter);
			}
			if (marker == 0xFFDB) {
				decodeDQT(iter);
				iter += getWord(iter);
			}
			if (marker == 0xFFDA) {
				iter += decodeSOS(iter);
			}
		}
	}

	/****************************************/

	int			JPEG::decodeNumber(size_t code, int bits) {
		int l = 1 << (code - 1);

		if (bits >= l) return bits;
		else		   return bits - (2 * l - 1);
	}
	uint8		JPEG::getCode(Bitstream& scanStream, HuffmanTable& huff) {
		int code = 0;
		int length = 0;
		while (huff.find(HuffmanKey(code, length)) == huff.end()) {
			code = (code << 1) | scanStream.getBit();
			length++;

			if (length > 16) {
				throw std::exception("asdf");
			}
		}

		//printf("\n");

		return huff.at(HuffmanKey(code, length));
	}

	Component	JPEG::rearrange_to_ZZ(Component x) {
		static int rearrange_array[64] = {
				0,  1,  5,  6, 14, 15, 27, 28,
				2,  4,  7, 13, 16, 26, 29, 42,
				3,  8, 12, 17, 25, 30, 41, 43,
				9, 11, 18, 24, 31, 40, 44, 53,
			10, 19, 23, 32, 39, 45, 52, 54,
			20, 22, 33, 38, 46, 51, 55, 60,
			21, 34, 37, 47, 50, 56, 59, 61,
			35, 36, 48, 49, 57, 58, 62, 63
		};

		Component y;
		for (int i = 0; i < 64; i++) {
			y[i] = x[rearrange_array[i]];
		}
		return y;
	}
	Component	JPEG::buildMatrix(Bitstream& scanStream, int idx, QuantizationTable quant, double& oldCoef) {
		Component dcac;

		size_t code = getCode(scanStream, huffmanTables[idx]); // calculate DC component
		int bits = scanStream.getBitN(code);
		oldCoef += decodeNumber(code, bits);

		dcac[0] = oldCoef * quant[0];

		for (int i = 1; i < 64; i++) {
			code = getCode(scanStream, huffmanTables[0x2 + idx]);
			if (code == 0) break;

			if (code > 15) {
				i += code >> 4;
				code = code & 0x0F;
			}

			if (i < 64) {
				bits = scanStream.getBitN(code);
				double coef = decodeNumber(code, bits);
				dcac[i] = (double)quant[i] * coef;
			}
		}

		dcac = rearrange_to_ZZ(dcac);

		return dcac;
	}

	void		JPEG::VLDOneByOne(Bitstream& scanStream, MCU** mcus) {
		double coef[3] = { 0.0 };

		for (int y = 0; y < mcuCount.y; y++) {
			for (int x = 0; x < mcuCount.x; x++) {
				//printf("MCU at (%d, %d)\n", x, y);

				for (int i = 0; i < 3; i++) {
					mcus[y][x][i] = buildMatrix(scanStream, quantMap[i], quantTables[quantMap[i]], coef[i]);
					mcus[y][x][i] = calcIdct(mcus[y][x][i]);
				}
			}
		}
	}
	void		JPEG::VLDTwoByTwoRange(Component* comps, MCU** mcus, Bound v_bound, Bound h_bound) {
		Component Cb, Cr;
		for (int y = v_bound.start; y < v_bound.end; y += 2) {
			for (int x = h_bound.start; x < h_bound.end; x += 2) {
				int cnt = ((mcuCount.x + 1) / 2) * (y / 2) * 6 + (x * 3);

				for (int j = 0; j < 2; j++) {
					for (int i = 0; i < 2; i++) {
						if (y + j >= v_bound.end) continue;
						if (x + i >= h_bound.end) continue;
						mcus[y + j][x + i][0] = calcIdct(comps[cnt++]);
					}
				}

				Cb = calcIdct(comps[cnt++]);
				Cr = calcIdct(comps[cnt++]);

				for (int j = 0; j < 16; j++) {
					for (int i = 0; i < 16; i++) {
						if (y + (j / 8) >= v_bound.end) continue;
						if (x + (i / 8) >= h_bound.end) continue;

						MCU& mcu = mcus[y + (j / 8)][x + (i / 8)];

						mcu[1][(j % 8) * 8 + (i % 8)] = Cb[(j / 2) * 8 + (i / 2)];
						mcu[2][(j % 8) * 8 + (i % 8)] = Cr[(j / 2) * 8 + (i / 2)];
					}
				}

			}
		}
	}
	void		JPEG::VLDTwoByTwo(Bitstream& scanStream, MCU** mcus) {
		double coef[3] = { 0.0 };

		Component* comps = new Component[(mcuCount.y + 1) * (mcuCount.x + 1) * 3 / 2];

		int cnt = 0;
		for (int y = 0; y < mcuCount.y; y += 2) {
			for (int x = 0; x < mcuCount.x; x += 2) {
				for (int i = 0; i < 4; i++)
					comps[cnt++] = buildMatrix(scanStream, 0, quantTables[0], coef[0]);

				comps[cnt++] = buildMatrix(scanStream, 1, quantTables[1], coef[1]);
				comps[cnt++] = buildMatrix(scanStream, 1, quantTables[1], coef[2]);
			}
		}

		int x_mid = mcuCount.x / 2; if (x_mid % 2 == 1) x_mid--;
		int y_mid = mcuCount.y / 2; if (y_mid % 2 == 1) y_mid--;

		std::thread t0(&JPEG::VLDTwoByTwoRange, this, comps, mcus, Bound{ 0,     y_mid }, Bound{ 0,      x_mid });		// ÁÂ»ó
		std::thread t1(&JPEG::VLDTwoByTwoRange, this, comps, mcus, Bound{ 0,     y_mid }, Bound{ x_mid, mcuCount.x });	// ¿ì»ó
		std::thread t2(&JPEG::VLDTwoByTwoRange, this, comps, mcus, Bound{ y_mid, mcuCount.y }, Bound{ 0,      x_mid });		// ÁÂÇÏ
		std::thread t3(&JPEG::VLDTwoByTwoRange, this, comps, mcus, Bound{ y_mid, mcuCount.y }, Bound{ x_mid, mcuCount.x });	// ¿ìÇÏ

		t0.join();
		t1.join();
		t2.join();
		t3.join();

		delete[] comps;
	}

	MCU**		JPEG::VLD() {
		MCU** mcus = new MCU * [mcuCount.y + 1];
		for (int y = 0; y < mcuCount.y; y++) {
			mcus[y] = new MCU[mcuCount.x + 1];
		}

		Bitstream scanStream = Bitstream(scanData);

		switch (getSample()) {
		case ONE_BY_ONE: VLDOneByOne(scanStream, mcus); break;
		case TWO_BY_TWO: VLDTwoByTwo(scanStream, mcus); break;
		}

		return mcus;
	}

	/****************************************/

	Component	JPEG::calcIdct(Component comp) {
		Component tmp, res;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				double cache = comp[i * 8 + j];
				for (int k = 0; k < 8; k++) {
					tmp[i * 8 + k] += cache * idct[j * 8 + k];
				}
			}
		}

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				for (int k = 0; k < 8; k++) {
					res[i * 8 + j] += idct[k * 8 + i] * tmp[k * 8 + j];
				}

				res[i * 8 + j] = (int)res[i * 8 + j] / 4;
			}
		}

		return res;
	}
	void		JPEG::idctToImage(MCU** mcus) {
		YUV** yuvData = new YUV * [size.y];
		for (int y = 0; y < size.y; y++) {
			yuvData[y] = new YUV[size.x];
		}

		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {
				const MCU& mcu = mcus[y / 8][x / 8];

				yuvData[y][x].Y = (int)mcu[0][(y % 8) * 8 + (x % 8)];
				yuvData[y][x].Cb = (int)mcu[1][(y % 8) * 8 + (x % 8)];
				yuvData[y][x].Cr = (int)mcu[2][(y % 8) * 8 + (x % 8)];
			}
		}

		for (int y = 0; y < mcuCount.y; y++) {
			delete[] mcus[y];
		}
		delete[] mcus;

		data = std::vector<std::vector<RGBA>>(size.y);
		for (int y = 0; y < size.y; y++) {
			data[y] = std::vector<RGBA>(size.x);
		}
		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {
				data[y][x] = yuvToRgb(yuvData[y][x]);
			}
		}

		for (int y = 0; y < size.y; y++) {
			delete[] yuvData[y];
		}
		delete[] yuvData;

		return;
	}

	void JPEG::decode() {
		decodeHeader();
		MCU** mcus = VLD();
		idctToImage(mcus);
	}

	/****************************************/

	Size		JPEG::getSize() const {
		return size;
	}
	RGBA		JPEG::getPixel(int x, int y) const {
		return data[y][x];
	}
}