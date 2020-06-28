using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace ExpressionParser {
    class ExpressionParser {
        string expression;
        Dictionary<string, Func<double, double>> functions; // функции
        Dictionary<string, Func<double, double, double>> binaryFunctions; // бинарные функции
        Dictionary<string, Func<double, double, double>> operators; // операции
        Dictionary<string, double> constants; // константы
        Regex regexp; // регулярное выражение для парсинга
        Dictionary<string, double> variables; // переменные
        List<string> lexemes; // лексемы
        List<string> rpn; // польская запись

        public ExpressionParser(string expression) {
            this.expression = expression.ToLower(); // сохраняем выражение в нижнем регистре

            InitFunctions(); // инциализируем функции
            InitBinaryFunctions(); // инициализируем бинарные функции
            InitOperators(); // инциализируем операции
            InitConstants(); // инициализируем константы
            InitRegExp(); // инициализируем регулярное выражение
            SplitToLexemes(); // разбиваем на лексемы
            ConvertToRPN(); // получаем польскую запись
        }

        // инициализация функций
        void InitFunctions() {
            functions = new Dictionary<string, Func<double, double>>();

            functions["sin"] = Math.Sin;
            functions["cos"] = Math.Cos;
            functions["tan"] = Math.Tan;
            functions["tg"] = Math.Tan;
            functions["cot"] = (double x) => { return 1.0 / Math.Tan(x); };
            functions["ctg"] = (double x) => { return 1.0 / Math.Tan(x); };

            functions["sinh"] = Math.Sinh;
            functions["sh"] = Math.Sinh;
            functions["cosh"] = Math.Cosh;
            functions["ch"] = Math.Cosh;
            functions["tanh"] = Math.Tanh;
            functions["th"] = Math.Tanh;

            functions["asin"] = Math.Asin;
            functions["arcsin"] = Math.Asin;
            functions["acos"] = Math.Acos;
            functions["arccos"] = Math.Acos;
            functions["arctg"] = Math.Atan;
            functions["atan"] = Math.Atan;

            functions["ln"] = Math.Log;
            functions["log2"] = (double x) => { return Math.Log(x, 2); };
            functions["lg"] = Math.Log10;
            functions["exp"] = Math.Exp;

            functions["sqrt"] = Math.Sqrt;
            functions["cbrt"] = (double x) => { return Math.Pow(x, 1.0 / 3); };
            functions["abs"] = Math.Abs;
            functions["sign"] = (double x) => { return Math.Sign(x); };
        }

        // инициализация бинарных функций
        void InitBinaryFunctions() {
            binaryFunctions = new Dictionary<string, Func<double, double, double>>();

            binaryFunctions["max"] = Math.Max;
            binaryFunctions["min"] = Math.Min;
            binaryFunctions["log"] = (double x, double y) => { return Math.Log(y, x); };
            binaryFunctions["pow"] = Math.Pow;
            binaryFunctions["root"] = (double x, double y) => { return Math.Pow(y, 1.0 / x); };
        }

        // инициализация операций
        void InitOperators() {
            operators = new Dictionary<string, Func<double, double, double>>();

            operators["+"] = (double x, double y) => { return x + y; };
            operators["-"] = (double x, double y) => { return x - y; };
            operators["*"] = (double x, double y) => { return x * y; };
            operators["/"] = (double x, double y) => { return x / y; };
            operators["%"] = (double x, double y) => { return x % y; };
            operators["^"] = (double x, double y) => { return Math.Pow(x, y); };
        }

        // инициализация констант
        void InitConstants() {
            constants = new Dictionary<string, double>();

            constants["pi"] = Math.PI;
            constants["e"] = Math.E;
            constants["ln2"] = Math.Log(2);
            constants["ln10"] = Math.Log(10);
            constants["sqrt2"] = Math.Sqrt(2);
        }

        // инициализация регулярного выражения
        void InitRegExp() {
            string number = "\\d+\\.\\d+|\\d+"; // вещественные числа
            string operations = "\\" + string.Join("|\\", operators.Keys); // операции
            string functions = string.Join("|", this.functions.Keys); // функции
            string binaryFunctions = string.Join("|", this.binaryFunctions.Keys); // бинарные функции
            string constants = string.Join("|", this.constants.Keys); // константы
            string variables = "[a-z][a-z\\d]*"; // переменные

            RegexOptions options = RegexOptions.IgnoreCase | RegexOptions.Multiline;
            regexp = new Regex(number + "|\\(|\\)|,|" + operations + "|" + functions + "|" + binaryFunctions + "|" + constants + "|" + variables, options);
        }

        // парсинг на лексемы с проверкой на корректность
        void SplitToLexemes() {
            MatchCollection matches = regexp.Matches(expression); // разбиваем на лексемы
            lexemes = new List<string>();

            foreach (Match match in matches)
                lexemes.Add(match.Value);

            if (string.Join("", lexemes) != expression.Replace(" ", "")) // если выражения не совпадают
                throw new Exception("Unknown characters in expression"); // значит есть некорректные символы
        }

        // проверка на функцию
        bool IsFunction(string lexeme) {
            return functions.ContainsKey(lexeme);
        }

        // проверка на бинарную функцию
        bool IsBinaryFunction(string lexeme) {
            return binaryFunctions.ContainsKey(lexeme);
        }

        // проверка на операцию
        bool IsOperator(string lexeme) {
            return this.operators.ContainsKey(lexeme);
        }

        // проверка на константу
        bool IsConstant(string lexeme) {
            return constants.ContainsKey(lexeme);
        }

        // проверка на число
        bool IsNumber(string lexeme) {
            return Regex.IsMatch(lexeme, @"^\d+\.\d+|\d+$");
        }

        // проверка на переменную
        bool IsVariable(string lexeme) {
            return Regex.IsMatch(lexeme, @"^[a-z][a-z\d]*$");
        }

        // получение приоритета операции
        int GetPriority(string lexeme) {
            if (IsFunction(lexeme) || IsBinaryFunction(lexeme))
                return 4;

            if (lexeme == "!" || lexeme == "^")
                return 3;

            if (lexeme == "*" || lexeme == "/" || lexeme == "%")
                return 2;

            if (lexeme == "+" || lexeme == "-")
                return 1;

            return 0;
        }

        // проверка, что текущая лексема менее приоритетна лексемы на вершине стека
        bool IsMorePriority(string curr, string top) {
            if (curr == "^" || curr == "!")
                return GetPriority(top) > GetPriority(curr);

            return GetPriority(top) >= GetPriority(curr);
        }

        // получение польской записи
        void ConvertToRPN() {
            rpn = new List<string>();
            variables = new Dictionary<string, double>();
            Stack<string> stack = new Stack<string>();
            bool mayUnary = true;

            foreach (string lexeme in lexemes) {
                if (IsNumber(lexeme) || IsConstant(lexeme)) {
                    rpn.Add(lexeme);
                    mayUnary = false;
                }
                else if (IsFunction(lexeme) || IsBinaryFunction(lexeme)) {
                    stack.Push(lexeme);
                    mayUnary = true;
                }
                else if (IsVariable(lexeme)) {
                    rpn.Add(lexeme);
                    variables[lexeme] = 0;
                    mayUnary = false;
                }
                else if (lexeme == ",") {
                    while (stack.Count > 0 && stack.Peek() != "(")
                        rpn.Add(stack.Pop());

                    if (stack.Count == 0)
                        throw new Exception("Incorrect expression");
                }
                else if (IsOperator(lexeme)) {
                    string curr = lexeme == "-" && mayUnary ? "!" : lexeme;

                    while (stack.Count > 0 && IsMorePriority(curr, stack.Peek()))
                        rpn.Add(stack.Pop());

                    stack.Push(curr);
                    mayUnary = lexeme == "^";
                }
                else if (lexeme == "(") {
                    stack.Push(lexeme);
                    mayUnary = true;
                }
                else if (lexeme == ")") {
                    while (stack.Count > 0 && stack.Peek() != "(")
                        rpn.Add(stack.Pop());

                    if (stack.Count == 0)
                        throw new Exception("Incorrect expression: brackets are disbalanced");

                    stack.Pop();

                    if (stack.Count > 0 && IsFunction(stack.Peek()))
                        rpn.Add(stack.Pop());

                    mayUnary = false;
                }
                else
                    throw new Exception("Incorrect expression: unknown lexeme '" + lexeme + "'");
            }

            while (stack.Count > 0) {
                if (stack.Peek() == "(")
                    throw new Exception("Incorrect expression: brackets are disbalanced");

                rpn.Add(stack.Pop());
            }
        }

        // обновление значения переменной
        public void SetValue(string name, double value) {
            variables[name] = value;
        }

        // вычисление выражения
        public double Evaluate() {
            Stack<double> stack = new Stack<double>();

            foreach (string lexeme in rpn) {
                if (this.IsOperator(lexeme)) {
                    if (stack.Count < 2)
                        throw new Exception("Unable to evaluate operator '" + lexeme + "'");

                    double arg2 = stack.Pop();
                    double arg1 = stack.Pop();

                    stack.Push(operators[lexeme](arg1, arg2));
                }
                else if (IsFunction(lexeme)) {
                    if (stack.Count < 1)
                        throw new Exception("Unable to evaluate function '" + lexeme + "'");

                    double arg = stack.Pop();
                    stack.Push(functions[lexeme](arg));
                }
                else if (IsBinaryFunction(lexeme)) {
                    if (stack.Count < 2)
                        throw new Exception("Unable to evaluate function '" + lexeme + "'");

                    double arg2 = stack.Pop();
                    double arg1 = stack.Pop();

                    stack.Push(this.binaryFunctions[lexeme](arg1, arg2));
                }
                else if (lexeme == "!") {
                    if (stack.Count < 1)
                        throw new Exception("Unable to evaluate unary minus");

                    stack.Push(-stack.Pop());
                }
                else if (IsConstant(lexeme)) {
                    stack.Push(constants[lexeme]);
                }
                else if (IsVariable(lexeme)) {
                    stack.Push(variables[lexeme]);
                }
                else if (IsNumber(lexeme)) {
                    stack.Push(double.Parse(lexeme.Replace(".", ",")));
                }
                else
                    throw new Exception("Unknown rpn lexeme '" + lexeme + "'");
            }

            if (stack.Count != 1)
                throw new Exception("Incorrect expression");

            return stack.Pop();
        }
    }
}
