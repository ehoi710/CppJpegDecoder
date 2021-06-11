#pragma once

#include <stdio.h>
#include <assert.h>

#include <array>
#include <map>
#include <type_traits>
#include <thread>

#include "Utils.h"
#include "Bitstream.h"
#include "Image.h"
#include "Bitmap.h"
#include "IDCT.h"

#pragma warning(disable : 4996)

// Failed!
// Check everything!
namespace EJPEG {

	using Code = unsigned int;
	using HuffmanKey = std::pair<Code, size_t>;
	using HuffmanTable = std::map<HuffmanKey, uint8>;

	using QuantizationTable = uint16[64];

	struct Component {
		double table[64] = { 0, };

		double& operator[](int idx);
		double		operator[](int idx) const;

		Component& operator=(const Component& right);
	};
	using MCU = Component[3];

	constexpr auto ONE_BY_ONE = 0;
	constexpr auto TWO_BY_TWO = 1;

	class JPEG : public Image {
	public:
		JPEG();
		JPEG(std::string file_path);

	private:
		int			getType(uint8 type);
		int			getSample();

		std::vector<uint8>
					removeFF00(std::vector<uint8>::iterator iter);

		void		decodeSOF(std::vector<uint8>::iterator iter);
		void		decodeDHT(std::vector<uint8>::iterator iter);
		void		decodeDQT(std::vector<uint8>::iterator iter);
		size_t		decodeSOS(std::vector<uint8>::iterator iter);

		void		decodeHeader();

		/****************************************/

		int			decodeNumber(size_t code, int bits);
		uint8		getCode(Bitstream& scanStream, HuffmanTable& huff);

		Component	rearrange_to_ZZ(Component x);
		Component	buildMatrix(Bitstream& scanStream, int idx, QuantizationTable quant, double& oldCoef);

		void		VLDOneByOne(Bitstream& scanStream, MCU** mcus);
		void		VLDTwoByTwoRange(Component* comps, MCU** mcus, Bound v_bound, Bound h_bound);
		void		VLDTwoByTwo(Bitstream& scanStream, MCU** mcus);

		MCU** VLD();

		/****************************************/

		Component	calcIdct(Component comp);
		void		idctToImage(MCU** mcus);

	public:
		void		decode();

		Size		getSize() const;
		RGBA		getPixel(int x, int y) const;

	private:
		static const IDCT idct;

		int colorScale;

		int HSubsamp[3];
		int VSubsamp[3];
		int quantMap[3];

		std::vector<uint8> rawData;
		std::vector<uint8> scanData;

		std::vector<std::vector<RGBA>> data;

		HuffmanTable      huffmanTables[4];

		QuantizationTable quantTables[2];

		Size size;
		Size mcuCount;
	};
}