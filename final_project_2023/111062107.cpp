#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
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
        return current->isEndOfWord; // Check if the word ends at this node and no more characters after it
    }

    bool wildcard_search(TrieNode* node, const std::string& pattern, size_t index) {
        if (index == pattern.size()) {
            return node->isEndOfWord;
        }

        char current_char = pattern[index];
        if (current_char == '*') {
            // Handle '*' case
            for (const auto& child : node->children) {
                // Recursively search for the rest of the pattern
                if (wildcard_search(child.second, pattern, index) || wildcard_search(child.second, pattern, index + 1)) {
                    return true;
                }
            }

            // If '*' can match an empty sequence, search without consuming '*'
            return wildcard_search(node, pattern, index + 1);
        } else {
            // Handle non-'*' case
            if (node->children.find(current_char) != node->children.end()) {
                return wildcard_search(node->children[current_char], pattern, index + 1);
            }
        }

        return false;
    }

    bool wildcard_search_trie(const std::string& pattern) {
        return wildcard_search(this->root, pattern, 0);
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

Trie tries[10000];
Trie tries_reversed[10000];
vector<vector<string>> Titles;
// Utility Func

// string parser: output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string, Trie& trie) {
    vector<string> parse_string;
    for (auto& word : tmp_string) {
        //transform(word.begin(), word.end(), word.begin(), ::tolower);
        trie.insert(word); // Insert the original word into Trie
        parse_string.emplace_back(word);
    }
    return parse_string;
}
vector<string> word_parse2(vector<string> tmp_string, Trie& trie) {
    vector<string> parse_string;
    for (auto& word : tmp_string) {
        // 保留單詞中的英文字母
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) { return !std::isalpha(c); }), word.end());

        // 轉換為小寫
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        //cout << word << endl;
        trie.insert(word); // 將原始單詞插入 Trie
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
bool compare_filenames(const string& a, const string& b) {
    return stoi(a) < stoi(b);
}
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

bool calculate (Trie& trie,Trie& trie_reversed,const string& query) {
    if (query[0] == '"') {
        //cout << "serch way 1" << endl;
        string tmp;
        for (auto &c : query) {
            if ( ((c >= 'a' && c <= 'z')) ) {
                tmp = tmp + c;
            }
        }
//        string tmp2 = tmp;
//        reverse(tmp2.begin(), tmp2.end()); || trie_reversed.search(tmp2)
        if (trie.search_exactly_word(tmp)) {
            return true;
            //titles.push_back(title);
        }
        else {
            return false;
        }
    }
    /*
    Exact Search: “search-word”
    Eg: we want to search essay with graph, we use query - “graph”
    */
    else if (query[0] == '*') {
        //cout << "serch way 2" << endl;
        string tmp;
        for (auto &c : query) {
            if (((c >= 'a' && c <= 'z'))) {
                tmp = tmp + c;
            }
        }
        reverse(tmp.begin(), tmp.end());
        if (trie_reversed.search(tmp)) {
            return true;
            //titles.push_back(title);
        }
        else {
            return false;
        }
    }
    /*
    Suffix Search: *search-word*
    Eg: we want to search essay with suffix graph, we use query - *graph*
    */
    else if (query[0] == '<') {
        //cout << "serch wildcard" << endl;
        string tmp;
        for (auto &c : query) {
            if (((c >= 'a' && c <= 'z') || c == '*')) {
                tmp = tmp + c;
            }
        }
        if (trie.wildcard_search_trie(tmp)) {
            return true;
            //titles.push_back(title);
        }
        else {
            return false;
        }
    }
    /*
    Wildcard Search: <search-pattern>
    Eg: we want to search essay with word pattern gr*h, we use query - <gr*h>. “*”
    can be empty, single or multiple characters, so gr*h should match words like graph,
    growth…etc.
    */
    else {
        //cout << "serch way 3" << endl;
        string tmp;
        for (auto &c : query) {
            if (((c >= 'a' && c <= 'z') )) {
                tmp = tmp + c;
            }
        }
        if (trie.search(tmp)) {
            return true;
            //titles.push_back(title);
        }
        else {
            return false;
        }
    }
    /*
    Prefix Search: search-word
    Eg: we want to search essay with prefix graph, we use query - graph
    */
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
    sort(files.begin(), files.end(), compare_filenames);
    ifstream queryFile(query_file_path);
    if (!queryFile.is_open()) {
        cerr << "Error opening query file: " << query_file_path << endl;
        return 1;
    }
    Trie trie;
    Trie trie_reversed;
    int cnt = 0;
    for (const auto& file : files) {
        if (file.size() >= 4 && file.substr(file.size() - 4) == ".txt") { // Ensure that the file name contains at least four characters, including .txt.
            Trie trie;
            Trie trie_reversed;
            // Open the file
            fstream fi(data_dir + file, ios::in);

            string tmp;
            // GET TITLENAME
            vector<string> title,title2,title3,title4;
            if (getline(fi, tmp)) {
                // GET TITLENAME WORD ARRAY
                vector<string> tmp_string = split(tmp, " ");
                vector<string> reversed_tmp = tmp_string;
                for (string& str : reversed_tmp) {
                    reverse(str.begin(), str.end());
                }
                title = word_parse(tmp_string, trie);
                Titles.push_back(title);
                title2 = word_parse(reversed_tmp , trie_reversed);
                title3 = word_parse2(tmp_string, trie);
                title4 = word_parse2(reversed_tmp , trie_reversed);
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
                vector<string> content = word_parse2(tmp_string, trie);
                vector<string> content2 = word_parse2(reversed_tmp, trie_reversed);
                // ......
            }
            tries[cnt] = trie;
            tries_reversed[cnt] = trie_reversed;
            cnt ++ ;
        }
    }
    string query;
    while (getline(queryFile, query)) {
        queue<int> op;
        for (int i=0;i<query.size();i++) {
            if (query[i] == '+') { // and
                op.push(1);
            }
            else if (query[i] == '/') { // or
                op.push(2);
            }
            else if (query[i] == '-') { //xor
                op.push(3);
            }
        }
        //cout << "now is search : " << query << endl;
        set<string> titles_found;
        vector<vector<string>> titles;
        // Iterate through all files

        // Check if the query is found in the Trie
        for (int k=0;k<cnt;k++) {
            string tmmp;
            bool valid = 0;
            int i=0;
            for (i;i<query.size();i++) {
                if (query[i] == '+' || query[i] == '-' || query[i] == '/') {
                    break;
                }
                else if (query[i] != ' ') {
                    tmmp = tmmp + query[i];
                }
                else continue;
            }
            //cout << "tmmp 1st¡G" << tmmp << endl;
            valid = calculate(tries[k],tries_reversed[k],tmmp);
            tmmp.clear();
            queue<int> op_use = op;
            for (i;i<query.size();i++) {
                if (query[i] == '+' || query[i] == '-' || query[i] == '/') {

                    for (int j=i+2;j<query.size();j++) {
                        if (query[j] != ' ') {

                            tmmp = tmmp + query[j];
                        }
                        if (query[j] == ' '|| j == query.size()-1) {
                            //cout << "in\n";
                            bool valid2;
                            valid2 =  calculate(tries[k],tries_reversed[k],tmmp);
                            int a = op_use.front();
                            //cout << "a:" <<a << endl;
                            op_use.pop();
                            if (a == 1) {
                                valid = valid & valid2;
                            }
                            else if (a == 2) {
                                valid = valid | valid2;
                            }
                            else if (a == 3) {
                                valid = valid & !valid2;
                            }
                            //cout << "tmmp" << tmmp << endl;
                            tmmp.clear();
                            i = j;
                            break;
                        }
                    }
                }
                else continue;
            }
            //cout << "valid ¡G" << valid << endl;
            if (valid) {
                titles.push_back(Titles[k]);
                ///cout << "file num:" << k << endl;
            }
        }
        // Output the result to the output file
        ///cout << "Query: " << query << endl;
        if (!titles.empty()) {
            ///cout << "Titles found: " << endl;
            for (const auto& titleList : titles) {
                for (auto it = titleList.begin(); it != titleList.end(); ++it) {
                    outputFile << *it;
                    ///cout << *it;
                    if (next(it) != titleList.end()) {
                        outputFile << " ";
                    }
                }
                outputFile << endl;
                ///cout << endl;
            }
        } else {
            outputFile << "Not Found!" << endl;
            ///cout << "Not Found!" << endl;
        }
    }

    queryFile.close();

    // Close the output file
    outputFile.close();

    return 0;
}
