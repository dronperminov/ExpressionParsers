#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <stack>

using namespace std;

class ExpressionParser {
    vector<string> lexemes; // лексемы
    vector<string> rpn; // польская запись
    map<string, double> variables; // переменные

    bool IsDigit(char c) const; // проверка на цифру
    bool IsLetter(char c) const; // проверка на букву
    void SplitToLexemes(const string& s); // разбиение выражения на лексемы

    bool IsFunction(const string& lexeme) const; // проверка на функцию
    bool IsBinaryFunction(const string& lexeme) const; // проверка на бинарную функцию
    bool IsOperator(const string& lexeme) const; // проверка на операцию
    bool IsConstant(const string& lexeme) const; // проверка на константу
    bool IsNumber(const string& lexeme) const; // проверка на число
    bool IsVariable(const string& lexeme) const; // проверка на переменную

    int GetPriority(const string lexeme) const; // получение приоритета операции
    bool IsMorePriority(const string &curr, const string &top) const; // проверка, что текущая лексема менее приоритетна лексемы на вершине стека
    void ConvertToRPN(); // получение польской записи
    
    double EvaluateOperator(const string& op, double arg1, double arg2) const; // вычисление операции
    double EvaluateFunction(const string& f, double arg) const; // вычисление функции
    double EvaluateBinaryFunction(const string& f, double arg1, double arg2) const; // вычисление бинарной функции
    double EvaluateConstant(const string& name) const; // вычисление константы
public:
    ExpressionParser(const string& expression); // конструктор из выражения

    void SetValue(string name, double value); // обновление значения переменной
    double Evaluate(); // вычисление выражения
};

// проверка на цифру
bool ExpressionParser::IsDigit(char c) const {
    return c >= '0' && c <= '9';
}

// проверка на букву
bool ExpressionParser::IsLetter(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// разбиение выражения на лексемы
void ExpressionParser::SplitToLexemes(const string& s) {
    size_t i = 0; // индекс в строке

    while (i < s.length()) {
        if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '%' || s[i] == '^') {
            lexemes.push_back(string(1, s[i++])); // кладём операцию
        }
        else if (s[i] == '(' || s[i] == ')' || s[i] == ',') {
            lexemes.push_back(string(1, s[i++])); // кладём скобку или разделитель
        }
        else if (IsDigit(s[i])) { // если цифра
            string number = ""; // строка для числа
            int points = 0; // счётчик точек

            while (i < s.length() && (IsDigit(s[i]) || s[i] == '.')) {
                if (s[i] == '.') {
                    points++;

                    if (points > 1)
                        throw string("Invalid real number in expression");
                }

                number += s[i++]; // наращиваем число
            }

            lexemes.push_back(number); // добавляем число
        }
        else if (IsLetter(s[i])) { // если буква
            string word = ""; // строка для слова

            while (i < s.length() && (IsLetter(s[i]) || IsDigit(s[i])))
                word += s[i++]; // наращиваем слово

            lexemes.push_back(word); // добавляем слово
        }
        else if (s[i] == ' ' || s[i] == '\t') { // если пробельный символ
            i++; // пропускаем
        }
        else // иначе незивестный символ в выражении
            throw string("Unknown character in expression: '") + s[i] + "'";
    }
}

// проверка на функцию
bool ExpressionParser::IsFunction(const string& lexeme) const {
    if (lexeme == "sin" || lexeme == "cos" || lexeme == "tan" || lexeme == "tg" || lexeme == "cot" || lexeme == "ctg")
        return true;

    if (lexeme == "sinh" || lexeme == "sh" || lexeme == "cosh" || lexeme == "ch" || lexeme == "tanh" || lexeme == "th")
        return true;

    if (lexeme == "asin" || lexeme == "arcsin" || lexeme == "acos" || lexeme == "arccos" || lexeme == "arctg" || lexeme == "atan")
        return true;

    if (lexeme == "ln" || lexeme == "log2" || lexeme == "lg" || lexeme == "exp")
        return true;

    if (lexeme == "sqrt" || lexeme == "cbrt" || lexeme == "abs" || lexeme == "sign")
        return true;

    return false;
}

// проверка на бинарную функцию
bool ExpressionParser::IsBinaryFunction(const string& lexeme) const {
    return lexeme == "max" || lexeme == "min" || lexeme == "log" || lexeme == "pow" || lexeme == "root";
}

// проверка на операцию
bool ExpressionParser::IsOperator(const string& lexeme) const {
    return lexeme == "+" || lexeme == "-" || lexeme == "*" || lexeme == "/" || lexeme == "%" || lexeme == "^";
}

// проверка на константу
bool ExpressionParser::IsConstant(const string& lexeme) const {
    return lexeme == "pi" || lexeme == "e" || lexeme == "ln2" || lexeme == "ln10" || lexeme == "sqrt2";
}

// проверка на число
bool ExpressionParser::IsNumber(const string& lexeme) const {
    for (size_t i = 0; i < lexeme.length(); i++)
        if (!IsDigit(lexeme[i]) && lexeme[i] != '.')
            return false;

    return true;
}

// проверка на переменную
bool ExpressionParser::IsVariable(const string& lexeme) const {
    if (!IsLetter(lexeme[0]))
        return false;

    for (size_t i = 1; i < lexeme.length(); i++)
        if (!IsLetter(lexeme[i]) && !IsDigit(lexeme[i]))
            return false;

    return !IsFunction(lexeme) && !IsBinaryFunction(lexeme) && !IsConstant(lexeme);
}

// получение приоритета операции
int ExpressionParser::GetPriority(const string lexeme) const {
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
bool ExpressionParser::IsMorePriority(const string &curr, const string &top) const {
    if (curr == "^" || curr == "!")
        return GetPriority(top) > GetPriority(curr);

    return GetPriority(top) >= GetPriority(curr);
}

// получение польской записи
void ExpressionParser::ConvertToRPN() {
    stack<string> stack;
    bool mayUnary = true;

    for (string lexeme : lexemes) {
        if (IsNumber(lexeme) || IsConstant(lexeme)) {
            rpn.push_back(lexeme);
            mayUnary = false;
        }
        else if (IsFunction(lexeme) || IsBinaryFunction(lexeme)) {
            stack.push(lexeme);
            mayUnary = true;
        }
        else if (IsVariable(lexeme)) {
            rpn.push_back(lexeme);
            variables[lexeme] = 0;
            mayUnary = false;
        }
        else if (lexeme == ",") {
            while (stack.size() > 0 && stack.top() != "(") {
                rpn.push_back(stack.top());
                stack.pop();
            }

            if (stack.size() == 0)
                throw string("Incorrect expression");
        }
        else if (IsOperator(lexeme)) {
            string curr = lexeme == "-" && mayUnary ? "!" : lexeme;

            while (stack.size() > 0 && IsMorePriority(curr, stack.top())) {
                rpn.push_back(stack.top());
                stack.pop();
            }

            stack.push(curr);
            mayUnary = lexeme == "^";
        }
        else if (lexeme == "(") {
            stack.push(lexeme);
            mayUnary = true;
        }
        else if (lexeme == ")") {
            while (stack.size() > 0 && stack.top() != "(") {
                rpn.push_back(stack.top());
                stack.pop();
            }

            if (stack.size() == 0)
                throw string("Incorrect expression: brackets are disbalanced");

            stack.pop();

            if (stack.size() > 0 && IsFunction(stack.top())) {
                rpn.push_back(stack.top());
                stack.pop();
            }

            mayUnary = false;
        }
        else
            throw string("Incorrect expression: unknown lexeme '") + lexeme + "'";
    }

    while (stack.size() > 0) {
        if (stack.top() == "(")
            throw string("Incorrect expression: brackets are disbalanced");

        rpn.push_back(stack.top());
        stack.pop();
    }
}

// вычисление операции
double ExpressionParser::EvaluateOperator(const string& op, double arg1, double arg2) const {
    if (op == "+")
        return arg1 + arg2;

    if (op == "-")
        return arg1 - arg2;

    if (op == "*")
        return arg1 * arg2;

    if (op == "/")
        return arg1 / arg2;

    if (op == "%")
        return fmod(arg1, arg2);

    if (op == "^")
        return pow(arg1, arg2);

    throw string("Unhandled operator '") + op + "'";
}

// вычисление функции
double ExpressionParser::EvaluateFunction(const string& f, double arg) const {
    if (f == "sin")
        return sin(arg);

    if (f == "cos")
        return cos(arg);

    if (f == "tan" || f == "tg")
        return tan(arg);

    if (f == "cot" || f == "ctg")
        return 1.0 / tan(arg);

    if (f == "sinh" || f == "sh")
        return sinh(arg);

    if (f == "cosh" || f == "ch")
        return cosh(arg);

    if (f == "tanh" || f == "th")
        return tanh(arg);

    if (f == "asin" || f == "arcsin")
        return asin(arg);

    if (f == "acos" || f == "arccos")
        return acos(arg);

    if (f == "atan" || f == "arctg")
        return atan(arg);

    if (f == "ln")
        return log(arg);

    if (f == "log2")
        return log2(arg);

    if (f == "lg")
        return log10(arg);

    if (f == "exp")
        return exp(arg);

    if (f == "sqrt")
        return sqrt(arg);

    if (f == "cbrt")
        return cbrt(arg);

    if (f == "abs")
        return fabs(arg);

    if (f == "sign")
        return arg > 0 ? 1 : (arg < 0 ? -1 : 0);

    throw string("Unhandled function '") + f + "'";
}

// вычисление бинарной функции
double ExpressionParser::EvaluateBinaryFunction(const string& f, double arg1, double arg2) const {
    if (f == "max")
        return max(arg1, arg2);

    if (f == "min")
        return min(arg1, arg2);

    if (f == "log")
        return log(arg2) / log(arg1);

    if (f == "pow")
        return pow(arg1, arg2);

    if (f == "root")
        return pow(arg2, 1.0 / arg1);

    throw string("Unhandled binary function '") + f + "'";
}

// вычисление константы
double ExpressionParser::EvaluateConstant(const string& name) const {
    if (name == "pi")
        return M_PI;

    if (name == "e")
        return M_E;

    if (name == "ln2")
        return log(2);

    if (name == "ln10")
        return log(10);

    if (name == "sqrt2")
        return sqrt(2);

    throw string("Unhandled constant '") + name + "'";
}

// конструктор из выражения
ExpressionParser::ExpressionParser(const string& expression) {
    SplitToLexemes(expression); // разбиваем на лексемы
    ConvertToRPN(); // получаем польскую запись
}

// обновление значения переменной
void ExpressionParser::SetValue(string name, double value) {
    variables[name] = value;
}

// вычисление выражения
double ExpressionParser::Evaluate() {
    stack<double> stack;

    for (string lexeme : rpn) {
        if (IsOperator(lexeme)) {
            if (stack.size() < 2)
                throw string("Unable to evaluate operator '") + lexeme + "'";

            double arg2 = stack.top();
            stack.pop();
            double arg1 = stack.top();
            stack.pop();

            stack.push(EvaluateOperator(lexeme, arg1, arg2));
        }
        else if (IsFunction(lexeme)) {
            if (stack.size() < 1)
                throw string("Unable to evaluate function '") + lexeme + "'";

            double arg = stack.top();
            stack.pop();
            stack.push(EvaluateFunction(lexeme, arg));
        }
        else if (IsBinaryFunction(lexeme)) {
            if (stack.size() < 2)
                throw string("Unable to evaluate function '") + lexeme + "'";

            double arg2 = stack.top();
            stack.pop();
            double arg1 = stack.top();
            stack.pop();

            stack.push(EvaluateBinaryFunction(lexeme, arg1, arg2));
        }
        else if (lexeme == "!") {
            if (stack.size() < 1)
                throw string("Unable to evaluate unary minus");

            double arg = stack.top();
            stack.pop();
            stack.push(-arg);
        }
        else if (IsConstant(lexeme)) {
            stack.push(EvaluateConstant(lexeme));
        }
        else if (IsVariable(lexeme)) {
            stack.push(variables[lexeme]);
        }
        else if (IsNumber(lexeme)) {
            stack.push(stod(lexeme));
        }
        else
            throw string("Unknown rpn lexeme '") + lexeme + "'";
    }

    if (stack.size() != 1)
        throw string("Incorrect expression");

    return stack.top();
}