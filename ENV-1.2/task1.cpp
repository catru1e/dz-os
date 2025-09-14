#include <iostream>
#include <cstdlib>

bool isInteger(const std::string& str) {
    if (str.empty())
        return false;

    int start = 0;
    if (str[0] == '-')
        start = 1;

    if (start == str.size())
        return false;

    for (int i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " a b c\n";
        return 1;
    }

    for (int i = 1; i < 4; ++i) {
        if (!isInteger(argv[i])) {
            std::cerr << "Error: '" << argv[i] << "' is not a valid integer.\n";
            return 1;
        }
    }

    int a = std::atoi(argv[1]);
    int b = std::atoi(argv[2]);
    int c = std::atoi(argv[3]);

    int minVal = a;
    int maxVal = a;

    if (b < minVal) minVal = b;
    if (c < minVal) minVal = c;

    if (b > maxVal) maxVal = b;
    if (c > maxVal) maxVal = c;

    std::cout << "min-" << minVal << ", max-" << maxVal << "\n";

    return 0;
}