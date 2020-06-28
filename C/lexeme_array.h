#pragma once

typedef struct {
    char **lexemes; // массив лексем
    int size; // количество
    int capacity; // ёмкость
} lexeme_array_t;

lexeme_array_t init_array() {
    lexeme_array_t array;
    array.size = 0;
    array.capacity = 1;
    array.lexemes = (char **) malloc(sizeof(char *));

    return array;
}

void add_lexeme(lexeme_array_t *array, char *lexeme) {
    array->lexemes[array->size++] = lexeme;

    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->lexemes = (char **) realloc(array->lexemes, array->capacity * sizeof(char *));
    }
}