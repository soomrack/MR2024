#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>
#include <map>
#include <stdexcept>

using namespace std;

map<string, int> precedence = {
    {"^", 4},
    {"*", 3}, {"/", 3},
    {"+", 2}, {"-", 2},
    {"(", 1}
};

bool isOperator(const string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^";
}

bool isUnary(const string& expr, size_t pos) {
    if (pos == 0) return true;
    char prev = expr[pos - 1];
    return prev == '(' || isOperator(string(1, prev));
}

double applyOperator(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("Деление на ноль!");
        return a / b;
    }
    if (op == "^") return pow(a, b);
    throw runtime_error("Неизвестный оператор: " + op);
}

string infixToRPN(const string& infix) {
    stack<string> opStack;
    stringstream output;
    string token;

    for (size_t i = 0; i < infix.size(); ++i) {
        char c = infix[i];

        if (isspace(c)) continue;

        if (isdigit(c) || (c == '-' && isUnary(infix, i) && isdigit(infix[i+1]))) {
            if (c == '-') output << "0 ";
            while (i < infix.size() && (isdigit(infix[i]) || infix[i] == '.')) {
                output << infix[i++];
            }
            output << ' ';
            --i;
            continue;
        }

        token = string(1, c);
        if (token == "(") {
            opStack.push(token);
        } 
        else if (token == ")") {
            while (!opStack.empty() && opStack.top() != "(") {
                output << opStack.top() << ' ';
                opStack.pop();
            }
            if (opStack.empty()) throw runtime_error("Несбалансированные скобки!");
            opStack.pop();
        } 
        else if (isOperator(token)) {
            while (!opStack.empty() && precedence[opStack.top()] >= precedence[token]) {
                output << opStack.top() << ' ';
                opStack.pop();
            }
            opStack.push(token);
        } 
        else {
            throw runtime_error("Некорректный символ: " + token);
        }
    }

    while (!opStack.empty()) {
        if (opStack.top() == "(") throw runtime_error("Несбалансированные скобки!");
        output << opStack.top() << ' ';
        opStack.pop();
    }

    return output.str();
}

double evaluateRPN(const string& rpn) {
    stack<double> s;
    istringstream iss(rpn);
    string token;

    while (iss >> token) {
        if (isOperator(token)) {
            if (s.size() < 2) throw runtime_error("Недостаточно операндов для " + token);
            double b = s.top(); s.pop();
            double a = s.top(); s.pop();
            s.push(applyOperator(a, b, token));
        } 
        else {
            try {
                s.push(stod(token));
            } catch (...) {
                throw runtime_error("Некорректный токен: " + token);
            }
        }
    }

    if (s.size() != 1) throw runtime_error("Неверное выражение");
    return s.top();
}

int main() {
    string infixExpr;
    cout << "Введите выражение (например, '3 + 4 * (2 - 1)'): ";
    getline(cin, infixExpr);

    try {
        string rpnExpr = infixToRPN(infixExpr);
        cout << "ОПЗ: " << rpnExpr << endl;

        double result = evaluateRPN(rpnExpr);
        cout << "Результат: " << result << endl;
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}