#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "lexeme_array.h"
#include "variable_array.h"
#include "stack.h"

typedef struct {
    variable_array_t variables;
    lexeme_array_t lexemes;
    lexeme_array_t rpn;
} expression_parser_t;

// проверка на цифру
int is_digit(char c) {
    return c >= '0' && c <= '9';
}

// проверка на букву
int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// разбиение выражения на лексемы
int split_to_lexemes(const char *s, expression_parser_t *parser) {
    parser->lexemes = init_array();
    int i = 0; // индекс в строке

    while (s[i]) {
        if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '%' || s[i] == '^' || s[i] == '(' || s[i] == ')' || s[i] == ',') {
            char *lexeme = (char *) calloc(2, sizeof(char));
            lexeme[0] = s[i++];

            add_lexeme(&parser->lexemes, lexeme); // кладём операцию
        }
        else if (is_digit(s[i])) { // если цифра
            char *number = (char *) malloc(sizeof(char)); // строка для числа
            int size = 0;
            int points = 0; // счётчик точек

            while (s[i] && (is_digit(s[i]) || s[i] == '.')) {
                if (s[i] == '.') {
                    points++;

                    if (points > 1) {
                        printf("Invalid real number in expression\n");
                        return -1;
                    }
                }

                number[size++] = s[i++]; // наращиваем число
                number = (char *) realloc(number, (size + 1) * sizeof(char));
            }

            number[size] = '\0';
            add_lexeme(&parser->lexemes, number); // добавляем число
        }
        else if (is_letter(s[i])) { // если буква
            char *word = (char *) malloc(sizeof(char)); // строка для слова
            int size = 0;

            while (s[i] && (is_letter(s[i]) || is_digit(s[i]))) {
                word[size++] = s[i++]; // наращиваем слово
                word = (char *) realloc(word, (size + 1) * sizeof(char));
            }

            word[size] = '\0';
            add_lexeme(&parser->lexemes, word); // добавляем слово
        }
        else if (s[i] == ' ' || s[i] == '\t') { // если пробельный символ
            i++; // пропускаем
        }
        else { // иначе незивестный символ в выражении
            printf("Unknown character in expression: '%c'\n", s[i]);
            return -1;
        }
    }

    return 0;
}

// проверка на функцию
int is_function(const char *lexeme) {
    if (!strcmp(lexeme, "sin") || !strcmp(lexeme, "cos")) 
        return 1;

    if (!strcmp(lexeme, "tan") || !strcmp(lexeme, "tg"))
        return 1;

    if (!strcmp(lexeme, "cot") || !strcmp(lexeme, "ctg"))
        return 1;

    if (!strcmp(lexeme, "sinh") || !strcmp(lexeme, "sh"))
        return 1;

    if (!strcmp(lexeme, "cosh") || !strcmp(lexeme, "ch"))
        return 1;

    if (!strcmp(lexeme, "tanh") || !strcmp(lexeme, "th"))
        return 1;

    if (!strcmp(lexeme, "asin") || !strcmp(lexeme, "arcsin"))
        return 1;

    if (!strcmp(lexeme, "acos") || !strcmp(lexeme, "arccos"))
        return 1;

    if (!strcmp(lexeme, "arctg") || !strcmp(lexeme, "atan"))
        return 1;

    if (!strcmp(lexeme, "ln") || !strcmp(lexeme, "log2") || !strcmp(lexeme, "lg"))
        return 1;

    if (!strcmp(lexeme, "exp") || !strcmp(lexeme, "abs") || !strcmp(lexeme, "sign"))
        return 1;

    if (!strcmp(lexeme, "sqrt") || !strcmp(lexeme, "cbrt") )
        return 1;

    return 0;
}

// проверка на бинарную функцию
int is_binary_function(const char *lexeme) {
    return !strcmp(lexeme, "max") || !strcmp(lexeme, "min") || !strcmp(lexeme, "log") || !strcmp(lexeme, "pow") || !strcmp(lexeme, "root");
}

// проверка на операцию
int is_operator(const char *lexeme) {
    return !strcmp(lexeme, "+") || !strcmp(lexeme, "-") || !strcmp(lexeme, "*") || !strcmp(lexeme, "/") || !strcmp(lexeme, "%") || !strcmp(lexeme, "^");
}

// проверка на константу
int is_constant(const char *lexeme) {
    return !strcmp(lexeme, "pi") || !strcmp(lexeme, "e") || !strcmp(lexeme, "ln2") || !strcmp(lexeme, "ln10") || !strcmp(lexeme, "sqrt2");
}

// проверка на число
int is_number(const char *lexeme) {
    for (int i = 0; lexeme[i]; i++)
        if (!is_digit(lexeme[i]) && lexeme[i] != '.')
            return 0;

    return 1;
}

// проверка на переменную
int is_variable(const char *lexeme) {
    if (!is_letter(lexeme[0]))
        return 0;

    for (int i = 1; lexeme[i]; i++)
        if (!is_letter(lexeme[i]) && !is_digit(lexeme[i]))
            return 0;

    return !is_function(lexeme) && !is_binary_function(lexeme) && !is_constant(lexeme);
}

// получение приоритета операции
int get_priority(const char *lexeme) {
    if (is_function(lexeme) || is_binary_function(lexeme))
        return 4;

    if (!strcmp(lexeme, "!") || !strcmp(lexeme, "^"))
        return 3;

    if (!strcmp(lexeme, "*") || !strcmp(lexeme, "/") || !strcmp(lexeme, "%"))
        return 2;

    if (!strcmp(lexeme, "+") || !strcmp(lexeme, "-"))
        return 1;

    return 0;
}

// проверка, что текущая лексема менее приоритетна лексемы на вершине стека
int is_more_priority(const char *curr, const char *top) {
    if (!strcmp(curr, "^") || !strcmp(curr, "!"))
        return get_priority(top) > get_priority(curr);

    return get_priority(top) >= get_priority(curr);
}

// получение польской записи
int convert_to_rpn(expression_parser_t *parser) {
    parser->rpn = init_array();
    stack_t stack = init_stack();
    int mayUnary = 1;

    for (int i = 0; i < parser->lexemes.size; i++) {
        char *lexeme = parser->lexemes.lexemes[i];

        if (is_number(lexeme) || is_constant(lexeme)) {
            add_lexeme(&parser->rpn, lexeme);
            mayUnary = 0;
        }
        else if (is_function(lexeme) || is_binary_function(lexeme)) {
            push_lexeme(&stack, lexeme);
            mayUnary = 1;
        }
        else if (is_variable(lexeme)) {
            add_lexeme(&parser->rpn, lexeme);
            add_variable(&parser->variables, lexeme);
            mayUnary = 0;
        }
        else if (!strcmp(lexeme, ",")) {
            while (!is_empty(stack) && strcmp(peek_lexeme(stack), "("))
                add_lexeme(&parser->rpn, pop_lexeme(&stack));

            if (is_empty(stack)) {
                printf("Incorrect expression\n");
                return -1;
            }
        }
        else if (is_operator(lexeme)) {
            if (!strcmp(lexeme, "-") && mayUnary) {
                lexeme[0] = '!';
                lexeme[1] = '\0';
            }

            while (!is_empty(stack) && is_more_priority(lexeme, peek_lexeme(stack)))
                add_lexeme(&parser->rpn, pop_lexeme(&stack));

            push_lexeme(&stack, lexeme);
            mayUnary = !strcmp(lexeme, "^");
        }
        else if (!strcmp(lexeme, "(")) {
            push_lexeme(&stack, lexeme);
            mayUnary = 1;
        }
        else if (!strcmp(lexeme, ")")) {
            while (!is_empty(stack) && strcmp(peek_lexeme(stack), "("))
                add_lexeme(&parser->rpn, pop_lexeme(&stack));

            if (is_empty(stack)) {
                printf("Incorrect expression: brackets are disbalanced\n");
                return -1;
            }

            pop_lexeme(&stack);

            if (!is_empty(stack) && is_function(peek_lexeme(stack)))
                add_lexeme(&parser->rpn, pop_lexeme(&stack));

            mayUnary = 0;
        }
        else {
            printf("Incorrect expression: unknown lexeme '%s'\n", lexeme);
            return -1;
        }
    }

    while (!is_empty(stack) > 0) {
        if (!strcmp(peek_lexeme(stack), "(")) {
            printf("Incorrect expression: brackets are disbalanced\n");
            return -1;
        }

        add_lexeme(&parser->rpn, pop_lexeme(&stack));
    }

    return 0;
}

// вычисление операции
double evaluate_operator(const char *op, double arg1, double arg2) {
    if (!strcmp(op, "+"))
        return arg1 + arg2;

    if (!strcmp(op, "-"))
        return arg1 - arg2;

    if (!strcmp(op, "*"))
        return arg1 * arg2;

    if (!strcmp(op, "/"))
        return arg1 / arg2;

    if (!strcmp(op, "%"))
        return fmod(arg1, arg2);

    if (!strcmp(op, "^"))
        return pow(arg1, arg2);

    return 0;
}

// вычисление функции
double evaluate_function(const char *f, double arg) {
    if (!strcmp(f, "sin"))
        return sin(arg);

    if (!strcmp(f, "cos"))
        return cos(arg);

    if (!strcmp(f, "tan") || !strcmp(f, "tg"))
        return tan(arg);

    if (!strcmp(f, "cot") || !strcmp(f, "ctg"))
        return 1.0 / tan(arg);

    if (!strcmp(f, "sinh") || !strcmp(f, "sh"))
        return sinh(arg);

    if (!strcmp(f, "cosh") || !strcmp(f, "ch"))
        return cosh(arg);

    if (!strcmp(f, "tanh") || !strcmp(f, "th"))
        return tanh(arg);

    if (!strcmp(f, "asin") || !strcmp(f, "arcsin"))
        return asin(arg);

    if (!strcmp(f, "acos") || !strcmp(f, "arccos"))
        return acos(arg);

    if (!strcmp(f, "atan") || !strcmp(f, "arctg"))
        return atan(arg);

    if (!strcmp(f, "ln"))
        return log(arg);

    if (!strcmp(f, "log2"))
        return log2(arg);

    if (!strcmp(f, "lg"))
        return log10(arg);

    if (!strcmp(f, "exp"))
        return exp(arg);

    if (!strcmp(f, "sqrt"))
        return sqrt(arg);

    if (!strcmp(f, "cbrt"))
        return cbrt(arg);

    if (!strcmp(f, "abs"))
        return fabs(arg);

    if (!strcmp(f, "sign"))
        return arg > 0 ? 1 : (arg < 0 ? -1 : 0);

    return 0;
}

// вычисление бинарной функции
double evaluate_binary_function(const char *f, double arg1, double arg2) {
    if (!strcmp(f, "max"))
        return arg1 > arg2 ? arg1 : arg2;

    if (!strcmp(f, "min"))
        return arg1 < arg2 ? arg1 : arg2;

    if (!strcmp(f, "log"))
        return log(arg2) / log(arg1);

    if (!strcmp(f, "pow"))
        return pow(arg1, arg2);

    if (!strcmp(f, "root"))
        return pow(arg2, 1.0 / arg1);

    return 0;
}

// вычисление константы
double evaluate_constant(const char *name) {
    if (!strcmp(name, "pi"))
        return M_PI;

    if (!strcmp(name, "e"))
        return M_E;

    if (!strcmp(name, "ln2"))
        return log(2);

    if (!strcmp(name, "ln10"))
        return log(10);

    if (!strcmp(name, "sqrt2"))
        return sqrt(2);

    return 0;
}

// инициализация парсера
int init_parser(const char *expression, expression_parser_t *parser) {
    // разбиваем на лексемы
    if (split_to_lexemes(expression, parser))
        return -1;

    if (convert_to_rpn(parser)) // получаем польскую запись
        return -1;

    return 0;
}

// обновление значения переменной
void set_value(expression_parser_t *parser, char *name, double value) {
    parser->variables.variables[index_of_variable(parser->variables, name)].value = value;
}

// вычисление выражения
int evaluate(expression_parser_t parser, double *result) {
    stack_t stack = init_stack();

    for (int i = 0; i < parser.rpn.size; i++) {
        char *lexeme = parser.rpn.lexemes[i];

        if (is_operator(lexeme)) {
            if (is_empty(stack) || stack.top->next == NULL) {
                printf("Unable to evaluate operator '%s'\n", lexeme);
                return -1;
            }

            double arg2 = pop_value(&stack);
            double arg1 = pop_value(&stack);

            push_value(&stack, evaluate_operator(lexeme, arg1, arg2));
        }
        else if (is_function(lexeme)) {
            if (is_empty(stack)) {
                printf("Unable to evaluate function '%s'\n", lexeme);
                return -1;
            }

            double arg = pop_value(&stack);
            push_value(&stack, evaluate_function(lexeme, arg));
        }
        else if (is_binary_function(lexeme)) {
            if (is_empty(stack) || stack.top->next == NULL) {
                printf("Unable to evaluate function '%s'\n", lexeme);
                return -1;
            }

            double arg2 = pop_value(&stack);
            double arg1 = pop_value(&stack);

            push_value(&stack, evaluate_binary_function(lexeme, arg1, arg2));
        }
        else if (!strcmp(lexeme, "!")) {
            if (is_empty(stack)) {
                printf("Unable to evaluate unary minus\n");
                return -1;
            }

            double arg = pop_value(&stack);
            push_value(&stack, -arg);
        }
        else if (is_constant(lexeme)) {
            push_value(&stack, evaluate_constant(lexeme));
        }
        else if (is_variable(lexeme)) {
            push_value(&stack, parser.variables.variables[index_of_variable(parser.variables, lexeme)].value);
        }
        else if (is_number(lexeme)) {
            push_value(&stack, atof(lexeme));
        }
        else {
            printf("Unknown rpn lexeme '%s'\n", lexeme);
            return -1;
        }
    }

    if (is_empty(stack)) {
        printf("Incorrect expression\n");
        return -1;
    }

    *result = pop_value(&stack);

    if (!is_empty(stack)) {
        printf("Incorrect expression\n");
        return -1;
    }

    return 0;
}