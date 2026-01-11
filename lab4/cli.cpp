#include "cli.h"
#include "archive.h"
#include <vector>
#include <string>
#include <stdexcept>

enum class Command { None, Create, List, Extract, Append, Delete, Merge };

static bool startsWith(const std::string& s, const std::string& p) {
    return s.rfind(p, 0) == 0;
}

void parseCLI(int argc, char** argv) {
    Command cmd = Command::None;
    std::string out, a1, a2;
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-c" || arg == "--create") cmd = Command::Create;
        else if (arg == "-l" || arg == "--list") cmd = Command::List;
        else if (arg == "-x" || arg == "--extract") cmd = Command::Extract;
        else if (arg == "-a" || arg == "--append") cmd = Command::Append;
        else if (arg == "-d" || arg == "--delete") cmd = Command::Delete;
        else if (arg == "-A" || arg == "--concatenate") cmd = Command::Merge;

        else if (arg == "-f" || arg == "--file") out = argv[++i];
        else if (startsWith(arg, "--file=")) out = arg.substr(7);

        else {
            if (cmd == Command::Merge && a1.empty()) a1 = arg;
            else if (cmd == Command::Merge && a2.empty()) a2 = arg;
            else files.push_back(arg);
        }
    }

    switch (cmd) {
        case Command::Create: createArchive(out, files); break;
        case Command::List: listArchive(out); break;
        case Command::Extract: extractArchive(out); break;
        case Command::Append: appendFile(out, files.at(0)); break;
        case Command::Delete: deleteFile(out, files.at(0)); break;
        case Command::Merge: mergeArchives(a1, a2, out); break;
        default: throw std::runtime_error("No command");
    }
}
