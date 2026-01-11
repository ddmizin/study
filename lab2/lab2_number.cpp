#include "lab2_number.h"
#include <cstdint>
#include <iostream>

namespace {

    bool IsZero(const int2025_t& number){
        for (int i = 0; i < int2025_t::kSIZE; i++){
            if (number.data[i] != 0){
                return false;
            }
        }
        return true;
    }

    void ShiftToLeft(int2025_t& number){
        uint16_t trn = 0;
        for (int i = 0; i < int2025_t::kSIZE; i++){
            uint16_t value = (number.data[i] << 1) | trn;
            number.data[i] = value & 0xFF;
            trn = value >> 8;
        }
    }

    void ShiftToRight(int2025_t& number){
        uint8_t trn = 0;
        for (int i = int2025_t::kSIZE - 1; i >= 0; i--){
            uint8_t new_trn = number.data[i] & 1;
            number.data[i] = (number.data[i] >> 1) | (trn << 7);
            trn = new_trn;
        }
    }

     void SetZero(int2025_t& number){
        for (int i = 0; i < int2025_t::kSIZE; i++){
            number.data[i] = 0;
        }
        number.is_positive = true;
    }

    bool GetBit(const int2025_t& number, int bit_pos) {
        int byte = bit_pos / 8;
        int bit_in_byte = bit_pos % 8;
        return (number.data[byte] >> bit_in_byte) & 1;
    }

    void SetBit(int2025_t& number, int bit_pos, bool flag) {
        int byte = bit_pos / 8;
        int bit_in_byte = bit_pos % 8;
        if (flag) {
            number.data[byte] |= (1 << bit_in_byte);
        } else {
            number.data[byte] &= ~(1 << bit_in_byte);
        }
    }

    int CompareAbsoluteNumber(const int2025_t& lhs, const int2025_t& rhs) {
        for (int i = int2025_t::kSIZE - 1; i >= 0; i--) {
            if (lhs.data[i] != rhs.data[i]) {
                if (lhs.data[i] < rhs.data[i]){
                    return -1;
                }
                else {
                    return 1;
                }
            }
        }
        return 0;
    }

    int2025_t AddAbsoluteNumber(const int2025_t& lhs, const int2025_t& rhs) {
        int2025_t result;
        SetZero(result);
        uint16_t trn = 0;
        for (int i = 0; i < int2025_t::kSIZE; i++) {
            uint16_t sum = lhs.data[i] + rhs.data[i] + trn;
            result.data[i] = sum & 0xFF;
            trn = sum >> 8;
        }
        return result;
    }

    int2025_t SubtractAbsoluteNumber(const int2025_t& lhs, const int2025_t& rhs) {
        int2025_t result;
        SetZero(result);
        int16_t bor = 0;
        for (int i = 0; i < int2025_t::kSIZE; i++) {
            int16_t diff = lhs.data[i] - rhs.data[i] - bor;
            if (diff < 0) {
                diff += 256;
                bor = 1;
            } else {
                bor = 0;
            }
            result.data[i] = diff & 0xFF;
        }
        return result;
    }
} //namespace


int2025_t from_int(int32_t i) {
    int2025_t result;
    SetZero(result);
    if (i < 0) {
        result.is_positive = false;
        i = -i; 
    }
    for (int j = 0; j < 4 && j < int2025_t::kSIZE; j++) {
        result.data[j] = (i >> (j * 8)) & 0xFF;
    }
    return result;
}

int2025_t from_string(const char* buff) {
    int2025_t result = from_int(0);
    const char* ptr = buff;
    while (*ptr && (*ptr == ' ' || *ptr == '\t')) {
        ptr++;
    }
    bool positive = true;
    if (*ptr == '-') {
        positive = false;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }
    while (*ptr == '0') {
        ptr++;
    }
    while (*ptr >= '0' && *ptr <= '9') {
        int2025_t result_x2 = result;
        ShiftToLeft(result_x2); 
        int2025_t result_x8 = result;
        for (int i = 0; i < 3; i++) {
            ShiftToLeft(result_x8); 
        }
        result = AddAbsoluteNumber(result_x2, result_x8); 
        int2025_t digit = from_int(*ptr - '0');
        result = AddAbsoluteNumber(result, digit);
        ptr++;
    }
    result.is_positive = positive;
    return result;
}

int2025_t operator+(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t result;
    SetZero(result);
    if (lhs.is_positive == rhs.is_positive) {
        result = AddAbsoluteNumber(lhs, rhs);
        result.is_positive = lhs.is_positive;
    } else {
        int sign = CompareAbsoluteNumber(lhs, rhs);
        if (sign >= 0) {
            result = SubtractAbsoluteNumber(lhs, rhs);
            result.is_positive = lhs.is_positive;
        } else {
            result = SubtractAbsoluteNumber(rhs, lhs);
            result.is_positive = rhs.is_positive;
        }
    }
    if (IsZero(result)) {
        result.is_positive = true;
    }
    return result;
}

int2025_t operator-(const int2025_t& lhs, const int2025_t& rhs) {
    int2025_t negative_rhs = rhs;
    negative_rhs.is_positive = !rhs.is_positive;
    return lhs + negative_rhs;
}

int2025_t operator*(const int2025_t& lhs, const int2025_t& rhs) {
    if (IsZero(lhs) || IsZero(rhs)) {
        return from_int(0);
    }
    if (lhs == from_int(1)) {
        return rhs;
    }
    if (rhs == from_int(1)) {
        return lhs;
    }
    if (lhs == from_int(-1)) {
        int2025_t result = rhs;
        result.is_positive = !rhs.is_positive;
        return result;
    }
    if (rhs == from_int(-1)) {
        int2025_t result = lhs;
        result.is_positive = !lhs.is_positive;
        return result;
    }
    int2025_t result;
    SetZero(result);
    uint16_t temp[int2025_t::kSIZE * 2] = {0};
    for (int i = 0; i < int2025_t::kSIZE; i++) {
        if (lhs.data[i] == 0) {
            continue; 
        }
        uint16_t trn = 0;
        uint8_t lhs_byte = lhs.data[i];
        for (int j = 0; j < int2025_t::kSIZE; j++) {
            if (rhs.data[j] == 0 && trn == 0) {
                continue; 
            }
            uint16_t res = (uint16_t)lhs_byte * rhs.data[j] + temp[i + j] + trn;
            temp[i + j] = res & 0xFF;
            trn = res >> 8;
        }
        if (trn > 0 && (i + int2025_t::kSIZE) < (int2025_t::kSIZE * 2)) {
            temp[i + int2025_t::kSIZE] += trn;
        }
    }
    for (int i = 0; i < int2025_t::kSIZE; i++) {
        result.data[i] = temp[i] & 0xFF;
    }
    result.is_positive = (lhs.is_positive == rhs.is_positive);
    return result;
}

int2025_t operator/(const int2025_t& lhs, const int2025_t& rhs) {
    if (IsZero(rhs)){
        return from_int(0);
    }
    if (IsZero(lhs)){
        return from_int(0);
    }
    int2025_t result;
    SetZero(result);
    int2025_t divisible = lhs;
    int2025_t divider = rhs;

    divisible.is_positive = true;
    divider.is_positive = true;
    int2025_t remains;
    SetZero(remains);
    for (int i = int2025_t::kBITS - 1; i >= 0; i--){
        ShiftToLeft(remains);
        SetBit(remains, 0, GetBit(divisible, i));
        if (CompareAbsoluteNumber(remains, divider) >= 0){
            remains = SubtractAbsoluteNumber(remains, divider);
            SetBit(result, i, true);
        }
    }
    result.is_positive = (lhs.is_positive == rhs.is_positive);
    return result;
}

bool operator==(const int2025_t& lhs, const int2025_t& rhs) {
    if (IsZero(lhs) && IsZero(rhs)){
        return true;
    }
    if (lhs.is_positive != rhs.is_positive){
        return false;
    }
    return CompareAbsoluteNumber(lhs,rhs) == 0;
}

bool operator!=(const int2025_t& lhs, const int2025_t& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const int2025_t& value) {
    if (IsZero(value)){
        stream << "0";
        return stream;
    }
    if (!value.is_positive){
        stream << "-";
    }

    int2025_t number = value;
    number.is_positive = false;
    char buffer[610];
    for (int i = 0; i < 610; i++){
        buffer[i] = 0;
    }
    int pos = 0;
    int2025_t zero = from_int(0);
    int2025_t ten = from_int(10);
    while(!(number == zero)){
        int2025_t full = number / ten;
        int2025_t remains = number - ( full * ten);
        int digit = 0;
        for (int i = 0; i < 4; i++){
            digit |= (remains.data[i] << (i*8));
        }
        buffer[pos++] = '0' + digit;
        number = full;
        if (pos > 609){
            break;
        }
    }
    for (int i = pos - 1; i >= 0; i--){
        stream << buffer[i];
    }
    return stream;
}