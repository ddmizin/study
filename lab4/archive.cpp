#include "archive.h"
#include "hamming.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

static const char SIGNATURE[] = "HAF2026";

static std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}

static void writeFile(const std::string& name, const std::vector<uint8_t>& data) {
    ensureDirectoryExists(name);
    std::ofstream out(name, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}

static std::vector<Entry> readArchive(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open archive");
    }
    char sig[8]{};
    in.read(sig, 7);
    if (std::string(sig) != SIGNATURE) {
        throw std::runtime_error("Invalid archive format");
    }
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    std::vector<Entry> entries;
    for (size_t i = 0; i < count; ++i) {
        Entry e;
        size_t nameLen, dataLen;

        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        e.name.resize(nameLen);
        in.read(e.name.data(), nameLen);

        in.read(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));
        std::vector<uint8_t> encoded(dataLen);
        in.read(reinterpret_cast<char*>(encoded.data()), dataLen);

        if (!decodeHamming(encoded)) {
            throw std::runtime_error("Archive corrupted");
        }
        e.data = encoded;
        entries.push_back(e);
    }
    return entries;
}

static void writeArchive(const std::string& path, const std::vector<Entry>& entries) {
    ensureDirectoryExists(path);
    std::ofstream out(path, std::ios::binary | std::ios::trunc);

    out.write(SIGNATURE, 7);
    size_t count = entries.size();
    out.write(reinterpret_cast<char*>(&count), sizeof(count));

    for (const auto& e : entries) {
        auto encoded = encodeHamming(e.data);
        size_t nameLen = e.name.size();
        size_t dataLen = encoded.size();

        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        out.write(e.name.data(), nameLen);
        out.write(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));
        out.write(reinterpret_cast<char*>(encoded.data()), dataLen);
    }
}

void createArchive(const std::string& archive, const std::vector<std::string>& files) {
    std::vector<Entry> entries;
    for (const auto& f : files) {
        entries.push_back({f, readFile(f)});
    }
    writeArchive(archive, entries);
}

void listArchive(const std::string& archive) {
    for (const auto& e : readArchive(archive)) {
        std::cout << e.name << "\n";
    }
}

void extractArchive(const std::string& archive, const std::vector<std::string>& files) {
    auto entries = readArchive(archive);
    if (files.empty()) {
        for (const auto& e : entries) {
            writeFile(e.name, e.data);
        }
        return;
    }

    for (const auto& name : files) {
        auto it = std::find_if(entries.begin(), entries.end(), [&](const Entry& e) { return e.name == name; });
        if (it != entries.end()) {
            writeFile(it->name, it->data);
        } 
        else {
            std::cout << "File not found in archive: " << name << "\n";
        }
    }
}

void appendFile(const std::string& archive, const std::string& file) {
    auto entries = readArchive(archive);
    auto data = readFile(file);
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
    writeArchive(archive, entries);
}

void deleteFile(const std::string& archive, const std::string& file) {
    auto entries = readArchive(archive);
    entries.erase(std::remove_if(entries.begin(), entries.end(), [&](const Entry& e){ return e.name == file; }), entries.end());
    writeArchive(archive, entries);
}

static std::string addSuffixToFilename(const std::string& name, int index) {
    size_t dot = name.find_last_of('.');
    if (dot == std::string::npos) {
        return name + "(" + std::to_string(index) + ")";
    }
    return name.substr(0, dot) + "(" + std::to_string(index) + ")" + name.substr(dot);
}

void mergeArchives(const std::string& a, const std::string& b, const std::string& out) {
    auto ea = readArchive(a);
    auto eb = readArchive(b);

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
                it->name = addSuffixToFilename(it->name, 1);
                e.name  = addSuffixToFilename(e.name, 2);
                ea.push_back(e);
            } 
            else {
                return;
            }
        }
    }
    writeArchive(out, ea);
}
