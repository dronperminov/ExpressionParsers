#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression_parser.h"

void test_parser(const char* expression, double answer) {
    expression_parser_t parser;
    if (init_parser(expression, &parser))
        return;

    double result;

    if (evaluate(parser, &result))
        return;

    if (fabs(result - answer) > 1e-10)
        printf("FAILED: %s: %lf != %lf\n", expression, result, answer);
}

int main() {
    test_parser("pi", M_PI);
    test_parser("1+2+3+4", 10);
    test_parser("(1+2)*(3+4)", 21);
    test_parser("4^-2", 1.0 / 16);
    test_parser("4^(-2)", 1.0 / 16);
    test_parser("4^-2*4", 0.25);
    test_parser("sqrt(25 - abs(3 * (2 - 5)))", 4);
    test_parser("e^pi - exp(2*acos(0))", 0);
    test_parser("lg(10000)", 4);
    test_parser("cbrt(8)", 2);
    test_parser("cbrt(-8)", -2);
    test_parser("2^2^3", 256);
    test_parser("(2^2)^3", 64);
    test_parser("2^(2^3)", 256);
    test_parser("-1^2", -1);
    test_parser("(-1)^2", 1);
    test_parser("2^-2^2", 1.0 / 16);
    test_parser("2^(-2)^2", 16);
    test_parser("(2^-2)^2", 1.0 / 16);
    test_parser("2^-2^-2", pow(2, -0.25));

    test_parser("max(6, 8)", 8);
    test_parser("min(6, 8)", 6);
    test_parser("log(2, 8)", 3);
    test_parser("pow(2, 8)", 256);
    test_parser("root(4, 256)", 4);
    test_parser("root(8 / 4 + log(2, 4), 2 ^ 8)", 4);
}