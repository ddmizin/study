#pragma once
#include <vector>
#include <cstdint>

std::vector<uint8_t> EncodeHamming(const std::vector<uint8_t>& data);
bool DecodeHamming(std::vector<uint8_t>& data);
