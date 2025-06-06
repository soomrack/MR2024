#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>

using namespace std;

class AhoCorasick {
private:
    struct Node {
        map<char, int> next;
        int link = -1;
        int parent;
        char parent_char;
        bool is_terminal = false;
        vector<int> pattern_indices;
    };

    vector<Node> trie;
    vector<string> patterns;

public:
    AhoCorasick();
    void add_pattern(const string& pattern);
    void build_links();
    vector<pair<int, int>> search(const string& text);
};

AhoCorasick::AhoCorasick() {
    trie.emplace_back();
    trie[0].link = 0;
    trie[0].parent = -1;
}

void AhoCorasick::add_pattern(const string& pattern) {
    int node = 0;
    size_t i = 0;
    while (i < pattern.size()) {
        char c = pattern[i];
        if (trie[node].next.count(c) == 0) {
            trie[node].next[c] = trie.size();
            trie.emplace_back();
            trie.back().parent = node;
            trie.back().parent_char = c;
        }
        node = trie[node].next[c];
        i++;
    }
    trie[node].is_terminal = true;
    trie[node].pattern_indices.push_back(patterns.size());
    patterns.push_back(pattern);
}

void AhoCorasick::build_links() {
    queue<int> q;
    q.push(0);

    while (!q.empty()) {
        int node = q.front();
        q.pop();

        map<char, int>::iterator it = trie[node].next.begin();
        while (it != trie[node].next.end()) {
            q.push(it->second);
            it++;
        }

        if (node == 0 || trie[node].parent == 0) {
            trie[node].link = 0;
        } else {
            int parent_link = trie[trie[node].parent].link;
            char c = trie[node].parent_char;
            
            while (parent_link != 0 && trie[parent_link].next.count(c) == 0) {
                parent_link = trie[parent_link].link;
            }

            if (trie[parent_link].next.count(c)) {
                trie[node].link = trie[parent_link].next[c];
            } else {
                trie[node].link = 0;
            }
        }
    }
}

vector<pair<int, int>> AhoCorasick::search(const string& text) {
    vector<pair<int, int>> matches;
    int node = 0;
    int pos = 0;

    while (pos < text.size()) {
        char c = text[pos];
        
        while (node != 0 && trie[node].next.count(c) == 0) {
            node = trie[node].link;
        }

        if (trie[node].next.count(c)) {
            node = trie[node].next[c];
        }

        int current = node;
        while (current != 0) {
            if (trie[current].is_terminal) {
                size_t j = 0;
                while (j < trie[current].pattern_indices.size()) {
                    int pattern_idx = trie[current].pattern_indices[j];
                    matches.emplace_back(pattern_idx, pos - patterns[pattern_idx].size() + 1);
                    j++;
                }
            }
            current = trie[current].link;
        }
        pos++;
    }

    return matches;
}

int main() {
    AhoCorasick ac;
    
    vector<string> patterns = {"he", "she", "his", "hers"};
    size_t i = 0;
    while (i < patterns.size()) {
        ac.add_pattern(patterns[i]);
        i++;
    }
    
    ac.build_links();
    
    string text = "ushers";
    
    auto matches = ac.search(text);
    
    cout << "Текст: " << text << endl;
    cout << "Найденные шаблоны:" << endl;
    i = 0;
    while (i < matches.size()) {
        cout << "  " << patterns[matches[i].first] << " на позиции " << matches[i].second << endl;
        i++;
    }
    
    return 0;
}