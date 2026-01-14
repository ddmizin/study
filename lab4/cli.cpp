#include "cli.h"
#include "archive.h"
#include <vector>
#include <string>
#include <stdexcept>

enum class Command { 
    kNone, 
    kCreate, 
    kList, 
    kExtract, 
    kAppend, 
    kDelete, 
    kMerge 
};

static bool StartsWith(const std::string& s, const std::string& p) {
    return s.rfind(p, 0) == 0;
}

void ParseCLI(int argc, char** argv) {
    Command cmd = Command::kNone;
    std::string out, a1, a2;
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-c" || arg == "--create") {
            cmd = Command::kCreate;
        }
        else if (arg == "-l" || arg == "--list") {
            cmd = Command::kList;
        }
        else if (arg == "-x" || arg == "--extract") {
            cmd = Command::kExtract;
        }
        else if (arg == "-a" || arg == "--append") {
            cmd = Command::kAppend;
        }
        else if (arg == "-d" || arg == "--delete") {
            cmd = Command::kDelete;
        }
        else if (arg == "-A" || arg == "--concatenate") {
            cmd = Command::kMerge;
        }
        else if (arg == "-f" || arg == "--file") {
            out = argv[++i];
        }
        else if (StartsWith(arg, "--file=")) {
            out = arg.substr(7);
        }
        else {
            if (cmd == Command::kMerge && a1.empty()) {
                a1 = arg;
            }
            else if (cmd == Command::kMerge && a2.empty()) {
                a2 = arg;
            }
            else {
                files.push_back(arg);
            }
        }
    }

    switch (cmd) {
        case Command::kCreate: CreateArchive(out, files); break;
        case Command::kList: ListArchive(out); break;
        case Command::kExtract: ExtractArchive(out, files); break;
        case Command::kAppend: AppendFile(out, files.at(0)); break;
        case Command::kDelete: DeleteFile(out, files.at(0)); break;
        case Command::kMerge: MergeArchives(a1, a2, out); break;
        default: throw std::runtime_error("No command");
    }
}
