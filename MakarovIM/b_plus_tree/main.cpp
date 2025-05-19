#include <iostream>
#include <fstream>
#include "b_plus_tree.hpp"

int main() {
    BPlusTree tree;
    std::ifstream fin("/mnt/c/Users/im.makarov/Desktop/pr/my/practice/cpp_lib/b_plus_tree/test_data.txt");
    if (!fin.is_open()) {
        std::cerr << "Не удалось открыть файл test_data.txt\n";
        return 1;
    }
    long x;
    // считываем числа из файла
    while (fin >> x) {
        tree.insert(x);
    }
    fin.close();

    std::cout << "Структура B+-дерева после вставки:\n";
    tree.print_tree();
    
    // Пример поиска нескольких значений
    long search_keys[] = {17, 100, 5};
    for (long key : search_keys) {
        bool found = tree.search(key);
        std::cout << "Поиск ключа " << key << ": ";
        std::cout << (found ? "найден" : "не найден") << "\n";
    }

    // Удаляем несколько значений и показываем результат
    long remove_keys[] = {17, 5};
    for (long key : remove_keys) {
        std::cout << "Удаление ключа " << key << "\n";
        tree.remove(key);
        tree.print_tree();
    }

    return 0;
}
