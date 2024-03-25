/*
 * This is an example fuzz test for the lodepng
 * library using the google/fuzztest framework.
 *
 * Based on:
 * https://github.com/lvandeve/lodepng/blob/master/lodepng_fuzzer.cpp
 */

#include "fuzztest/fuzztest.h"
#include "lodepng.h"

using namespace fuzztest;

// Amount of valid colortype/bidthdepth combinations in the PNG file format.
const size_t num_combinations = 15;

LodePNGColorType colortypes[num_combinations] = {
  LCT_GREY, LCT_GREY, LCT_GREY, LCT_GREY, LCT_GREY, // 1, 2, 4, 8 or 16 bits
  LCT_RGB, LCT_RGB, // 8 or 16 bits
  LCT_PALETTE, LCT_PALETTE, LCT_PALETTE, LCT_PALETTE, // 1, 2, 4 or 8 bits
  LCT_GREY_ALPHA, LCT_GREY_ALPHA, // 8 or 16 bits
  LCT_RGBA, LCT_RGBA, // 8 or 16 bits
};

unsigned bitdepths[num_combinations] = {
  1, 2, 4, 8, 16, // gray
  8, 16, // rgb
  1, 2, 4, 8, // palette
  8, 16, // gray+alpha
  8, 16, // rgb+alpha
};

// Helper function
// Tries to decode a PNG image from the given settings and data
unsigned TryDecode(lodepng::State& state, const std::vector<unsigned char> &data) {
    unsigned w, h;
    std::vector<unsigned char> image;
    return lodepng::decode(image, w, h, state, data);
}

// Fuzz test
// Fuzzes the lodepng::decode function
void FuzzLodepngDecode(const std::vector<unsigned char> &data, unsigned int idx) {
    idx %= num_combinations;
    LodePNGColorType colortype = colortypes[idx];
    unsigned bitdepth = bitdepths[idx];

    // Make the decoder ignore three types of checksums the PNG/zlib format have
    // built-in, because they are less likely to be correct in the random input
    // data, and if invalid make the decoder return an error before much gets ran.
    lodepng::State state;
    state.decoder.zlibsettings.ignore_adler32 = 1;
    state.decoder.zlibsettings.ignore_nlen = 1;
    state.decoder.ignore_crc = 1;
    // Also make decoder attempt to support partial files with missing ending to
    // go further with parsing.
    state.decoder.ignore_end = 1;
    // First test without color conversion (keep color type of the PNG)
    state.decoder.color_convert = 0;

    unsigned error = TryDecode(state, data);

    // If valid PNG found, try decoding with color conversion to the most common
    // default color type, and to the randomly chosen type.
    if(error == 0) {
        state.decoder.color_convert = 1;
        TryDecode(state, data);

        state.info_raw.colortype = colortype;
        state.info_raw.bitdepth = bitdepth;
        TryDecode(state, data);
    }
}
FUZZ_TEST(FuzztestSuite, FuzzLodepngDecode);

