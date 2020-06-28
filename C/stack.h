#include <stdio.h>
#include <stdlib.h>

typedef union {
    double value; // число
    char *lexeme; // лексема
} node_value_t;

// структура для элемента стека
typedef struct node_t {
    node_value_t value; // значение элемента
    struct node_t *next; // указатель на следующий элемент
} node_t;

// структура для стека
typedef struct stack_t {
    node_t *top; // указатель на вершину стека
} stack_t;

// инициализация пустого стека
stack_t init_stack() {
    stack_t stack; // создаём стек
    stack.top = NULL; // стек пуст, вершины нет
    return stack; // возвращаем созданный стек
}

// добавление элемента в стек
void push(stack_t *stack, node_value_t value) {
    node_t *node = (node_t *) malloc(sizeof(node_t)); // выделяем память под элемент

    node->value = value; // сохраняем значение
    node->next = stack->top; // за новым элементом следует текущий верхний
    stack->top = node; // делаем новый элемент вершиной
}

// добавление числа в стек
void push_value(stack_t *stack, double number) {
    node_value_t value;
    value.value = number;
    push(stack, value);
}

// добавление числа в стек
void push_lexeme(stack_t *stack, char *lexeme) {
    node_value_t value;
    value.lexeme = lexeme;
    push(stack, value);
}

// удаление элемента из стека
node_value_t pop(stack_t *stack) {
    node_value_t value = stack->top->value; // запоминаем значение на вершине стека

    node_t *tmp = stack->top; // сохраняем значение текущей вершины
    stack->top = stack->top->next; // переходим к следующему элементу
    free(tmp); // удаляем старое начало

    return value; // возвращаем сохранённое значение
}

// удаление числа из стека
double pop_value(stack_t *stack) {
    return pop(stack).value;
}

// удаление лексемы из стека
char* pop_lexeme(stack_t *stack) {
    return pop(stack).lexeme;
}

// получение элемента на вершине стека
node_value_t peek(stack_t stack) {
    return stack.top->value; // возвращаем элемент на вершине стека
}

// получение числа на вершине стека
double peek_value(stack_t stack) {
    return peek(stack).value;
}

// получение лексемы на вершине стека
char* peek_lexeme(stack_t stack) {
    return peek(stack).lexeme;
}

// проверка стека на пустоту
int is_empty(stack_t stack) {
    return stack.top == NULL; // стек пуст, если нет вершины
}