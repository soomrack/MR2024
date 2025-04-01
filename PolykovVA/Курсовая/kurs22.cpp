#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct stack_char {
    char data;
    struct stack_char* next;
};


struct stack_ll {
    long long data;
    struct stack_ll* next;
};


void push_char(struct stack_char** top, char symbol) {
    struct stack_char* new_node = (struct stack_char*)malloc(sizeof(struct stack_char));
    if (new_node) {
        new_node->data = symbol;
        new_node->next = *top;
        *top = new_node;
    }
}


char pop_char(struct stack_char** top) {
    if (!*top) return '\0';
    struct stack_char* temp = *top;
    char symbol = temp->data;
    *top = temp->next;
    free(temp);
    return symbol;
}


void push_ll(struct stack_ll** top, long long value) {
    struct stack_ll* new_node = (struct stack_ll*)malloc(sizeof(struct stack_ll));
    if (new_node) {
        new_node->data = value;
        new_node->next = *top;
        *top = new_node;
    }
}


long long pop_ll(struct stack_ll** top) {
    if (!*top) return 0;
    struct stack_ll* temp = *top;
    long long value = temp->data;
    *top = temp->next;
    free(temp);
    return value;
}


bool is_left_associative(char op) {
    return (op != '^'); 
}


int get_priority(char op) {
    switch (op) {
    case '(': case ')': return 1;
    case '+': case '-': return 2;
    case '*': case '/': case '%': return 3;
    case '^': return 4;
    default: return 0;
    }
}


int main() {
    struct stack_char* operators = NULL;    
    struct stack_ll* numbers = NULL;        
    char polish[2501] = { 0 };                
    int polish_index = 0;

    printf("Enter the expression> ");

    
    char symbol;
    bool unary_minus = false, unary_plus = false;


    while ((symbol = fgetc(stdin)) != '\n' && symbol != EOF) {
        if (symbol >= '0' && symbol <= '9') {
            polish[polish_index++] = symbol;
        }
        else {
            if (symbol != '(' && symbol != ')')
                polish[polish_index++] = ' ';

            if (symbol == '(') {
                char next = fgetc(stdin);
                if (next == '-') unary_minus = true;
                else if (next == '+') unary_plus = true;
                else ungetc(next, stdin);
                push_char(&operators, symbol);
            }

            else if (symbol == ')') {
                while (operators && operators->data != '(') {
                    polish[polish_index++] = pop_char(&operators);
                    polish[polish_index++] = ' ';
                }
                if (operators) pop_char(&operators);
                unary_minus = unary_plus = false;
            }

            else if (symbol == '+' || symbol == '-' || symbol == '*' ||
                symbol == '/' || symbol == '^') {
                if ((symbol == '-' && unary_minus) || (symbol == '+' && unary_plus)) {
                    polish[polish_index++] = ' ';
                }

                while (operators && operators->data != '(' &&
                    (get_priority(operators->data) > get_priority(symbol) ||
                        (get_priority(operators->data) == get_priority(symbol) &&
                            is_left_associative(symbol)))) {
                    polish[polish_index++] = pop_char(&operators);
                    polish[polish_index++] = ' ';
                }
                push_char(&operators, symbol);
                unary_minus = unary_plus = false;
            }
        }
    }

    while (operators) {
        polish[polish_index++] = ' ';
        polish[polish_index++] = pop_char(&operators);
    }
    polish[polish_index] = '\0';

    
    polish_index = 0;
    long long number = 0;
    bool building_number = false;


    while (polish[polish_index] != '\0') {
        symbol = polish[polish_index++];

        if (symbol >= '0' && symbol <= '9') {
            number = number * 10 + (symbol - '0');
            building_number = true;
        }
        else if (symbol == ' ' && building_number) {
            push_ll(&numbers, number);
            number = 0;
            building_number = false;
        }
        else if (symbol == '+' || symbol == '-' || symbol == '*' ||
            symbol == '/' || symbol == '^') {
            if (!numbers || !numbers->next) {
                printf("Error: insufficient operands\n");
                return 1;
            }
            long long b = pop_ll(&numbers);
            long long a = pop_ll(&numbers);
            long long result;

            switch (symbol) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/':
                if (b == 0) {
                    printf("Error: division by zero\n");
                    return 1;
                }
                result = a / b;
                break;
            case '^':
                if (b < 0) {
                    result = 0; 
                }
                else {
                    result = 1;
                    for (int i = 0; i < b; i++) {
                        result *= a;
                    }
                }
                break;
            }
            push_ll(&numbers, result);
        }
    }

    
    printf("Polska notation: %s\n", polish);
    if (numbers && !numbers->next) {
        printf("Result: %lld\n", pop_ll(&numbers));
    }
    else {
        printf("Error: invalid expression\n");
    }

    
    while (operators) pop_char(&operators);
    while (numbers) pop_ll(&numbers);

    return 0;
}