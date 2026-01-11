#include "utils.h"
#include <filesystem>

void ensureDirectoryExists(const std::string& filePath) {
    namespace fs = std::filesystem;
    fs::path p(filePath);
    fs::path dir = p.parent_path();

    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
}
