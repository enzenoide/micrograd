#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <cmath>
#include <string>

class Value;
using Value_ptr = std::shared_ptr<Value>;

class Value {
public:
    double data;
    double grad;
    std::vector<Value_ptr> prev_;
    std::function<void()> _backward;
    std::string op;

    Value(double data, std::vector<Value_ptr> children = {}, std::string op = "") 
        : data(data), grad(0.0), prev_(children), _backward([](){}), op(op) {}

};

// operator+ lives OUTSIDE the class because it operates on Value_ptr (smart pointers)
// handling the + operation between Value_ptr
Value_ptr operator+(Value_ptr self, Value_ptr other) {
    Value_ptr out = std::make_shared<Value>(
        self->data + other->data, 
        std::vector<Value_ptr>{self, other}, 
        "+"
    );

    out->_backward = [self, other, out]() {
        self->grad += out->grad;
        other->grad += out->grad;
    };
    
    return out;
}
//handling the + operation between a Value_ptr and a double
Value_ptr operator+(Value_ptr self, double other){
  return self+ std::make_shared<Value>(other);
}
//mirroring the handling between a value_ptr and a double
Value_ptr operator+(double self, Value_ptr other){
  return other + self;
}

//handling the * operation between Value_ptr
Value_ptr operator*(Value_ptr self, Value_ptr other){
  Value_ptr out = std::make_shared<Value>(self -> data * other -> data,std::vector<Value_ptr>{self,other},"*");
  out -> _backward = [self,other,out](){
    self -> grad += other -> data * out -> grad;
    other -> grad += self -> data * out -> grad;
  };
  return out;
}


//handling between Value_ptr and a double
Value_ptr operator*(Value_ptr self, double other){
  return self * std::make_shared<Value>(other);
}


//mirroring the handling between a value_ptr and a double
Value_ptr operator*(double self, Value_ptr other){
  return other * self;
}


Value_ptr operator-(Value_ptr self){
  return self * -1.0;
}
Value_ptr operator-(Value_ptr self, Value_ptr other){
  return self + (-other);
}
Value_ptr operator-(Value_ptr self, double other){
  return self + (-other);
}
Value_ptr operator-(double self, Value_ptr other){
  return self + (-other);
}

Value_ptr pow(Value_ptr self, double other) {
    
    Value_ptr out = std::make_shared<Value>(
        std::pow(self->data, other), 
        std::vector<Value_ptr>{self}, 
        "**" + std::to_string(other)
    );

    out->_backward = [self, other, out]() {
        self->grad += (other * std::pow(self->data, other - 1.0)) * out->grad;
    };

    return out;
}
Value_ptr exp(Value_ptr self){
  Value_ptr out = std::make_shared<Value>(std::exp(self -> data),std::vector<Value_ptr>{self},"exp");

  out -> _backward = [self,out](){
    self -> grad += out -> data * out -> grad;
  };
  return out;
}

int main() {
    //must create shared_ptrs, not raw objects
    Value_ptr a = std::make_shared<Value>(2.0);
    Value_ptr b = std::make_shared<Value>(3.0);
    
    auto c = a + b;
    
    std::cout << "Value of c: " << c->data << std::endl;
    
    return 0;
}
