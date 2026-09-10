#ifndef RATIONAL_HPP
#define RATIONAL_HPP
#include <iostream>
class Rational{
public:
//initialize
    Rational(int numer,int denom);
    Rational(int number);
    Rational();
    Rational(Rational const &a);
//get
    int numer() const;
    int denom() const;
//change
    void numer(int a);
    void denom(int a);
//normalize
    int GCD() const;
    Rational &normalize();
//operator overload(change itself)
    Rational &operator= (Rational const &a);
    Rational &operator+=(Rational const &a);
    Rational &operator-=(Rational const &a);
    Rational &operator*=(Rational const &a);
    Rational &operator/=(Rational const &a);
private:
    int numer_;
    int denom_;
//friend in & out
    friend std::ostream &operator<< (std::ostream & out,Rational const &a);
    friend std::istream &operator>>(std::istream &in, Rational &a);

//friend operator
    friend Rational operator+ (Rational const &a, Rational const &b);
    friend Rational operator- (Rational const &a, Rational const &b);
    friend Rational operator- (Rational const &a);
    friend Rational operator* (Rational const &a, Rational const &b);
    friend Rational operator/ (Rational const &a, Rational const &b);
//friend bool
    friend bool operator==(Rational const &a, Rational const &b);
    friend bool operator!=(Rational const &a, Rational const &b);
    friend bool operator> (Rational const &a, Rational const &b);
    friend bool operator< (Rational const &a, Rational const &b);
    friend bool operator>=(Rational const &a, Rational const &b);
    friend bool operator<=(Rational const &a, Rational const &b);

};
#endif