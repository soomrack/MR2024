#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <random>
#include <sstream>
#include <cmath>
#include <iomanip>


// Типы токенов
enum class TokenType { NUMBER, OPERATOR, LEFT_PAREN, RIGHT_PAREN };


// Класс для представления токена
class Token {
public:
    TokenType type;
    double value;     // Для чисел
    char op;          // Для операторов
    bool isUnary;     // Флаг унарного оператора
    
    explicit Token(double val) : type(TokenType::NUMBER), value(val), isUnary(false) {}
    explicit Token(char c, bool unary = false) : type(TokenType::OPERATOR), op(c), isUnary(unary) {
        if (c == '(') type = TokenType::LEFT_PAREN;
        else if (c == ')') type = TokenType::RIGHT_PAREN;
    }

    int getPriority() const {
        if (type == TokenType::OPERATOR) {
            if (isUnary) return 4;
            if (op == '^') return 3;
            if (op == '*' || op == '/') return 2;
            if (op == '+' || op == '-') return 1;
        }
        return 0;
    }

    bool isOperator() const { return type == TokenType::OPERATOR; }
    bool isNumber() const { return type == TokenType::NUMBER; }
    bool isLeftParenthesis() const { return type == TokenType::LEFT_PAREN; }
    bool isRightParenthesis() const { return type == TokenType::RIGHT_PAREN; }
};


// Лексический анализатор
class Tokenizer {
    std::string expr;
    size_t pos = 0;
public:
    explicit Tokenizer(const std::string& s) : expr(s) {}

    Token next() {
        while (pos < expr.size() && expr[pos] == ' ') pos++;

        if (pos >= expr.size()) throw std::invalid_argument("Unexpected end");

        // Обработка унарных операторов
        if (expr[pos] == '+' || expr[pos] == '-') {
            bool isUnary = false;
            
            if (pos == 0) {
                isUnary = true;
            } else {
                // Поиск предыдущего непробельного символа
                int prev = pos - 1;
                while (prev >= 0 && expr[prev] == ' ') {
                    prev--;
                }
                
                if (prev < 0) {
                    isUnary = true;
                } else {
                    char prev_char = expr[prev];
                    if (prev_char == '(' || prev_char == '+' || prev_char == '-' || 
                        prev_char == '*' || prev_char == '/' || prev_char == '^') {
                        isUnary = true;
                    }
                }
            }
            
            if (isUnary) {
                char op = expr[pos++];
                // Получаем следующий токен после унарного оператора
                Token nextToken = next();
                if (nextToken.isNumber()) {
                    if (op == '-') nextToken.value = -nextToken.value;
                    return nextToken;
                }
                throw std::invalid_argument("Unary operator must precede a number");
            }
        }

        if (isdigit(expr[pos]) || expr[pos] == '.') {
            size_t start = pos;
            while (pos < expr.size() && (isdigit(expr[pos]) || expr[pos] == '.')) pos++;
            return Token(stod(expr.substr(start, pos - start)));
        }

        char c = expr[pos++];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')') {
            return Token(c);
        }

        throw std::invalid_argument("Invalid character: " + std::string(1, c));
    }

    bool isEnd() const { return pos >= expr.size(); }
};


// Конвертер в ОПЗ
class Converter {
    bool debug;
private:
    void printState(const std::string& msg, const Token& t, const std::stack<Token>& s, const std::queue<Token>& q) {
        std::cout << "Step: " << msg << " " << (t.isNumber() ? std::to_string(t.value) : std::string(1, t.op)) << std::endl;
        
        // Вывод стека
        std::cout << "Stack: ";
        std::stack<Token> temp_stack = s;
        std::vector<Token> stack_items;
        while (!temp_stack.empty()) {
            stack_items.push_back(temp_stack.top());
            temp_stack.pop();
        }
        for (auto it = stack_items.rbegin(); it != stack_items.rend(); ++it) {
            if (it->isNumber()) std::cout << it->value << " ";
            else std::cout << it->op << " ";
        }
        
        // Вывод очереди
        std::cout << "\nOutput: ";
        std::queue<Token> temp_q = q;
        while (!temp_q.empty()) {
            Token token = temp_q.front();
            temp_q.pop();
            if (token.isNumber()) std::cout << token.value << " ";
            else std::cout << token.op << " ";
        }
        std::cout << "\n\n";
    }

public:
    explicit Converter(bool d = false) : debug(d) {}

    std::queue<Token> infixToRPN(const std::string& expr) {
        std::stack<Token> op_stack;
        std::queue<Token> output;
        Tokenizer tokenizer(expr);

        while (!tokenizer.isEnd()) {
            Token token = tokenizer.next();

            if (token.isNumber()) {
                output.push(token);
                if (debug) printState("Number", token, op_stack, output);
            }
            else if (token.isLeftParenthesis()) {
                op_stack.push(token);
                if (debug) printState("(", token, op_stack, output);
            }
            else if (token.isRightParenthesis()) {
                while (!op_stack.empty() && !op_stack.top().isLeftParenthesis()) {
                    output.push(op_stack.top());
                    op_stack.pop();
                    if (debug) printState("Pop from stack", token, op_stack, output);
                }
                if (op_stack.empty()) throw std::invalid_argument("Mismatched parentheses");
                op_stack.pop(); // Удаляем '('
                if (debug) printState(")", token, op_stack, output);
            }
            else if (token.isOperator()) {
                while (!op_stack.empty() && 
                      (op_stack.top().getPriority() > token.getPriority() || 
                      (op_stack.top().getPriority() == token.getPriority() && token.op != '^')) &&
                      !op_stack.top().isLeftParenthesis()) {
                    output.push(op_stack.top());
                    op_stack.pop();
                    if (debug) printState("Pop from stack", token, op_stack, output);
                }
                op_stack.push(token);
                if (debug) printState("Operator", token, op_stack, output);
            }
        }

        while (!op_stack.empty()) {
            if (op_stack.top().isLeftParenthesis()) throw std::invalid_argument("Mismatched parentheses");
            output.push(op_stack.top());
            op_stack.pop();
            if (debug) printState("Pop remaining", Token(' '), op_stack, output);
        }

        return output;
    }
};


// Вычислитель ОПЗ
class Evaluator {
    bool debug;
private:
    double applyOp(char op, double a, double b, bool isUnary) {
        if (isUnary) {
            switch(op) {
                case '+': return a;
                case '-': return -a;
                default: throw std::invalid_argument("Unknown unary operator");
            }
        }
        
        switch(op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': 
                if (b == 0) throw std::invalid_argument("Division by zero");
                return a / b;
            case '^': return pow(a, b);
            default: throw std::invalid_argument("Unknown operator");
        }
    }

    void printState(const std::string& msg, const Token& t, const std::stack<double>& s) {
        std::cout << "Step: " << msg << " " << (t.isNumber() ? std::to_string(t.value) : std::string(1, t.op)) << std::endl;
        std::cout << "Stack: ";
        std::stack<double> temp = s;
        std::vector<double> items;
        while (!temp.empty()) {
            items.push_back(temp.top());
            temp.pop();
        }
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n\n";
    }


public:
    explicit Evaluator(bool d = false) : debug(d) {}

    double evaluateRPN(std::queue<Token>& rpn) {
        std::stack<double> calc_stack;

        while (!rpn.empty()) {
            Token token = rpn.front();
            rpn.pop();

            if (token.isNumber()) {
                calc_stack.push(token.value);
                if (debug) printState("Number", token, calc_stack);
            }
            else {
                if (token.isUnary) {
                    if (calc_stack.empty()) throw std::invalid_argument("Not enough operands");
                    double a = calc_stack.top(); calc_stack.pop();
                    double result = applyOp(token.op, a, 0, true);
                    calc_stack.push(result);
                }
                else {
                    if (calc_stack.size() < 2) throw std::invalid_argument("Not enough operands");
                    double b = calc_stack.top(); calc_stack.pop();
                    double a = calc_stack.top(); calc_stack.pop();
                    double result = applyOp(token.op, a, b, false);
                    calc_stack.push(result);
                }
                if (debug) printState("Operator", token, calc_stack);
            }
        }

        if (calc_stack.size() != 1) throw std::invalid_argument("Invalid expression");
        return calc_stack.top();
    }
};


// Генератор случайных выражений
std::string generateRandomExpression(int length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> num_dist(1, 100);
    std::uniform_int_distribution<int> op_dist(0, 5);
    std::uniform_int_distribution<int> unary_dist(0, 1);
    std::string ops = "+-*/^";
    
    std::stringstream ss;
    
    // Начало с числа или унарного оператора
    if (unary_dist(gen)) {
        ss << (unary_dist(gen) ? "+" : "-");
    }
    ss << num_dist(gen);
    
    for (int i = 0; i < length; ++i) {
        // Добавление оператора
        char op = ops[op_dist(gen)];
        ss << ' ' << op << ' ';
        
        // Возможен унарный оператор перед числом
        if (op_dist(gen) < 2 && op != '^') {
            ss << (unary_dist(gen) ? "+" : "-");
        }
        ss << num_dist(gen);
        
        // Возможны скобки
        if (op_dist(gen) < 1 && i < length - 2) {
            int len = std::uniform_int_distribution<int>(1, 3)(gen);
            ss << " ( ";
            if (unary_dist(gen)) ss << (unary_dist(gen) ? "+" : "-");
            ss << num_dist(gen);
            for (int j = 0; j < len; ++j) {
                ss << ' ' << ops[op_dist(gen)] << ' ';
                if (unary_dist(gen)) ss << (unary_dist(gen) ? "+" : "-");
                ss << num_dist(gen);
            }
            ss << " )";
            i += len + 2;
        }
    }
    
    return ss.str();
}


// Тестирование
void runTests(int count) {
    auto start = std::chrono::high_resolution_clock::now();
    int success = 0;
    
    for (int i = 0; i < count; ++i) {
        std::string expr = generateRandomExpression(10); // Выражения длиной 10 операторов

        try {
            Converter converter;
            Evaluator evaluator;
            std::queue<Token> rpn = converter.infixToRPN(expr);
            double result = evaluator.evaluateRPN(rpn);
            success++;
            
            if (i % 100 == 0) {  // Вывод каждого 100-ого выражения
                std::cout << "Expression: " << expr << "\n";
                std::cout << "Result: " << std::setprecision(6) << result << "\n\n";
            }
        } catch (const std::exception& e) {
            if (i % 100 == 0) {
                std::cout << "Failed expression: " << expr << "\n";
                std::cout << "Error: " << e.what() << "\n\n";
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Processed " << count << " expressions (" << success << " successful) in " 
              << duration.count() << " seconds\n";
    std::cout << "Average time per expression: " << duration.count()/count << " seconds\n";
}


int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test") {
        runTests(1000); // Тест на 1000 выражений
        return 0;
    }

    std::string expr;
    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    try {
        bool debug = argc > 1 && std::string(argv[1]) == "--debug";
        
        Converter converter(debug);
        Evaluator evaluator(debug);
        
        std::queue<Token> rpn = converter.infixToRPN(expr);
        double result = evaluator.evaluateRPN(rpn);
        
        std::cout << "Result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
