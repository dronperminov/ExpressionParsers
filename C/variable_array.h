#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *name;
    double value;
} variable_t;

typedef struct {
    variable_t *variables;
    int size;
} variable_array_t;

variable_array_t init_variables() {
    variable_array_t variables;
    variables.variables = NULL;
    variables.size = 0;

    return variables;
}

int index_of_variable(variable_array_t variables, char *name) {
    for (int i = 0; i < variables.size; i++)
        if (!strcmp(variables.variables[i].name, name))
            return i;

    return -1;
}

void add_variable(variable_array_t *variables, char *name) {
    if (index_of_variable(*variables, name) > -1)
        return;

    variables->size++;
    variables->variables = (variable_t *) realloc(variables->variables, variables->size * sizeof(variable_t));
    variables->variables[variables->size - 1].name = name;
    variables->variables[variables->size - 1].value = 0;
}