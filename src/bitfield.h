//
// Created by Shae Bolt on 12/20/2017.
//

#ifndef ALURE_BITFIELD_H
#define ALURE_BITFIELD_H
#include "alure2.h"

namespace alure {

    template<size_t N>
    struct Bitfield {
    private:
        Array<uint8_t, (N + 7) / 8> mElems;

    public:
        Bitfield() { std::fill(mElems.begin(), mElems.end(), 0); }

        bool operator[](size_t i) const noexcept { return static_cast<bool>(mElems[i / 8] & (1 << (i % 8))); }

        void set(size_t i) noexcept { mElems[i / 8] |= 1 << (i % 8); }
    };
};
#endif //ALURE_BITFIELD_H
