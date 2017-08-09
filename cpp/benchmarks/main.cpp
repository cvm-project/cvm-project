#include <iostream>
#include <chrono>
#include <cmath>
#include "immintrin.h"
//#include "avxintrin.h"

#define MAX 1l<<32
using namespace std::chrono;

double add(double a, double b) {
    return a + b;
}

double simple_sum(double *array) {
    double acc = 0;

    for (size_t i = 0; i < MAX; i++) {
        acc += array[i];
    }
    return acc;
}

double unrolled_sum(double *array) {
    double acc[] = {0.0, 0.0, 0.0, 0.0};
    for (size_t i = 0; i < MAX; i += 4) {
        acc[0] += array[i];
        acc[1] += array[i + 1];
        acc[2] += array[i + 2];
        acc[3] += array[i + 3];
    }
    return acc[0] + acc[1] + acc[2] + acc[3];
}

double map_filter_reduce() {
    double v0 = 0.0;
    double acc = 0.0;
    while (v0 < MAX) {
        double t1[2] = {v0, v0 * 3 + 7};
        v0++;
        if (fmod(t1[0], 2) == 1.0)
            continue;
        acc = acc + t1[0];
    }
    return acc;
}


int main() {


    double *array = new double[MAX];
    for (size_t i = 0; i < MAX; i++) {
        array[i] = (double) rand() / RAND_MAX * 10;
    }

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    auto res = simple_sum(array);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    std::cout << res << std::endl;
    auto duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << duration << " simple sum\n";

    t1 = high_resolution_clock::now();
    res = unrolled_sum(array);
    t2 = high_resolution_clock::now();

    std::cout << res << std::endl;
    duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << duration << " unrolled sum\n";


//    t1 = high_resolution_clock::now();
//    vectorized_sum();
//    t2 = high_resolution_clock::now();
//
//    duration = duration_cast<milliseconds>(t2 - t1).count();
//    std::cout << duration << " vectorized sum\n";
//
//
//    t1 = high_resolution_clock::now();
//    vectorized_sum2();
//    t2 = high_resolution_clock::now();

//    duration = duration_cast<milliseconds>(t2 - t1).count();
//    std::cout << duration << " vectorized sum2\n";

//    t1 = high_resolution_clock::now();
//    map_filter_reduce();
//    t2 = high_resolution_clock::now();
//
//    duration = duration_cast<milliseconds>(t2 - t1).count();
//    std::cout << duration << " map filter reduce\n";

    return 0;
}
