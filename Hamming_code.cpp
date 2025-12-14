#include <iostream>
#include <vector>
#include <string>
#include <utility>

// открыть в терминале Ubuntu(WSL)
// перед использованием необходимо сгенерировать исполняемый файл командой 'make'
// после использование необходимо удалить исполняемый файл командой 'make clean'

static bool isPowerOfTwo(int x) {
    return x && !(x & (x - 1));
}

static int calcParityBits(int n) {
    int r = 0;
    while ((1 << r) < n + 1) r++;
    return r;
}

static std::string encodeHamming(int m, int n, const std::string &data) {
    int r = n - m;
    std::vector<int> code(n + 1, 0);

    for (int i = 1, j = 0; i <= n; i++) {
        if (!isPowerOfTwo(i)) {
            code[i] = data[j++] - '0';
        }
    }

    for (int i = 0; i < r; i++) {
        int p = 1 << i;
        int parity = 0;
        for (int j = 1; j <= n; j++) {
            if (j & p) parity ^= code[j];
        }
        code[p] = parity;
    }

    std::string res;
    for (int i = 1; i <= n; i++) res.push_back(char('0' + code[i]));
    return res;
}

static std::pair<int, std::string> decodeHamming(int n, const std::string &vec) {
    int r = calcParityBits(n);
    std::vector<int> code(n + 1);
    for (int i = 1; i <= n; i++) code[i] = vec[i - 1] - '0';

    int syndrome = 0;
    for (int i = 0; i < r; i++) {
        int p = 1 << i;
        int parity = 0;
        for (int j = 1; j <= n; j++) {
            if (j & p) parity ^= code[j];
        }
        if (parity) syndrome |= p;
    }

    if (syndrome >= 1 && syndrome <= n) {
        code[syndrome] ^= 1;
    }

    std::string data;
    for (int i = 1; i <= n; i++) {
        if (!isPowerOfTwo(i)) data.push_back(char('0' + code[i]));
    }

    return {r, data};
}

static bool isValidHamming(int n, const std::string &vec) {
    int r = calcParityBits(n);
    std::vector<int> code(n + 1);
    for (int i = 1; i <= n; i++) code[i] = vec[i - 1] - '0';

    for (int i = 0; i < r; i++) {
        int p = 1 << i;
        int parity = 0;
        for (int j = 1; j <= n; j++) {
            if (j & p) parity ^= code[j];
        }
        if (parity) return false;
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    std::string cmd = argv[1];

    if (cmd == "encode") {
        int m = std::stoi(argv[2]);
        int n = std::stoi(argv[3]);
        std::string data = argv[4];
        std::cout << encodeHamming(m, n, data) << '\n';
    } else if (cmd == "decode") {
        int n = std::stoi(argv[2]);
        std::string vec = argv[3];
        auto res = decodeHamming(n, vec);
        std::cout << res.first << '\n' << res.second << '\n';
    } else if (cmd == "is_valid") {
        int n = std::stoi(argv[2]);
        std::string vec = argv[3];
        std::cout << (isValidHamming(n, vec) ? 1 : 0) << '\n';
    }
    return 0;
}
