#ifndef COMPLEXNUM_H
#define COMPLEXNUM_H

#include <iostream>

struct Complex {
    double real;
    double imag;

    Complex(double r = 0.0, double i = 0.0);

    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(double scalar) const;

    bool operator<(const Complex& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Complex& c);

    double abs() const;
};


#endif
