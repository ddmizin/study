#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Entry {
    std::string name;
    std::vector<uint8_t> data;
};

void CreateArchive(const std::string& archive, const std::vector<std::string>& files);
void ListArchive(const std::string& archive);
void ExtractArchive(const std::string& archive, const std::vector<std::string>& files);
void AppendFile(const std::string& archive, const std::string& file);
void DeleteFile(const std::string& archive, const std::string& file);
void MergeArchives(const std::string& a, const std::string& b, const std::string& out);
