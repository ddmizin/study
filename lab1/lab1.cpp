#include <iostream>
#include <fstream>

struct Data {
    char DataKey[101];
    char DataValue[101];
};

bool CompareStringPrefix(const char* str, const char* prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (str[i] != prefix[i]) {
            return false;
        }
        i++;
    }
    return true;
}

bool CompareString(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return str1[i] == str2[i];
}

void CopyString(char* new_string, const char* source) {
    int i = 0;
    while (source[i] != '\0') {
        new_string[i] = source[i];
        i++;
    }
    new_string[i] = '\0';
}

int LengthString(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

bool IsSpace(char c) {
    return c == ' ' || c == '\t'  || c == '\n';
}

bool IsValidSymbol(char c) {
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           (c >= '0' && c <= '9') || 
           (c == '_');
}

void DeleteSpaces(char* str) {
    if (str == nullptr) {
        return;
    }

    int l = LengthString(str);
    int start = 0;
    int end = l - 1;

    while (start < l && IsSpace(str[start])) {
        start++;
    }

    while (end >= 0 && IsSpace(str[end])) {
        end--;
    }

    int new_l = end - start + 1;
    if (new_l > 0) {
        for (int i = 0; i < new_l; ++i) {
            str[i] = str[start + i];
        }
    }
    str[new_l] = '\0';
}

const char* FindChar(const char* str, char c) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == c) {
            return str + i;
        }
        ++i;
    }
    return nullptr;
}

const char* FindValue(const Data* Data, int count, const char* key) {
    for (int i = 0; i < count; i++) {
        if (CompareString(Data[i].DataKey, key)) {
            return Data[i].DataValue;
        }
    }
    return nullptr;
}

bool CommandLine(int argc, char* argv[], char*& TemplatePath, char*& DataPath, char*& OutputPath){
    bool template_pr = false;
    bool data_pr = false;
    for (int i = 1; i < argc; ++i){
        if (CompareString(argv[i], "-t")){
            if (i + 1 < argc){
                TemplatePath = argv[++i];
                template_pr = true;
            }
            else {
                return false;
            }
        }
        else if (CompareString(argv[i], "-d")){
            if (i + 1 < argc){
                DataPath = argv[++i];
                data_pr = true;
            }
            else {
                return false;
            }
        }
        else if (CompareString(argv[i], "-o")){
            if (i + 1 < argc){
                OutputPath = argv[++i];
            }
            else {
                return false;
            }
        }
        else if (CompareStringPrefix(argv[i], "--template=")){
            TemplatePath = argv[i] + 11;
            template_pr = true;
        }
        else if (CompareStringPrefix(argv[i], "--data=")){
            DataPath = argv[i] + 7;
            data_pr = true;
        }
        else if (CompareStringPrefix(argv[i], "--output=")){
            OutputPath = argv[i] + 9;
        }
        else {
            return false;
        }
    }
    if (!template_pr || !data_pr){
        return false;
    }
    return TemplatePath != nullptr && DataPath != nullptr;
}

int ReadData(const char* DataPath, Data* Data, int& count){
        std::ifstream data_file(DataPath);
        if (!data_file.is_open()){
            return 3;
        }
        char line[10000];
        count = 0;
        while (data_file.getline(line, 10000)){
            DeleteSpaces(line);
            if (line[0] == '/0'){
                continue;
            }
            if (line[0] == '#'){
                continue;
            }
            if (line[0] == '/' && line[1] == '/'){
                continue;
            }
            const char* separator = FindChar(line, '=');
            if (separator == nullptr){
                continue;
            }
            int key_l = separator - line;
            if (key_l > 100){
                continue;
            }
            char key[101];
            for (int i = 0; i < key_l; ++i){
                key[i] = line[i];
            }
            key[key_l] = '\0';
            DeleteSpaces(key);
            const char* value_start = separator + 1;
            int value_l = LengthString(value_start);
            if (value_l > 100){
                continue;
            }
            char value[100];
            CopyString(value, value_start);
            DeleteSpaces(value);
            if (LengthString(key) == 0 || LengthString(value) == 0){
                continue;
            }
            for (int i = 0; i < key_l; ++i){
                if (!IsValidSymbol(key[i])){
                    continue;
                }
            }
            for (int i = 0; i < value_l; ++i){
                if (!IsValidSymbol(value[i])){
                    continue;
                }
            }
            bool key_repeated = false;
            for (int i = 0; i < count; ++i){
                if (CompareString(key, Data[i].DataKey)){
                    CopyString(Data[i].DataValue, value);
                    key_repeated = true;
                    break;
                }
            }
            if (!key_repeated){
                if (count < 1024){
                    CopyString(Data[count].DataKey, key);
                    CopyString(Data[count].DataValue, value);
                    ++count;
                }
            }
        }
        data_file.close();
        return 0;
}

int Template(const char* TemplatePath, const char* OutputPath, const Data* Data, int count){
    std::ifstream template_file(TemplatePath);
    if (!template_file.is_open()){
        return 3;
    }
    std::ofstream output_file;
    std::ostream* output_stream;
    if (OutputPath != nullptr){
        output_file.open(OutputPath);
        if (!output_file.is_open()){
            return 3;
        }
        output_stream = &output_file;
    }
    else {
        output_stream = &std::cout;
    }
    char curr;
    int pos = 0;
    char key_buffer[101];
    int key_pos = 0;
    while (template_file.get(curr)){
        switch (pos){
            case 0:
                if (curr == '{'){
                    pos = 1;
                }
                else {
                    *output_stream << curr;
                }
                break;
            case 1:
                if (curr == '{'){
                    pos = 2;
                    key_pos = 0;
                }
                else {
                    *output_stream << '{' << curr;
                    pos = 0;
                }
                break;
            case 2:
                if (curr == '}'){
                    pos = 1;
                }
                else if (key_pos < 100){
                    key_buffer[key_pos++] = curr;
                }
                break;
            case 3:
                if (curr == '}'){
                    key_buffer[key_pos] = '\0';
                    DeleteSpaces(key_buffer);
                    const char* value = FindValue(Data, count, key_buffer);
                    if (value == nullptr){
                        template_file.close();
                        if (OutputPath != nullptr){
                            output_file.close();
                        }
                        return 1;
                    }
                    *output_stream << value;
                    pos = 0;
                }
                else {
                    if (key_pos < 100){
                        key_buffer[key_pos++] = '}';
                        key_buffer[key_pos++] = curr;
                    }
                    pos = 2;
                }
                break;    
        }
        if (pos == 1 && curr == '}') {
            pos = 3;
        }
    }
    if (pos != 0){
        template_file.close();
        if (OutputPath != nullptr){
            output_file.close();
        }
        return 4;
    }
    template_file.close();
    if (OutputPath != nullptr){
        output_file.close();
    }
    return 0;

}

int main(int argc, char* argv[]){
    char* template_path = nullptr;
    char* data_path = nullptr;
    char* output_path = nullptr;
    if (!CommandLine(argc, argv, template_path, data_path, output_path)){
        return 2;
    }
    int count = 0;
    Data data[1024];
    int result = ReadData(data_path, data, count);
    if (result != 0){
        return result;
    }
    result = Template(template_path, output_path, data, count);
    return result;
}