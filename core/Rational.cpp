#include "Rational.hpp"
#include<cassert>
#include<cmath>
//initialize
Rational::Rational(int numer,int denom):numer_{numer},denom_{denom}{
    assert(denom!=0);
    normalize();
}

Rational::Rational(int number){
    numer_=number;
    denom_=1;
}

Rational::Rational(Rational const &a){
    numer_=a.numer_;
    denom_=a.denom_;
}

Rational::Rational(){
    numer_=0;
    denom_=1;
}

//get
int Rational::numer() const{
    return numer_;
}

int Rational::denom() const{
    return denom_;
}

//change
void Rational::numer(int a){
    numer_=a;
}

void Rational::denom(int a){
    assert(a!=0);
    denom_=a;
}

//normalize
int Rational::GCD() const{
    int a = std::abs(numer_);
    int b = std::abs(denom_);
    while (b!=0){
        int r=a%b;
        a=b;
        b=r;
    }
    return a;
}
Rational & Rational::normalize(){
    int g = GCD();
    numer_ /= g;
    denom_ /= g;

    if (denom_ < 0) {
        numer_ = -numer_;
        denom_ = -denom_;
    }
    return *this;
}


// operator overload(change itself)
Rational &Rational::operator=(Rational const &a)
{
    numer_=a.numer_;
    denom_=a.denom_;
    return *this;
}

Rational &Rational::operator+=(Rational const &a){
    Rational c{numer_ * a.denom_ + a.numer_ * denom_,denom_ * a.denom_};
    (*this)=c;
    return *this;
}

Rational &Rational::operator-=(Rational const &a){
    Rational c{numer_ * a.denom_ - a.numer_ * denom_,denom_ * a.denom_};
    (*this)=c;
    return *this;
}

Rational &Rational::operator*=(Rational const &a){
    Rational c{numer_*a.numer_,denom_*a.denom_};
    (*this)=c;
    return *this;
}

Rational &Rational::operator/=(Rational const &a){
    assert(a.numer_!=0);
    Rational c{a.denom_,a.numer_};
    (*this)*=c;
    return *this;
}

//friend cout
std::ostream &operator<<(std::ostream &out, Rational const &a){
    if(a.denom_==1){
        out<<a.numer_;
    }else if(a.numer_==0){
        out<<'0';
    }else{
        out<<a.numer_<<'/'<<a.denom_;
    }
    return out;
}

std::istream &operator>>(std::istream &in, Rational &a){
    int n=0;
    int d=1;
    char slash=0;
    in>>n;

    if(!in){
        return in;
    }
    if(in.peek()=='/'){
        in>>slash>>d;
        assert(d!=0);
    }else{
        d=1;
    }
    a.numer_=n;
    a.denom_=d;
    a.normalize();

    return in;
}

//friend operator
Rational operator+(Rational const &b, Rational const &a){
    Rational c{b};
    c+=a;
    return c;
}

Rational operator-(Rational const &b, Rational const &a){
    Rational c{b};
    c-=a;
    return c;
}

Rational operator- (Rational const &a){
    return Rational(-a.numer(), a.denom());
}

Rational operator*(Rational const &b, Rational const &a){
    Rational c{b};
    c*=a;
    return c;
}

Rational operator/(Rational const &b, Rational const &a){
    Rational c{b};
    c/=a;
    return c;
}
//firend bool
bool operator==(Rational const &a, Rational const &b){
    return a.numer()==b.numer() && a.denom()==b.denom();
}
bool operator!=(Rational const &a, Rational const &b){
    return !(a==b);
}
bool operator> (Rational const &a, Rational const &b){
    return a.numer_*b.denom_>b.numer_*a.denom_;
}
bool operator< (Rational const &a, Rational const &b){
    return a.numer_*b.denom_<b.numer_*a.denom_;
}
bool operator>=(Rational const &a, Rational const &b){
    return !(a<b);
}
bool operator<=(Rational const &a, Rational const &b){
    return !(a>b);
}

//遇到的问题
//1，成员函数其实是a.函数
//2，改变自己的都是返回应用，是复制的话不能嵌套使用
//3. abs
//4. 成员函数叫自己this的函数直接用，不用（*this）.函数
//5. 不是怎样不出错，而是怎么让错误不存在
//6. 应为friend不是成员函数，声明的时候不用声明class的类
//7.怎么输出