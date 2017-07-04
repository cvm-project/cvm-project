#include <iostream>
#include <chrono>
#include <cmath>
#include "immintrin.h"
//#include "avxintrin.h"

#define MAX 1l<<28
using namespace std::chrono;

double add(double a, double b) {
    return a + b;
}

double simple_sum() {
    double acc = 0;

    for (double i = 0; i < MAX; i++) {
        acc = add(acc, i);
    }
    return acc;
}

double unrolled_sum() {
    double acc[] = {0.0, 0.0, 0.0, 0.0};
    for (double i = 0; i < MAX; i += 4) {
        acc[0] = add(acc[0], i);
        acc[1] = add(acc[1], i + 1);
        acc[2] = add(acc[2], i + 2);
        acc[3] = add(acc[3], i + 3);
    }
    return acc[0] + acc[1] + acc[2] + acc[3];
}

//double vectorized_sum() {
//
//    double acc[4] = {0.0, 0.0, 0.0, 0.0};
//    __m256d acci = _mm256_load_pd(acc);
//    for (double i = 0; i < MAX; i += 8) {
//        //load
//        double d[4] = {i, i, i, i};
//        double inc1[4] = {0.0, 1.0, 2.0, 3.0};
//        double inc2[4] = {4.0, 5.0, 6.0, 7.0};
//        __m256d di = _mm256_load_pd(d);
//        __m256d inc1i = _mm256_load_pd(inc1);
//        __m256d inc2i = _mm256_load_pd(inc2);
//
//        __m256d ad1i = _mm256_add_pd(di, inc1i);
//        __m256d ad2i = _mm256_add_pd(di, inc2i);
//        __m256d res1i = _mm256_add_pd(ad1i, ad2i);
//
//        acci = _mm256_add_pd(acci, res1i);
//    }
//
//    _mm256_store_pd(acc, acci);
//    return acc[0] + acc[1] + acc[2] + acc[3];
//
//}

//double vectorized_sum2() {
//
//    double acc1[8] = {0.0, 0.0, 0.0, 0.0};
//    double acc2[8] = {0.0, 0.0, 0.0, 0.0};
//    double zeros[4] = {0.0, 0.0, 0.0, 0.0};
//    __m256d acci1 = _mm256_load_pd(zeros);
//    __m256d acci2 = _mm256_load_pd(zeros);
//    for (double i = 0; i < MAX; i += 16) {
//
//        double d[4] = {i, i, i, i};
//        double inc1[4] = {0.0, 1.0, 2.0, 3.0};
//        double inc2[4] = {4.0, 5.0, 6.0, 7.0};
//        __m256d di = _mm256_load_pd(d);
//        __m256d inc1i = _mm256_load_pd(inc1);
//        __m256d inc2i = _mm256_load_pd(inc2);
//
//        __m256d ad1i = _mm256_add_pd(di, inc1i);
//        __m256d ad2i = _mm256_add_pd(di, inc2i);
//        __m256d res1i = _mm256_add_pd(ad1i, ad2i);
//
//        acci1 = _mm256_add_pd(acci1, res1i);
//
//        double inc11[4] = {0.0, 1.0, 2.0, 3.0};
//        double inc22[4] = {4.0, 5.0, 6.0, 7.0};
//        di = _mm256_load_pd(d);
//        inc1i = _mm256_load_pd(inc11);
//        inc2i = _mm256_load_pd(inc22);
//
//        ad1i = _mm256_add_pd(di, inc1i);
//        ad2i = _mm256_add_pd(di, inc2i);
//        res1i = _mm256_add_pd(ad1i, ad2i);
//
//        acci2 = _mm256_add_pd(acci2, res1i);
//
//    }
//
//    _mm256_store_pd(acc1, acci1);
//    _mm256_store_pd(acc2, acci2);
//    return acc1[0] + acc1[1] + acc1[2] + acc1[3] + acc2[0] + acc2[1] + acc2[2] + acc2[3];
//
//}

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

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    simple_sum();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << duration << " simple sum\n";

    t1 = high_resolution_clock::now();
    unrolled_sum();
    t2 = high_resolution_clock::now();

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

    duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << duration << " vectorized sum2\n";

    t1 = high_resolution_clock::now();
    map_filter_reduce();
    t2 = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << duration << " map filter reduce\n";

    return 0;
}
