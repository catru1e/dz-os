#include <iostream>

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No arguments provided.\n";
        return 0;
    }

    std::cout << "Arguments in reverse order:\n";
    for (int i = argc - 1; i >= 1; --i) {
        std::cout << argv[i] << "\n";
    }
    return 0;
}