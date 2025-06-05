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
        map<char, int> next;  // Переходы по символам
        int link = -1;         // Суффиксная ссылка
        int parent;            // Родительский узел
        char parent_char;      // Символ, по которому пришли из родителя
        bool is_terminal = false; // Является ли узел терминальным
        vector<int> pattern_indices; // Индексы шаблонов, которые заканчиваются здесь
    };

    vector<Node> trie;
    vector<string> patterns;

public:
    AhoCorasick() {
        // Создаем корневой узел
        trie.emplace_back();
        trie[0].link = 0;
        trie[0].parent = -1;
    }

    // Добавление шаблона в автомат
    void add_pattern(const string& pattern) {
        int node = 0; // Начинаем с корня
        for (char c : pattern) {
            if (trie[node].next.count(c) == 0) {
                trie[node].next[c] = trie.size();
                trie.emplace_back();
                trie.back().parent = node;
                trie.back().parent_char = c;
            }
            node = trie[node].next[c];
        }
        trie[node].is_terminal = true;
        trie[node].pattern_indices.push_back(patterns.size());
        patterns.push_back(pattern);
    }

    // Построение суффиксных ссылок
    void build_links() {
        queue<int> q;
        q.push(0);

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            // Вычисляем суффиксную ссылку для всех детей
            for (const auto& [c, child] : trie[node].next) {
                q.push(child);
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

    // Поиск всех вхождений шаблонов в тексте
    vector<pair<int, int>> search(const string& text) {
        vector<pair<int, int>> matches; // Пары (индекс шаблона, позиция в тексте)
        int node = 0;

        for (int pos = 0; pos < text.size(); ++pos) {
            char c = text[pos];
            
            // Переходим по суффиксным ссылкам, пока не найдем переход по c
            while (node != 0 && trie[node].next.count(c) == 0) {
                node = trie[node].link;
            }

            if (trie[node].next.count(c)) {
                node = trie[node].next[c];
            }

            // Проверяем текущий узел и все суффиксные ссылки от него
            int current = node;
            while (current != 0) {
                if (trie[current].is_terminal) {
                    for (int pattern_idx : trie[current].pattern_indices) {
                        matches.emplace_back(pattern_idx, pos - patterns[pattern_idx].size() + 1);
                    }
                }
                current = trie[current].link;
            }
        }

        return matches;
    }
};

int main() {
    AhoCorasick ac;
    
    // Добавляем шаблоны для поиска
    vector<string> patterns = {"cat", "tok"};
    for (const auto& pattern : patterns) {
        ac.add_pattern(pattern);
    }
    
    // Строим автомат
    ac.build_links();
    
    // Текст, в котором ищем
    string text = "vcatenoktok";
    
    // Ищем шаблоны
    auto matches = ac.search(text);
    
    // Выводим результаты
    cout << "Текст: " << text << endl;
    cout << "Найденные шаблоны:" << endl;
    for (const auto& [pattern_idx, pos] : matches) {
        cout << "  " << patterns[pattern_idx] << " на позиции " << pos << endl;
    }
    
    return 0;
}