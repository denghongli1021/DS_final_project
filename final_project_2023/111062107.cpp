#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <dirent.h>

using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
    }

    bool search(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                return false; // Character not found in the trie
            }
            current = current->children[ch];
        }
        return true; // Check if the word ends at this node
    }

    bool search_exactly_word(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                return false; // Character not found in the trie
            }
            current = current->children[ch];
        }
        return current->isEndOfWord && current->children.empty(); // Check if the word ends at this node and no more characters after it
    }

    bool search_with_wildcard(const string& pattern) {
        return search_with_wildcard_recursive(root, pattern, 0);
    }

    // 遞迴搜尋支援通配符的內部函式
    bool search_with_wildcard_recursive(TrieNode* current, const string& pattern, int index) {
        if (index == pattern.size()) {
            return current->isEndOfWord;
        }

        char ch = pattern[index];
        cout << "ch : " << ch << " , Index : " << index << endl;
        if (ch == '*') {
            // 如果是通配符 '*'
            for (const auto& child : current->children) {
                if (search_with_wildcard_recursive(child.second, pattern, index) || // 繼續保持在 '*' 的位置
                    search_with_wildcard_recursive(child.second, pattern, index + 1)) { // 移動到 '*' 的下一個位置
                    return true;
                }
            }
            return false;
        } else {
            // 如果是正常字符
            if (current->children.find(ch) == current->children.end()) {
                return false;
            }
            return search_with_wildcard_recursive(current->children[ch], pattern, index + 1);
        }
    }

    void reset() {
        delete_trie(root);
        root = new TrieNode();
    }

    void delete_trie(TrieNode* node) {
        for (auto& child : node->children) {
            delete_trie(child.second);
        }
        delete node;
    }
};

// Utility Func

// string parser: output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string, Trie& trie) {
    vector<string> parse_string;
    for (auto& word : tmp_string) {
        trie.insert(word); // Insert the original word into Trie
        parse_string.emplace_back(word);
    }
    return parse_string;
}

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if ("" == str)
        return res;

    char* strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());

    char* d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char* p = strtok(strs, d);
    while (p) {
        string s = p;
        res.push_back(s);
        p = strtok(NULL, d);
    }

    return res;
}

// Get all files in the given directory
vector<string> get_files_in_directory(const string& directory) {
    vector<string> files;

    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (ent->d_type == DT_REG) {  // Regular file
                files.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }

    return files;
}

int main(int argc, char* argv[]) {

    // INPUT :
    // 1. data directory in data folder
    // 2. query file path
    // 3. output file name

    // Ensure enough command line parameters
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " [input_folder_path] [query_file_path] [output_file_name]" << endl;
        return 1;
    }

    string data_dir = argv[1] + string("/");
    string query_file_path = argv[2];
    string output_file_name = argv[3];

    // Open the output file
    ofstream outputFile(output_file_name);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << output_file_name << endl;
        return 1;
    }

    // Read File & Parser Example

    // Get all files in the data directory
    vector<string> files = get_files_in_directory(data_dir);

    Trie trie; // Create a Trie object
    Trie trie_reversed;
    ifstream queryFile(query_file_path);
    if (!queryFile.is_open()) {
        cerr << "Error opening query file: " << query_file_path << endl;
        return 1;
    }

    string query;
    while (getline(queryFile, query)) {

        cout << "now is search : " << query << endl;
        set<string> titles_found;
        vector<vector<string>> titles;
        // Iterate through all files
        for (const auto& file : files) {
            if (file.size() >= 4 && file.substr(file.size() - 4) == ".txt") { // Ensure that the file name contains at least four characters, including .txt.
                trie.reset(); // Reset the Trie for each query
                trie_reversed.reset();

                // Open the file
                fstream fi(data_dir + file, ios::in);

                string tmp;

                // GET TITLENAME
                vector<string> title,title2;
                if (getline(fi, tmp)) {
                    // GET TITLENAME WORD ARRAY
                    vector<string> tmp_string = split(tmp, " ");
                    vector<string> reversed_tmp = tmp_string;
                    for (string& str : reversed_tmp) {
                        reverse(str.begin(), str.end());
                    }
                    title = word_parse(tmp_string, trie);
                    title2 = word_parse(reversed_tmp , trie_reversed);
                }

                // GET CONTENT LINE BY LINE
                while (getline(fi, tmp)) {
                    // GET CONTENT WORD VECTOR
                    vector<string> tmp_string = split(tmp, " ");
                    vector<string> reversed_tmp = tmp_string;
                    for (string& str : reversed_tmp) {
                        reverse(str.begin(), str.end());
                    }
                    // PARSE CONTENT
                    vector<string> content = word_parse(tmp_string, trie);
                    vector<string> content2 = word_parse(reversed_tmp, trie_reversed);
                    // ......
                }

                // CLOSE FILE
                fi.close();
                // Check if the query is found in the Trie
                if (query[0] == '"') {
                    //cout << "serch way 1" << endl;
                    string tmp;
                    for (auto &c : query) {
                        if (c != '"') {
                            tmp = tmp + c;
                        }
                    }
                    if (trie.search_exactly_word(tmp)) {
                        titles.push_back(title);
                    }
                }
                else if (query[0] == '*') {
                    //cout << "serch way 2" << endl;
                    string tmp;
                    for (auto &c : query) {
                        if (c != '*') {
                            tmp = tmp + c;
                        }
                    }
                    if (trie_reversed.search(tmp)) {
                        titles.push_back(title);
                    }
                }
                else if (query[0] == '<') {
                    //cout << "serch wildcard" << endl;
                    string tmp;
                    for (auto &c : query) {
                        if (c != '>' && c != '<') {
                            tmp = tmp + c;
                        }
                    }
                    if (trie.search_with_wildcard(tmp)) {
                        titles.push_back(title);
                    }
                }
                else {
                    //cout << "serch way 3" << endl;
                    if (trie.search(query)) {
                        titles.push_back(title);
                    }
                }
            }
        }

        // Output the result to the output file
        cout << "Query: " << query << endl;
        if (!titles.empty()) {
            cout << "Titles found: " << endl;
            for (const auto& titleList : titles) {
                for (const auto& t : titleList) {
                    outputFile << t << " ";
                    cout << t << " ";
                }
                outputFile << endl;
                cout << endl;
            }
        } else {
            outputFile << "Not Found!" << endl;
            cout << "Not Found!" << endl;
        }
        cout << "--------------------------" << endl;
    }

    queryFile.close();

    // Close the output file
    outputFile.close();

    return 0;
}
