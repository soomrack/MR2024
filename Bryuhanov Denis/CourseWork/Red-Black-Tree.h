#include <cstdlib>
#include <cstring>
#include <cassert>
#include <queue>
#include <vector>
#include <iostream>

enum Color {
    BLACK,
    RED
};


struct Node {
	int    key;
	int    val;
	Color	color;
	Node	*parent;
	Node	*left;
	Node	*right;

	Node():parent(nullptr),left(nullptr),right(nullptr),color(RED){}
	~Node()
   	{
        	if (left != nullptr)
        	    delete left;
        	if (right != nullptr)
        	    delete right;
    	}
};


class Tree {
	public:
		Tree():root(nullptr),size(0){};
		~Tree();
		void	insert(int &key, int &val);
		bool	remove(const int &key);
		bool	search(const int &key, int &val) const;
        void	clear();
		void	printTree() const;
		int     getSize() const;
	private:
		int   size;
		Node *root;
		
		int 	cmp(const int &a, const int &b) const;
		void	leftRotate(Node *node);	
		void	rightRotate(Node *node);
		void	removeNode(Node *node);
};


Tree::~Tree()
{
	if (root != nullptr)
		delete root;
}


void Tree::insert(int &key, int &val)
{
	Node *node = new Node; // Заменил auto
	node->key = key;
	node->val = val;
	
	if (root == nullptr) {
		root = node;
		node->color = BLACK;
        this->size++;
		return;
	}

	Node *curr = root;
	while (curr->left != nullptr | curr->right != nullptr)
	{
		if (cmp(key, curr->key) == -1)
			curr = curr->left;
		else
			curr = curr->right;
	}
	
	node->parent = curr;
	if (cmp(key, curr->key) == -1)
		curr->left = node;
	else
		curr->right = node;

	while (curr->color == RED && curr->parent != nullptr)
	{
		bool isRight = (curr == curr->parent->right);
        	Node *uncle;
        	if (isRight)
       	        	uncle = curr->parent->left;
       	 	else
            		uncle = curr->parent->right;

        	if (uncle == nullptr) {
            		curr->color = BLACK;
            		curr->parent->color = RED; 
            		if (uncle == curr->parent->right) {
                		rightRotate(curr->parent);
            		}else {
                		leftRotate(curr->parent);
            		}
            		break;
        	}else if (uncle->color == RED) {
			curr->color = BLACK;
			uncle->color = BLACK;
      			curr->parent->color = RED;
			curr = curr->parent;
		}else {
			curr->color = BLACK;
			curr->parent->color = RED;

			if (isRight) {
				if (node == curr->left) {
					rightRotate(curr);
					curr = node;
				}
				leftRotate(curr->parent);
			}else {
				if (node == curr->right) {
					leftRotate(curr);
					curr = node;
				}
				rightRotate(curr->parent);
			}
		}
    		root->color = BLACK;
	}

	this->size++;
}


bool Tree::remove(const int &key)
{  
	Node *curr = root; // Заменил auto
	while (curr->left != nullptr | curr->right != nullptr)
	{
		if (curr->key == key)
			break;

		if (cmp(key, curr->key) >= 0)
			curr = curr->right;
		else
			curr = curr->left;
	}
	if (curr->key != key)
		return 0;
	
  	this->removeNode(curr);
  	(this->size)--;
    	return 1;
}


void Tree::removeNode(Node *node)
{
	if (node->color == RED) {
		if (node->left != nullptr && node->right != nullptr) {
            		Node *successor = node->right; // Заменил auto
            		while (successor->left != nullptr)
                		successor = successor->left;
            		node->key = successor->key;
            		node->val = successor->val;
            		this->removeNode(successor);
        	}else if (node->left != nullptr) {
            		node->key = node->left->key;
            		node->val = node->left->val;
            		node->color = node->left->color;
           		this->removeNode(node->left);
        	}else if (node->right != nullptr) {
            		node->key = node->right->key;
            		node->val = node->right->val;
            		node->color = node->right->color;        
            		this->removeNode(node->right);
        	}else {
            		if (node->parent == nullptr) {
                		free(node);
                		root = nullptr;
				return;
            		}

                	if (node->parent->left == node)
                		node->parent->left = nullptr;
                	else
                    		node->parent->right = nullptr;
                	
			free(node);
		}
 	}else {
        	if (node->left != nullptr && node->right != nullptr) {
        		Node *successor = node->right; // Заменил auto
            		while (successor->left != nullptr)
                		successor = successor->left;
            		node->key = successor->key;
            		node->val = successor->val;
            		this->removeNode(successor);
        	}else if (node->left != nullptr) {
            		node->key = node->left->key;
            		node->val = node->left->val;
            		this->removeNode(node->left);
        	}else if (node->right != nullptr) {
            		node->key = node->right->key;
            		node->val = node->right->val;
            		this->removeNode(node->right);
        	}else {
            		if (node->parent == nullptr) {
                		free(node);
                		root = nullptr;
            			return;
			}

                	if (node->parent->left == node) {
                    		node->parent->left = nullptr;    
                    		if (node->parent->right != nullptr
                        	&& node->parent->right->color == RED) {
                        		node->parent->right->color = BLACK;
                        		leftRotate(node->parent);
                    		}
                	}else {
                		node->parent->right = nullptr;
                    		if (node->parent->left != nullptr
                        	&& node->parent->left->color == RED) {
                        		node->parent->left->color = BLACK;
                        		rightRotate(node->parent);
                    		}
                	}

                	if (node->parent->left == nullptr
                    	&& node->parent->right == nullptr
                    	&& node->parent->parent != nullptr) {
                    		rightRotate(node->parent->parent);
                	}

                	free(node);
        	}    
	}
}


bool Tree::search(const int &key, int &val) const
{
	Node *curr = root; // Заменил auto
	while (curr->left != nullptr || curr->right != nullptr)
	{
		if (curr->key == key) {
			val = curr->val;
			break;
		}

		if (cmp(key, curr->key) < 0)
			curr = curr->left;
        else
			curr = curr->right;
	}
    
	if (curr->key == key){
        val = curr->val;
    	return 1;
    }
    return 0;
}


int Tree::cmp(const int &a, const int &b) const
{
  	if (a < b) return -1;
	if (a == b) return 0;
	return 1;
}


void Tree::leftRotate(Node *node)
{
	assert( node->right != nullptr);
	Node *temp = node->right; // Заменил auto

  	node->right = temp->left;
  	if (temp->left != nullptr)
  		temp->left->parent = node;
	temp->left = node;
	temp->parent = node->parent;
	node->parent = temp;

  	if (root == node) {
  		root = temp;
		return;
 	}
	if (temp->parent->left == node)
    		temp->parent->left = temp;
       	else
           	temp->parent->right = temp;
}


void Tree::rightRotate(Node *node)
{
	assert( node->left != nullptr);
	Node *temp = node->left; // Заменил auto

  	node->left = temp->right;
	if (temp->right != nullptr)
    		temp->right->parent = node;
	temp->right = node;
	temp->parent = node->parent;
	node->parent = temp;

	if (root == node) {
		root = temp;
		return;
	}
	if (temp->parent->left == node)
		temp->parent->left = temp;
	else
      		temp->parent->right = temp;
}


void Tree::printTree() const
{
	std::cout << "----------------" << std::endl;
	std::queue<Node*> q;
	q.push(root);
	while (!q.empty())
	{
		Node *top = q.front(); // Заменил auto
		q.pop();
		if (top->color == RED)
			std::cout << "R" ;
		else
			std::cout << "B" ;
		std::cout << top->key;
		std::cout << " ";
		if (top->left != nullptr) {
			q.push(top->left);
			if (top->left->color == RED)
				std::cout << "R" ;
			else
				std::cout << "B" ;
			std::cout << top->left->key;
			std::cout << " ";
		}else {
			std::cout << "NULL" << " ";
		}
		if (top->right != nullptr) {
			q.push(top->right);
			if (top->right->color == RED)
				std::cout << "R" ;
			else
				std::cout << "B" ;
			std::cout << top->right->key;
			std::cout << " ";
		}else {
			std::cout << "NULL" << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << "----------------" << std::endl;
}


int Tree::getSize() const
{
	return this->size;
}


void Tree::clear()
{
	delete this->root;
	this->root = nullptr;
    this->size = 0;
}
