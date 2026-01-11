#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Entry {
    std::string name;
    std::vector<uint8_t> data;
};

void createArchive(const std::string& archive, const std::vector<std::string>& files);
void listArchive(const std::string& archive);
void extractArchive(const std::string& archive);
void appendFile(const std::string& archive, const std::string& file);
void deleteFile(const std::string& archive, const std::string& file);
void mergeArchives(const std::string& a, const std::string& b, const std::string& out);
