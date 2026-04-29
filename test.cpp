#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <string>

using namespace std;

inline uint64_t fnv1a(const string& s) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : s) {
        hash ^= (uint64_t)(unsigned char)c;
        hash *= 1099511628211ULL;
    }
    return hash;
}

struct Entry {
    uint64_t key;
    int count;
    bool active;
};

class CuckooHashTable {
private:
    int M;
    vector<Entry> T1, T2;
    uint64_t seed1, seed2;

    size_t h1(uint64_t key) const {
        uint64_t x = key ^ seed1;
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        x ^= x >> 33;
        return x & (M - 1);
    }

    size_t h2(uint64_t key) const {
        uint64_t x = key ^ seed2;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ULL;
        x ^= x >> 33;
        return x & (M - 1);
    }

    void rehash() {
        vector<Entry> oldT1 = move(T1);
        vector<Entry> oldT2 = move(T2);
        
        M *= 2;
        T1.assign(M, {0, 0, false});
        T2.assign(M, {0, 0, false});
        
        seed1 ^= 0x123456789ABCDEF0ULL;
        seed2 ^= 0xFEDCBA9876543210ULL;
        
        for (const auto& e : oldT1) if (e.active) insert_internal(e.key, e.count);
        for (const auto& e : oldT2) if (e.active) insert_internal(e.key, e.count);
    }

    void insert_internal(uint64_t key, int count) {
        size_t p1 = h1(key);
        if (!T1[p1].active) { T1[p1] = {key, count, true}; return; }
        
        size_t p2 = h2(key);
        if (!T2[p2].active) { T2[p2] = {key, count, true}; return; }

        Entry cur = {key, count, true};
        for (int i = 0; i < 1000; ++i) {
            p1 = h1(cur.key);
            swap(cur, T1[p1]);
            if (!cur.active) return;
            
            p2 = h2(cur.key);
            swap(cur, T2[p2]);
            if (!cur.active) return;
        }
        
        rehash();
        insert_internal(cur.key, cur.count);
    }

public:
    CuckooHashTable(int initial_power_of_two = 524288) {
        M = initial_power_of_two;
        T1.assign(M, {0, 0, false});
        T2.assign(M, {0, 0, false});
        seed1 = 0x811c9dc5ULL;
        seed2 = 0x1a2b3c4dULL;
    }

    void add(uint64_t key) {
        size_t p1 = h1(key);
        if (T1[p1].active && T1[p1].key == key) {
            T1[p1].count++;
            return;
        }
        size_t p2 = h2(key);
        if (T2[p2].active && T2[p2].key == key) {
            T2[p2].count++;
            return;
        }
        insert_internal(key, 1);
    }

    int get(uint64_t key) const {
        size_t p1 = h1(key);
        if (T1[p1].active && T1[p1].key == key) return T1[p1].count;
        size_t p2 = h2(key);
        if (T2[p2].active && T2[p2].key == key) return T2[p2].count;
        return 0;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (!(cin >> n)) return 0;

    vector<uint64_t> A(n), B(n), C(n);
    CuckooHashTable banMachine;

    for (int i = 0; i < n; ++i) {
        string s; cin >> s;
        A[i] = fnv1a(s);
        banMachine.add(A[i]);
    }

    for (int i = 0; i < n; ++i) {
        string s; cin >> s;
        B[i] = fnv1a(s);
        banMachine.add(B[i]);
    }

    for (int i = 0; i < n; ++i) {
        string s; cin >> s;
        C[i] = fnv1a(s);
        banMachine.add(C[i]);
    }

    auto calculate_score = [&](const vector<uint64_t>& files) {
        int score = 0;
        for (int i = 0; i < n; ++i) {
            int cnt = banMachine.get(files[i]);
            if (cnt == 1) score += 3;
            else if (cnt == 2) score += 1;
        }
        return score;
    };

    int scoreA = calculate_score(A);
    int scoreB = calculate_score(B);
    int scoreC = calculate_score(C);

    cout << scoreA << " " << scoreB << " " << scoreC << "\n";

    return 0;
}