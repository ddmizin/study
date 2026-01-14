#include "hamming.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <cstddef>

static uint8_t Encode4(uint8_t d) {
    uint8_t d0 = (d >> 0) & 1;
    uint8_t d1 = (d >> 1) & 1;
    uint8_t d2 = (d >> 2) & 1;
    uint8_t d3 = (d >> 3) & 1;

    uint8_t p1 = d0 ^ d1 ^ d3;
    uint8_t p2 = d0 ^ d2 ^ d3;
    uint8_t p3 = d1 ^ d2 ^ d3;

    return (p1 << 0) | (p2 << 1) | (d0 << 2) |
           (p3 << 3) | (d1 << 4) | (d2 << 5) | (d3 << 6);
}

static bool Decode7(uint8_t& b) {
    uint8_t p1 = (b >> 0) & 1;
    uint8_t p2 = (b >> 1) & 1;
    uint8_t d0 = (b >> 2) & 1;
    uint8_t p3 = (b >> 3) & 1;
    uint8_t d1 = (b >> 4) & 1;
    uint8_t d2 = (b >> 5) & 1;
    uint8_t d3 = (b >> 6) & 1;

    uint8_t s1 = p1 ^ d0 ^ d1 ^ d3;
    uint8_t s2 = p2 ^ d0 ^ d2 ^ d3;
    uint8_t s3 = p3 ^ d1 ^ d2 ^ d3;

    uint8_t syndrome = s1 | (s2 << 1) | (s3 << 2);

    if (syndrome > 0 && syndrome <= 7) {
        b ^= (1 << (syndrome - 1));
    } 
    else if (syndrome != 0) {
        std::cerr << "Archive corrupted: multiple bit errors\n";
        return false;
    }
    b = (d0 << 0) | (d1 << 1) | (d2 << 2) | (d3 << 3);
    return true;
}

std::vector<uint8_t> EncodeHamming(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> out;
    for (uint8_t b : data) {
        out.push_back(Encode4(b & 0xF));
        out.push_back(Encode4(b >> 4));
    }
    return out;
}

bool DecodeHamming(std::vector<uint8_t>& data) {
    std::vector<uint8_t> decoded;
    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        uint8_t lo = data[i];
        uint8_t hi = data[i + 1];
        if (!Decode7(lo) || !Decode7(hi)) {
            return false;
        }
        decoded.push_back((hi << 4) | lo);
    }
    data = decoded;
    return true;
}
