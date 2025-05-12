#include <iostream>

using namespace std;


// создание структуры узла
struct Node {
    int x; 
    Node *left; //указатель левого поддерева
    Node *right; //указатель правого поддерева
    
    Node (int key)
    {
        x = key;
        left = nullptr;
        right = nullptr;
    }

    ~Node() 
    {
        delete left;
        delete right;
    }

};


//существование элемента в дереве
bool exist(Node * root, int y)
{
    while (root != nullptr) {
        if (root->x == y) return true;
        root = (y < root->x) ? root->left : root->right;
    }
    return false;

}


//добавление элемента в дерево
Node * insert (Node * root, int y)
{
    if (exist(root, y)) return root;

    if (root == nullptr) return new Node(y);
    
    if (y < root->x) {
        root->left = insert(root->left, y); 
    } 
    
    else if (y > root->x) {
        root->right = insert(root->right, y);
    }

    return root;
}

//расчет глубины дерева
int depth (Node * root)
{
    if (root == nullptr) return 0;
    return max(depth(root->right), depth(root->left)) + 1;
}

// удаление узла
Node* deleteNode(Node* root, int data) {
    if (root == nullptr) return root;
    
    
    // Поиск узла для удаления
    if (data < root->x) {
        root->left = deleteNode(root->left, data);
    } 
    else if (data > root->x) {
        root->right = deleteNode(root->right, data);
    } 
    else {
        // Узел с единственной дочерней нулевой ветви или лист
        if (root->left == nullptr) {
            Node *temp = root->right;
            root->right = nullptr;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            Node *temp = root->left;
            root->left = nullptr;
            delete root;
            return temp;
        }

        // У узла два дочерних узла, взять минимальное значение в правом поддереве
        Node *temp = root->right;
        while (temp && temp->left != nullptr) {
            temp = temp->left;
        }

        // заменить значение удаляемого узла значением найденного узла
        root->x = temp->x;

        // удалить узел
        root->right = deleteNode(root->right, temp->x);
    }

    return root;
}


void test_performance() {
    // Просто тестируем разных размера
    int sizes[] = {100, 1000, 5000, 10000, 15000, 30000,
        35000, 40000, 45000, 50000, 55000, 60000};
    
    cout << "Performance test:\n";

    // Цикл по 4 тестовым размерам (0-3)
    for(int i = 0; i < 12; i++) {
        // Создаем пустое дерево для теста
        Node* tree = nullptr;

        int size = sizes[i];
        
        // Запоминаем начальное время
        clock_t start = clock();
        
        // Вставляем элементы
        for(int j = 0; j < size; j++) {
            // Вставляем случайное число
            tree = insert(tree, rand() % size);
        }
        
        // Запоминаем конечное время
        clock_t end = clock();
        
        // Просто выводим результаты
        cout << size << " elements: " 
             << (double)(end - start)/CLOCKS_PER_SEC * 1000 
             << " ms\n";
             
        delete tree;
    }
}


//вывод
void print(Node * root)
{
    if (root == nullptr) return;

    print(root->left);

    cout << root->x << " ";

    print(root->right);
  
}

int main(){

    Node* root = nullptr;

    //вставка элементво в дерево
    root = insert(root, 1344);
    root = insert(root, -20);
    root = insert(root, 44);
    root = insert(root, 17);
    root = insert(root, 9);
    root = insert(root, -1);
    root = insert(root, -110);
    root = insert(root, 912);
    root = insert(root, 2000);
    

    std::cout<<"listya "; 
    print(root);
    cout<<endl;
 
    int tree_depth = depth(root);
    std::cout<<"depth "<< tree_depth << endl;
 
    
     root = deleteNode(root, 2000);
     std::cout << "";
     print(root);
     cout << endl;

     // Запуск теста производительности
    cout << "\nTest\n";
    test_performance();
   
    delete root;
 
    return 0;

}
