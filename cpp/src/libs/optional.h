//
// Created by sabir on 18.07.17.
//

#ifndef CPP_OPTIONAL_H
#define CPP_OPTIONAL_H

template<typename T>
class Optional {
public:
    T value;
    bool initialized;

    Optional(T value) : value(value), initialized(1) {}

    Optional() : initialized(0) {}

    explicit operator bool() const { return (initialized); }

    operator T() const { return value; }
};


#endif //CPP_OPTIONAL_H
