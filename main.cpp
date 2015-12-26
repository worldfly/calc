#include <iostream>
#include <string>
#include <cmath>
#include <ctype.h>
#include <vector>

using namespace std;

const char quit = 'q'; // лексема выхода
const char print = ';'; // лексема вычисления результата
const char number = '0'; // лексема числа
const char let = 'L'; // лексема let
const char var_name = 'a'; // лексема name
const char llog = 'l'; // лексема логарифма
const char lsin = 's'; // лексема синуса
const char lcos = 'c'; // лексема косинуса
const string prompt = "> ";
const string result = "= ";
const string declare = "let";

/* Exception class */
/* Обертка над исключениями для удобства */
class Exception {
public:
    string err;
    Exception(string s) : err(s){};
    string what() {
        return err;
    };
};

void error(string s) {
    throw Exception(s);
}
/* end of exception class */

/* Token class */
class Token {
public:
    char kind; /* вид лексемы */
    double value;
    string name;
    
    Token(char ch) : kind(ch), value(0){};
    Token(char ch, double val) : kind(ch), value(val){};
    Token(char ch, string s) : kind(ch), name(s){};
};

/* Класс для считывания потока лексем */
class Token_stream {
public:
    Token_stream();
    Token get();
    void putback(Token t);
    void ignore(char c);

private:
    bool full;
    Token buffer;
};

/* конструктор */
Token_stream::Token_stream() : full(false), buffer(0) {} /* по-умолчанию в буфере нет объектов */

/* возвращает лексему назад в поток */
void Token_stream::putback(Token t) {
    if (full) {
        error("putback() buffer is full");
    }

    buffer = t;
    full = true;
}

/* считываем символы и возвращает лексему */
Token Token_stream::get() {
    if (full) {
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;
    switch (ch) {
    case print:
    case quit:
    case '^':
    case '{': case '}': case '(': case ')': case '+':
    case '-': case '*': case '/': case '%': case '!':
        return Token(ch);

    case '.': case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7': case '8': case '9': {
        cin.putback(ch); /* возвращаем цифру в стандартный поток для считывания целого числа через >> */
        double val;
        cin >> val;
        return Token(number, val); /* number - вид лексемы, обознчающий число */

    case '=':
        return Token('=');
    }

    default:
        if (isalpha(ch)) { 
            cin.putback(ch);
            string s;
            // считываем имя переменной
            // содержит буквы, цифры, знак подчеркивания
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))
                s += ch;
            cin.putback(ch);
            if (s == declare) return Token(let); // ключевое слово let
            if (s == "log") return Token(llog);
            if (s == "cos") return Token(lcos);
            if (s == "sin") return Token(lsin);
            return Token(var_name, s);
        }
        error("bad token");
    }
}

/* отбрасывает символы, предшествующие символу c включительно */
void Token_stream::ignore(char c) {
    if (full && c == buffer.kind) { // сначала проверяем буфер
        full = false;
        return;
    }
    full = false;
    char ch = 0;
    while (cin >> c)
        if (ch == c) return;
}
/* end of Token class */

/* Variable class */
class Variable {
public:
    Variable();
    string name;
    double value;
    Variable(string n, double v) : name(n), value(v) {}
};

vector<Variable> var_table;

/* возвращает значение переменной с именем s */
double get_value(string s) {
    for (int i = 0; i < var_table.size(); ++i) {
        if (var_table[i].name == s) return var_table[i].value;
    }
    error("get: undefined variable "+s);
}

/* устанавливает значение d переменной s */
void set_value(string s, double d) {
    for (int i = 0; i < var_table.size(); ++i) {
        if (var_table[i].name == s) {
            var_table[i].value = d;
            return;
        }
    }
    error("set: undefined value "+s);
}

/* проверяет, определена ли переменная */
bool is_declared(string var) {
    for (int i = 0; i < var_table.size(); ++i) {
        if (var_table[i].name == var) return true;
    }
    return false;
}

/* определяет переменную var со значением val */
double define_name(string var, double val) {
    if (is_declared(var))
        error(var+" declared twice");
    var_table.push_back( Variable(var, val) );
    return val;
}
/* end of Variable class */

/* функция факториала */
double fact(double n) {
    double f = 1;
    for (int i = 1; i <= n; i++) {
        f *= i;
    }

    return f;
}

Token_stream ts;
double expression(); // определено ниже

/* первичное выражение */
double primary() {
    Token t = ts.get();
    switch (t.kind) {
    case '(': {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' expected");
        return d;
    }

    case '{': {
        double d = expression();
        t = ts.get();
        if (t.kind != '}') error("'}' expected");
        return d;
    }

    case '-':
        return -primary();

    case '+':
        return primary();

    case var_name:
        return get_value(t.name);
    case number:
        return t.value;
        
    case llog:
        return log(primary());
    case lsin:
        return sin(primary());
    case lcos:
        return cos(primary());

    default:
        error("primary expected");
    }
}

/* постфиксное выражение */
double postfix() {
    double left = primary();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '!':
            left = fact(left);
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}

/* терм */
double term() {
    double left = postfix();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '*':
            left *= postfix();
            t = ts.get();
            break;

        case '/': {
            double d = postfix();
            if (d == 0) {
                error("divide by zero");
            }
            left /= d;
            t = ts.get();
            break;
        }

        case '%': {
            int a = int(left);
            if (a != left) {
                error("left operand of % isn't an integer");
            }
            double d = term();
            int b = int(d);
            if (b != d) {
                error("right operand of % isn't an integer");
            }
            if (b == 0) {
                error("divide by zero");
            }
            left = a % b;
            return left;
            //ts.putback(t);
            break;
        }

        case '^': {
            double d = postfix();
            left = pow(left, d);
            t = ts.get();
            break;
        }

        default:
            ts.putback(t);
            return left;
        }
    }
}

/* выражение */
double expression() {
    double left = term(); /* считываем и вычисляем терм */
    Token t = ts.get(); /* считываем следующую лексему из потока */

    while (true) {
        switch (t.kind) {
        case '+':
            left += term();
            t = ts.get();
            break;

        case '-':
            left -= term();
            t = ts.get();
            break;

        default:
            ts.putback(t);  /* возвращаем лексему обратно в поток */
            return left; /* получившееся выражение - терм, возвращаем его */
        }
    }
}

/* объявление */
double declaration() {
    Token t = ts.get();
    if (t.kind != var_name) error("expected let in variable declaration");

    string var_name1 = t.name;

    Token t2 = ts.get();
    if (t2.kind != '=') error("missed symbol = in declaration of "+var_name1+" variable");

    double d = expression();
    define_name(var_name1, d);
    return d;
}

/* инструкция */
double statement() {
    Token t = ts.get();
    switch (t.kind) {
    case let:
        return declaration();
    default:
        ts.putback(t);
        return expression();
    }
}

/* вычисления */
int calculate() {
    cout << prompt;
    Token t = ts.get();
    while (t.kind == print) {
        t = ts.get();
    }
    if (t.kind == quit) {
        return 1;
    }

    ts.putback(t);
    cout << result << statement() << endl;
    return 0; // continue
}

int main(int argc, char* argv[]) {
    define_name("pi",3.141592653);
    define_name("e" ,2.718281828);
    while (cin) {
        try {
            int exit = calculate();
            if (exit) return 0;
        } catch (Exception& e) {
            cerr << e.what() << endl;
            ts.ignore(print); // игнорируем поток символов с ошибкой до символа печати
        } catch (...) {
            return 2;
        }
    }

    return 0;
}

/* Грамматика
Вычисление:
    Инструкция
    Печать
    Выход   
    Инструкция вычисления
Инструкция:
    Объявление
    Выражение
Объявление:
    "let" Имя "=" Выражение
Печать:
    ;
Выход:
    q
Выражение:
    Терм
    Выражение "+" Терм
    Выражение "-" Терм
Терм:
    Постфикс
    Терм "*" Постфикс // умножение
    Терм "/" Постфикс // деление
    Терм "%" Постфикс // остаток (деление по модулю)
Постфикс:
    Первичное выражение "!"
Первичное выражение:
    Имя
    Число
    "(" Выражение ")"
    "-" Первичное выражение // унарный минус - отрицательное число
    "+" Первичное выражение // унарный плюс
    "log" Первичное выражение
    "sin" Первичное выражение
    "cos" Первичное выражение
Число:
    литерал_с_плавающей_точкой

*/
