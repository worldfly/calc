#include <iostream>
#include <string>

using namespace std;

const char quit = 'q';
const char print = ';';
const string prompt = "> ";
const string result = "= ";

/* Exception class */
class Exception {
public:
    string err;
    Exception(string s) : err(s){};
    string what() { return err; };
};

void error(string s) {
    throw Exception(s);
}
/* end of exception class */

/* Token class class */
class Token {
public:
    char kind;
    double value;

    Token(char ch) : kind(ch), value(0){};
    Token(char ch, double val) : kind(ch), value(val){};
};

class Token_stream {
public:
    Token_stream();
    Token get();
    void putback(Token t);

private:
    bool full;
    Token buffer;
};

Token_stream::Token_stream() : full(false), buffer(0) {}

void Token_stream::putback(Token t) {
    if (full) {
        error("putback() buffer is full");
    }

    buffer = t;
    full = true;
}

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
        case '{': case '}': case '(': case ')': case '+':
        case '-': case '*': case '/': case '%': case '!':
            return Token(ch);

        case '.': case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7': case '8': case '9': {
            cin.putback(ch);
            double val;
            cin >> val;
            return Token('8', val);
        }

        default:
            error("bad token");
    }
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

// double get_value(string e)
//{
// for (int i = 0; i < var_table.size(); ++i)
// if (var_table[i].name == s) return var_table[i].value;
// error ("get: undefined variable");
//}
/* end of Variable class */

double fact(double n) {
    double f = 1;
    for (int i = 1; i <= n; i++) {
        f *= i;
    }

    return f;
}

Token_stream ts;

double expression();

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
        case '8':
            return t.value;
        default:
            error("primary expected");
    }
}

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
                if (d == 0) error("divide by zero");
                left /= d;
                t = ts.get();
                break;
            }
            case '%': {
                double d = term();
                int i1 = int(left);
                if (i1 != left) error("left operand of % isn't an integer");
                int i2 = int(d);
                if (i2 != d) error("right operand of % isn't an integer");
                if (i2 == 0) error("divide by zero");
                left = i1 % i2;
                // return left;
                ts.putback(t);
                break;
            }
            default:
                ts.putback(t);
                return left;
        }
    }
}

double expression() {
    double left = term();
    Token t = ts.get();

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
                ts.putback(t);
                return left;
        }
    }
}

int main(int argc, char* argv[]) {
    while (cin) {
        try {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();
            if (t.kind == quit) return 0;

            ts.putback(t);
            cout << result << expression() << endl;
        } catch (Exception& e) {
            cerr << e.what() << endl;
        } catch (...) {
            return 2;
        }
    }

    return 0;
}

/*
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
    Число
    "(" Выражение ")"
    "-" Первичное выражение
    "+" Первичное выражение
Число:
    литерал_с_плавающей_точкой

*/
