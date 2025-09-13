#include <iostream>
#include "complexSort.h"

int main() {
    std::vector<Complex> numbers = {
        Complex(3, 4),
        Complex(1, 1),
        Complex(0, 2),
        Complex(-3, -4),
        Complex(5, 0)
    };

    std::cout << "Before sorting:\n";
    for (const auto& c : numbers)
        std::cout << c << "\n";

    sortComplexNumbers(numbers);

    std::cout << "\nAfter sorting:\n";
    for (const auto& c : numbers)
        std::cout << c << "\n";

    return 0;
}