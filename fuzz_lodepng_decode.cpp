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


// Input domain
// Describes valid colortype values
Domain<LodePNGColorType> AnyColortype() {
    return ElementOf<LodePNGColorType>({LCT_GREY, LCT_RGB, LCT_PALETTE, LCT_GREY_ALPHA, LCT_RGBA});
}

// Input domain
// Describes valid bitdepth values for a given colortype
Domain<unsigned int> AnyBitdepthForColortype(LodePNGColorType t) {
    switch(t) {
        case LCT_GREY:
            return ElementOf<unsigned int>({1, 2, 4, 8, 16});
        case LCT_RGB:
            return ElementOf<unsigned int>({8, 16});
        case LCT_PALETTE:
            return ElementOf<unsigned int>({1, 2, 4, 8});
        case LCT_GREY_ALPHA:
            return ElementOf<unsigned int>({8, 16});
        case LCT_RGBA:
            return ElementOf<unsigned int>({8, 16});
        default:
            assert(0);
    }
}

// Input domain
// Describes valid pairs of colortype and bitdepth
Domain<std::pair<LodePNGColorType, unsigned int>> AnyColortypeBitdepthPair() {
    auto valid_colortype_bitdepth_pair = [](const LodePNGColorType t) {
        return PairOf(Just(t), AnyBitdepthForColortype(t));
    };
    return FlatMap(valid_colortype_bitdepth_pair, AnyColortype());
}

// Helper function
// Tries to decode a PNG image from the given settings and data
unsigned TryDecode(lodepng::State& state, std::vector<unsigned char> &data) {
    unsigned w, h;
    std::vector<unsigned char> image;
    return lodepng::decode(image, w, h, state, data.data(), data.size());
}

// Fuzz test
// Fuzzes the lodepng::decode function
void FuzzLodepngDecode(std::vector<unsigned char> data, std::pair<LodePNGColorType, unsigned int> settings) {
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

        state.info_raw.colortype = settings.first;
        state.info_raw.bitdepth = settings.second;
        TryDecode(state, data);
    }
}
FUZZ_TEST(FuzztestSuite, FuzzLodepngDecode)
    .WithDomains(
        NonEmpty(Arbitrary<std::vector<unsigned char>>()),
        AnyColortypeBitdepthPair());

