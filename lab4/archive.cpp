#include "archive.h"
#include "hamming.h"
#include "utils.h"
#include <fstream>
#include <cstring>
#include <iostream>
#include <cstdint>
#include <filesystem>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <stdexcept>

static const char kSignature[] = "HAF2026";

static std::vector<uint8_t> ReadFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}

static void WriteFile(const std::string& name, const std::vector<uint8_t>& data) {
    EnsureDirectoryExists(name);
    std::ofstream out(name, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}


static std::vector<Entry> ReadArchive(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open archive");

    char sig[8]{};
    in.read(sig, 7);
    if (std::string(sig) != kSignature)
        throw std::runtime_error("Invalid archive format");

    std::vector<uint8_t> encoded(std::istreambuf_iterator<char>(in), {});

    if (!DecodeHamming(encoded))
        throw std::runtime_error("Archive corrupted");

    size_t pos = 0;
    auto read = [&](void* dst, size_t sz) {std::memcpy(dst, encoded.data() + pos, sz); pos += sz;};

    size_t count;
    read(&count, sizeof(count));

    std::vector<Entry> entries;
    for (size_t i = 0; i < count; ++i) {
        Entry e;
        size_t name_len, data_len;

        read(&name_len, sizeof(name_len));
        e.name.resize(name_len);
        read(e.name.data(), name_len);

        read(&data_len, sizeof(data_len));
        e.data.resize(data_len);
        read(e.data.data(), data_len);

        entries.push_back(e);
    }

    return entries;
}


static void WriteArchive(const std::string& path, const std::vector<Entry>& entries) {
    EnsureDirectoryExists(path);
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(kSignature, 7);

    std::vector<uint8_t> payload;

    auto push = [&](const void* p, size_t sz) {const uint8_t* b = reinterpret_cast<const uint8_t*>(p); payload.insert(payload.end(), b, b + sz);};

    size_t count = entries.size();
    push(&count, sizeof(count));

    for (const auto& e : entries) {
        size_t name_len = e.name.size();
        size_t data_len = e.data.size();

        push(&name_len, sizeof(name_len));
        push(e.name.data(), name_len);

        push(&data_len, sizeof(data_len));
        push(e.data.data(), data_len);
    }

    auto encoded = EncodeHamming(payload);
    out.write(reinterpret_cast<char*>(encoded.data()), encoded.size());
}


void CreateArchive(const std::string& archive, const std::vector<std::string>& files) {
    std::vector<Entry> entries;
    for (const auto& f : files) {
        entries.push_back({f, ReadFile(f)});
    }
    WriteArchive(archive, entries);
}

void ListArchive(const std::string& archive) {
    for (const auto& e : ReadArchive(archive)) {
        std::cout << e.name << "\n";
    }
}

void ExtractArchive(const std::string& archive, const std::vector<std::string>& files)
{
    auto entries = ReadArchive(archive);

    auto extract_one = [&](const Entry& e) {
        std::filesystem::path p(e.name);
        std::string filename = p.filename().string();

        WriteFile(filename, e.data);
    };

    if (files.empty()) {
        for (const auto& e : entries) {
            extract_one(e);
        }
    } else {
        for (const auto& name : files) {
            auto it = std::find_if(entries.begin(), entries.end(), [&](const Entry& e) {return std::filesystem::path(e.name).filename()== std::filesystem::path(name).filename();});
            if (it != entries.end()) {
                extract_one(*it);
            }
        }
    }
}

void AppendFile(const std::string& archive, const std::string& file) {
    auto entries = ReadArchive(archive);
    auto data = ReadFile(file);
    auto it = std::find_if(entries.begin(), entries.end(), [&](const Entry& e) { return e.name == file; });

    if (it == entries.end()) {
        entries.push_back({file, data});
    } 
    else {
        std::cout << "File already exists in archive: " << file << "\n";
        std::cout << "1 - overwrite\n";
        std::cout << "2 - append content\n";
        std::cout << "3 - cancel\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            it->data = data;
        }
        else if (choice == 2) {
            it->data.insert(it->data.end(), data.begin(), data.end());
        }
        else {
            std::cout << "Operation cancelled\n";
            return;
        }
    }
    WriteArchive(archive, entries);
}

void DeleteFile(const std::string& archive, const std::string& file) {
    auto entries = ReadArchive(archive);
    entries.erase(std::remove_if(entries.begin(), entries.end(), [&](const Entry& e){ return e.name == file; }), entries.end());
    WriteArchive(archive, entries);
}

static std::string AddSuffixToFileName(const std::string& name, int index) {
    size_t dot = name.find_last_of('.');
    if (dot == std::string::npos) {
        return name + "(" + std::to_string(index) + ")";
    }
    return name.substr(0, dot) + "(" + std::to_string(index) + ")" + name.substr(dot);
}

void MergeArchives(const std::string& a, const std::string& b, const std::string& out) {
    auto ea = ReadArchive(a);
    auto eb = ReadArchive(b);

    for (auto& e : eb) {
        auto it = std::find_if(ea.begin(), ea.end(), [&](const Entry& x){ return x.name == e.name; });

        if (it == ea.end()) {
            ea.push_back(e);
        } 
        else {
            std::cout << "Conflict: " << e.name << "\n" << "1 - merge\n2 - cancel\n3 - keep both\nChoice: ";
            int c; std::cin >> c;
            if (c == 1) {
                it->data.insert(it->data.end(), e.data.begin(), e.data.end());
            }
            else if (c == 3) {
                it->name = AddSuffixToFileName(it->name, 1);
                e.name  = AddSuffixToFileName(e.name, 2);
                ea.push_back(e);
            } 
            else {
                return;
            }
        }
    }
    WriteArchive(out, ea);
}
