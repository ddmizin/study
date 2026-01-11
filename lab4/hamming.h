#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> encodeHamming(const std::vector<uint8_t>& data);
bool decodeHamming(std::vector<uint8_t>& data);
