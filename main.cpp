#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>


void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
    }
}

bool isValidWord(const char* word) {
    for (int i = 0; word[i]; i++) {
        if (!((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z'))) {
            return false;
        }
    }
    return true;
}

bool caseInsensitiveCompare(const char* str1, const char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 != len2) return false;
    
    for (int i = 0; i < len1; i++) {
        char c1 = str1[i];
        char c2 = str2[i];
        
        if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
        if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
        
        if (c1 != c2) return false;
    }
    return true;
}


int countWordsOfLength(const char* filename, int targetLength) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return -1;
    }
    
    char word[101];
    int count = 0;
    
    while (file >> word) {
        if (isValidWord(word) && strlen(word) == targetLength) {
            count++;
        }
    }
    
    file.close();
    return count;
}


bool getRandomWord(const char* filename, int targetLength, int wordIndex, char* result) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    char word[101];
    int currentIndex = 0;
    
    while (file >> word) {
        if (isValidWord(word) && strlen(word) == targetLength) {
            if (currentIndex == wordIndex) {
                strcpy(result, word);
                toLowerCase(result);
                file.close();
                return true;
            }
            currentIndex++;
        }
    }
    
    file.close();
    return false;
}


void analyzeGuess(const char* secret, const char* guess, char* result) {
    int length = strlen(secret);
    bool* used = new bool[length](); 
    
    for (int i = 0; i < length; i++) {
        if (guess[i] == secret[i]) {
            result[i] = '+';
            used[i] = true;
        } else {
            result[i] = '-';
        }
    }
    
  
    for (int i = 0; i < length; i++) {
        if (result[i] != '+') {
            for (int j = 0; j < length; j++) {
                if (!used[j] && guess[i] == secret[j]) {
                    result[i] = '=';
                    used[j] = true;
                    break;
                }
            }
        }
    }
    
    result[length] = '\0';
    delete[] used;
}

int main(int argc, char* argv[]) {
    const char* filename = argv[1];
    int wordLength = std::atoi(argv[2]);
    int wordCount = countWordsOfLength(filename, wordLength);

    std::srand(std::time(nullptr));
    
   
    int randomIndex = std::rand() % wordCount;
    char secretWord[101];
    char guess[101];
    char result[101];
    bool won = false;
    
    for (int attempt = 0; attempt < 6; attempt++) {
        std::cin >> guess;
        if (!isValidWord(guess)) {
            std::cout << "Invalid input" << std::endl;
            attempt--; 
            continue;
        }
        
        toLowerCase(guess);
        
        
        if (strlen(guess) != wordLength) {
            std::cout << "Error";
            attempt--; 
            continue;
        }
        
        
        analyzeGuess(secretWord, guess, result);
        std::cout << result << std::endl;
        
        
        if (caseInsensitiveCompare(guess, secretWord)) {
            std::cout << "win!" << std::endl;
            won = true;
            break;
        }
    }
    

    if (!won) {
        std::cout << "The word was: " << secretWord << std::endl;
    }
    
    return 0;
}