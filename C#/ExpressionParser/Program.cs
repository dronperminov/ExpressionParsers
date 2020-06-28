using System;
using System.Collections.Generic;

namespace ExpressionParser {
    class Program {
        static void TestParser(string expression, Dictionary<string, double> variables, double answer, double eps= 1e-10) {
            ExpressionParser parser = new ExpressionParser(expression);

            if (variables != null)
                foreach (string name in variables.Keys)
                    parser.SetValue(name, variables[name]);

            double result = parser.Evaluate();

            if (Math.Abs(result - answer) > eps)
                Console.WriteLine("FAILED: {0}: {1} != {2}", expression, result, answer);
        }

        static void Main(string[] args) {
            ExpressionParser calculator = new ExpressionParser("sqrt(abs(x))");

            for (double x = -10; x <= 10; x++) {
                calculator.SetValue("x", x);
                Console.WriteLine(calculator.Evaluate());
            }

            TestParser("pi", null, Math.PI);
            TestParser("1+2+3+4", null, 10);
            TestParser("(1+2)*(3+4)", null, 21);
            TestParser("4^-2", null, 1.0 / 16);
            TestParser("4^(-2)", null, 1.0 / 16);
            TestParser("4^-2*4", null, 0.25);
            TestParser("sqrt(25 - abs(3 * (2 - 5)))", null, 4);
            TestParser("sqrt(x)", new Dictionary<string, double>() { { "x", 625 } }, 25);
            TestParser("sin(arcsin(x))", new Dictionary<string, double>() { { "x", 0.4 } }, 0.4);
            TestParser("e^pi - exp(2*acos(0))", null, 0);
            TestParser("lg(10000)", null, 4);
            TestParser("cbrt(8)", null, 2);
            TestParser("cbrt(-8)", null, -2);
            TestParser("2^2^3", null, 256);
            TestParser("(2^2)^3", null, 64);
            TestParser("2^(2^3)", null, 256);
            TestParser("-1^2", null, -1);
            TestParser("(-1)^2", null, 1);
            TestParser("2^-2^2", null, 1.0 / 16);
            TestParser("2^(-2)^2", null, 16);
            TestParser("(2^-2)^2", null, 1.0 / 16);
            TestParser("2^-2^-2", null, Math.Pow(2, -0.25));

            TestParser("max(6, 8)", null, 8);
            TestParser("min(6, 8)", null, 6);
            TestParser("log(2, 8)", null, 3);
            TestParser("pow(2, 8)", null, 256);
            TestParser("root(4, 256)", null, 4);
            TestParser("root(8 / 4 + log(2, 4), 2 ^ 8)", null, 4);

            TestParser("(x1 + x2) ^ 2", new Dictionary<string, double>() { { "x1", 3 }, { "x2", 5 } }, 64);
            TestParser("(x123 + x26x) ^ 2", new Dictionary<string, double>() { { "x123", 3 }, { "x26x", 5 } }, 64);
            Console.ReadKey();
        }
    }
}
