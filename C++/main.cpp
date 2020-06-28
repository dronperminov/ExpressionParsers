#include <iostream>
#include <string>
#include "ExpressionParser.hpp"

using namespace std;

void TestParser(const string expression, map<string, double> variables, double answer, double eps = 1e-10) {
    ExpressionParser parser(expression);

    for (auto it = variables.begin(); it != variables.end(); it++)
        parser.SetValue(it->first, it->second);

    double result = parser.Evaluate();

    if (fabs(result - answer) > eps)
        cout << "FAILED: " << expression << ": " << result << " != " << answer << endl;
}

int main() {
    ExpressionParser calculator("sqrt(abs(x))");

    for (double x = -10; x <= 10; x++) {
        calculator.SetValue("x", x);
        cout << calculator.Evaluate() << endl;
    }

    TestParser("pi", { }, M_PI);
    TestParser("1+2+3+4", { }, 10);
    TestParser("(1+2)*(3+4)", { }, 21);
    TestParser("4^-2", { }, 1.0 / 16);
    TestParser("4^(-2)", { }, 1.0 / 16);
    TestParser("4^-2*4", { }, 0.25);
    TestParser("sqrt(25 - abs(3 * (2 - 5)))", { }, 4);
    TestParser("sqrt(x)", { { "x", 625 } }, 25);
    TestParser("sin(arcsin(x))", { { "x", 0.4 } }, 0.4);
    TestParser("e^pi - exp(2*acos(0))", { }, 0);
    TestParser("lg(10000)", { }, 4);
    TestParser("cbrt(8)", { }, 2);
    TestParser("cbrt(-8)", { }, -2);
    TestParser("2^2^3", { }, 256);
    TestParser("(2^2)^3", { }, 64);
    TestParser("2^(2^3)", { }, 256);
    TestParser("-1^2", { }, -1);
    TestParser("(-1)^2", { }, 1);
    TestParser("2^-2^2", { }, 1.0 / 16);
    TestParser("2^(-2)^2", { }, 16);
    TestParser("(2^-2)^2", { }, 1.0 / 16);
    TestParser("2^-2^-2", { }, pow(2, -0.25));

    TestParser("max(6, 8)", { }, 8);
    TestParser("min(6, 8)", { }, 6);
    TestParser("log(2, 8)", { }, 3);
    TestParser("pow(2, 8)", { }, 256);
    TestParser("root(4, 256)", { }, 4);
    TestParser("root(8 / 4 + log(2, 4), 2 ^ 8)", { }, 4);

    TestParser("(x1 + x2) ^ 2", { { "x1", 3 }, { "x2", 5 } }, 64);
    TestParser("(x123 + x26x) ^ 2", { { "x123", 3 }, { "x26x", 5 } }, 64);
}