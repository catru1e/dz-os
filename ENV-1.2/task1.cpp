#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " a b c\n";
        return 1;
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