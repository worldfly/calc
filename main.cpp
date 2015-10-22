#include <iostream>

bool isDigit (char b) {
    if (b == '0' ||  b == '1' || b == '2' || b == '3' || b == '4' || b == '5' ||
    b == '6' || b == '7' || b == '8' || b == '9') {
        return true;
    } else {
        return false;
    }
}

int main () {
    std::string a;
    std::cin >> a;
    std::string::iterator it = a.begin();
    std::cout << isDigit(*it);

    return 0;
}
