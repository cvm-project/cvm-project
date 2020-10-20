//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

#include "runtime/jit/operators/murmur_hash2.hpp"

#include <cassert>

namespace runtime::operators::MurmurHash2 {

auto Hash(const void *const key, unsigned int len, const unsigned int seed)
        -> unsigned int {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    constexpr unsigned int m = 0x5bd1e995;
    constexpr unsigned int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const auto *data = reinterpret_cast<const unsigned char *>(key);

    while (len >= 4) {
        auto k = *reinterpret_cast<const unsigned int *>(data);

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len) {
        case 3:
            h ^= data[2] << 16;  // NOLINT
        case 2:
            h ^= data[1] << 8;  // NOLINT
        case 1:
            h ^= data[0];  // NOLINT
            h *= m;
        default:
            assert(len == 0);
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;  // NOLINT
    h *= m;
    h ^= h >> 15;  // NOLINT

    return h;
}

}  // namespace runtime::operators::MurmurHash2
