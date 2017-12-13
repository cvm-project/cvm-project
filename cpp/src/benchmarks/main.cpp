#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "utils/timing.h"

#define MAX 1l << 29
using namespace std;

template <typename T>
T *realloc_or_throw(T *old_ptr, const size_t n_elements) {
    void *ptr = realloc(old_ptr, sizeof(T) * n_elements);
    if (ptr == nullptr) throw std::bad_alloc();
    return reinterpret_cast<T *>(ptr);
}

double my_add(double a, double b) { return a + b; }

double simple_sum(const double *const array) {
    TICK1
    double acc = 0;
    for (size_t j = 0; j < 1; j++) {
        for (size_t i = 0; i < MAX; i++) {
            acc = my_add(acc, array[i]);
        }
    }
    TOCK1
    std::cout << "outer loop sum " << DIFF1 << std::endl;
    return acc;
}

double unrolled_sum(const double *const array) {
    TICK2
    double acc[] = {0.0, 0.0, 0.0, 0.0};
    for (size_t j = 0; j < 1; j++) {
        for (size_t i = 0; i < MAX; i += 4) {
            acc[0] += array[i];
            acc[1] += array[i + 1];
            acc[2] += array[i + 2];
            acc[3] += array[i + 3];
        }
    }
    TOCK2
    std::cout << DIFF2 << std::endl;
    return acc[0] + acc[1] + acc[2] + acc[3];
}

struct tuple_1 {
    double v0;
    double v1;
};

struct result_struct1 {
    size_t size;
    tuple_1 *data;
};

result_struct1 gap_filter(const double *const array) {
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_1 *result = reinterpret_cast<tuple_1 *>(
            malloc(sizeof(tuple_1) * allocatedSize));
    for (size_t i = 0; i < MAX; i++) {
        tuple_1 t1 = {array[i], array[i] * 3 + 7};
        if (t1.v0 > 0.5) {
            continue;
        }
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = realloc_or_throw<tuple_1>(result, allocatedSize);
        }
        result[resSize] = t1;
        resSize++;
    }
    TOCK1
    std::cout << DIFF1 << " outer loop map filter\n";

    return {resSize, result};
}

double filter_sum(const double *const array) {
    double res = 0;
    TICK1
    for (size_t i = 0; i < MAX; i++) {
        if (array[i] > 0) {
            res += array[i];
            //            tot--;
        }
    }
    TOCK1
    std::cout << DIFF1 << " filter sum\n";
    return res;
}

double filter_sum2(const double *const array) {
    double res = 0;
    TICK1
    for (size_t i = 0; i < MAX; i++) {
        res += array[i] * static_cast<double>(array[i] > 0);
    }
    TOCK1
    std::cout << DIFF1 << " filter sum2\n";
    return res;
}

double filter_sum3(const double *const array) {
    double res = 0;
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    double *result =
            reinterpret_cast<double *>(malloc(sizeof(double) * allocatedSize));

    for (size_t i = 0; i < MAX; i++) {
        result[resSize] = array[i];
        resSize += static_cast<size_t>(array[i] > 0);
    }

    TOCK1
    free(result);
    std::cout << DIFF1 << " filter sum3\n";
    return res;
}

tuple_1 map_1(double a) { return {a, a * 3 + 7}; }

result_struct1 map(const double *array) {
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_1 *result = reinterpret_cast<tuple_1 *>(
            malloc(sizeof(tuple_1) * allocatedSize));

    for (size_t i = 0; i < MAX; i++) {
        tuple_1 t1 = map_1(array[i]);
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = realloc_or_throw<tuple_1>(result, allocatedSize);
        }
        result[resSize] = t1;
        resSize++;
    }
    TOCK1
    std::cout << DIFF1 << " outer loop map \n";

    return {resSize, result};
}

struct result_struct2 {
    size_t size;
    double *data;
};

result_struct2 filter(const double *const array) {
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    double *result = reinterpret_cast<double *>(malloc(sizeof(double) * MAX));

    for (size_t i = 0; i < MAX; i++) {
        if (array[i] * 100 > 50) {
            if (allocatedSize <= resSize) {
                allocatedSize *= 2;
                result = realloc_or_throw<double>(result, allocatedSize);
            }
            result[resSize] = array[i];
            resSize++;
        }
    }
    TOCK1
    std::cout << DIFF1 << " outer loop filter \n";

    return {resSize, result};
}

struct tuple_2 {
    int64_t v0;
    int64_t v1;
};

struct tuple_3 {
    int64_t v0;
    int64_t v1;
};

struct tuple_4 {
    int64_t v0;
    int64_t v1;
    int64_t v2;
};

struct result_struct {
    size_t size;
    tuple_4 *data;
};

size_t len2 = MAX >> 1;

result_struct join(tuple_2 *array1, tuple_3 *array2) {
    // build ht
    TICK1
    std::unordered_map<int64_t, vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        } else {
            vector<tuple_3> values;
            values.push_back(array2[i]);
            ht.emplace(key, values);
        }
    }
    TOCK1
    std::cout << DIFF1 << " build \n";

    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_4 *result = reinterpret_cast<tuple_4 *>(
            malloc(sizeof(tuple_4) * allocatedSize));

    for (size_t i = 0; i < MAX; i++) {
        auto key = array1[i].v0;
        if (ht.count(key) > 0) {
            for (auto t : ht[key]) {
                // build a tuple for every combination
                tuple_4 r = {key, array1[i].v1, t.v1};
                if (allocatedSize <= resSize) {
                    allocatedSize *= 2;
                    result = realloc_or_throw<tuple_4>(result, allocatedSize);
                }
                result[resSize] = r;
                resSize++;
            }
        } else {
            continue;
        }
    }
    result_struct ret;
    ret.data = result;
    ret.size = resSize;
    return ret;
}

result_struct map_filter_join(const int64_t *const array1,
                              const tuple_3 *const array2) {
    // build ht
    TICK1
    std::unordered_map<int64_t, vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        } else {
            vector<tuple_3> values;
            values.push_back(array2[i]);
            ht.emplace(key, values);
        }
    }
    TOCK1
    std::cout << DIFF1 << " build \n";

    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_4 *result = reinterpret_cast<tuple_4 *>(
            malloc(sizeof(tuple_4) * allocatedSize));

    for (size_t i = 0; i < MAX; i++) {
        tuple_2 t1 = {array1[i], array1[i] * 3 + 7};
        if (t1.v0 <= 0.5) {
            continue;
        }
        auto key = t1.v0;
        if (ht.count(key) > 0) {
            for (auto t : ht[key]) {
                // build a tuple for every combination
                tuple_4 r = {key, t1.v1, t.v1};
                if (allocatedSize <= resSize) {
                    allocatedSize *= 2;
                    result = realloc_or_throw<tuple_4>(result, allocatedSize);
                }
                result[resSize] = r;
                resSize++;
            }
        } else {
            continue;
        }
    }
    result_struct ret;
    ret.data = result;
    ret.size = resSize;
    TOCK1
    std::cout << DIFF1 << " prob \n";
    return ret;
}

struct tuple_5 {
    int64_t v0;
    int64_t v1;
};

struct result_struct_rbk {
    size_t size;
    tuple_5 *data;
};

result_struct_rbk map_reduce_by_key(const int64_t *const array) {
    // build ht
    TICK1
    std::unordered_map<int64_t, int64_t> ht;
    for (size_t i = 0; i < MAX; i++) {
        tuple_5 mapped = {array[i], 1};
        auto key = mapped.v0;
        auto it = ht.find(key);
        if (it != ht.end()) {
            auto t = it->second;
            it->second = t + mapped.v1;
        } else {
            ht.emplace(key, mapped.v1);
        }
    }

    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_5 *result = reinterpret_cast<tuple_5 *>(
            malloc(sizeof(tuple_5) * allocatedSize));

    for (auto it = ht.begin(); it != ht.end(); it++) {
        tuple_5 r = {it->first, it->second};
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = realloc_or_throw<tuple_5>(result, allocatedSize);
        }
        result[resSize] = r;
        resSize++;
    }
    result_struct_rbk ret;
    ret.data = result;
    ret.size = resSize;
    TOCK1
    std::cout << DIFF1 << " group by \n";
    return ret;
}

int main() {
    srand(time(NULL));
    double *array = reinterpret_cast<double *>(malloc((MAX) * sizeof(double)));

    for (size_t i = 0; i < MAX; i++) {
        array[i] = ((double)rand() / RAND_MAX) + 0.1;
    }
    //    auto res = simple_sum(array);
    //    cout << "sum result is " << res << endl;
    //
    //
    //    std::cout << res << std::endl;
    //    std::cout << duration << " simple sum\n";

    //    std::cout << res << std::endl;
    //    duration = duration_cast<milliseconds>(t2 - t1).count();
    //    std::cout << duration << " unrolled sum\n";

    //    auto res = map(array);
    //    free(array);
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //              << std::endl;

    auto res = filter_sum(array);
    std::cout << res << std::endl;

    res = filter_sum2(array);
    std::cout << res << std::endl;

    res = filter_sum3(array);
    std::cout << res << std::endl;

    //    srand(time(NULL));
    //    int64_t *array_rbk = new int64_t[MAX];
    //    for (size_t i = 0; i < MAX; i++) {
    //        array_rbk[i] = rand() % 100;
    //    }
    //    auto res = map_reduce_by_key(array_rbk);
    //    free(array);
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //              << std::endl;

    //    t1 = high_resolution_clock::now();
    //    auto res = filter(array);
    //    t2 = high_resolution_clock::now();
    //    free(array);
    //    std::cout << (res.data)[rand() % res.size] << " " << (res.data)[rand()
    //    % res.size] << " "
    //              << std::endl;

    //    t1 = high_resolution_clock::now();
    //    auto res = map_filter(array);
    //    t2 = high_resolution_clock::now();
    //    free(array);
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //              << std::endl;

    //
    //    tuple_2 *array1 = new tuple_2[MAX];
    //    for (size_t i = 0; i < MAX; i++) {
    //        array1[i] = {rand() % 1000, rand() % 1000};
    //    }
    //
    //    tuple_3 *array2 = new tuple_3[len2];
    //    for (size_t i = 0; i < len2; i++) {
    //        array2[i] = {rand() % 1000, rand() % 1000};
    //    }
    ////
    //    TICK1
    //    auto res = join(array1, array2);
    //    TOCK1
    //
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //            << (res.data)[rand() % res.size].v2 << std::endl;
    //    std::cout << DIFF1 << " whole join\n";
    //    free(array1);
    //    free(array2);

    //    int64_t *array1 = new int64_t[MAX];
    //    for (size_t i = 0; i < MAX; i++) {
    //        array1[i] = rand() % 1000;
    //    }
    //
    //    tuple_3 *array2 = new tuple_3[len2];
    //    for (size_t i = 0; i < len2; i++) {
    //        array2[i] = {rand() % 1000, rand() % 1000};
    //    }
    //////
    //    TICK1
    //    auto res = map_filter_join(array1, array2);
    //    TOCK1
    ////
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //              << (res.data)[rand() % res.size].v2 << std::endl;
    //    std::cout << DIFF1 << " whole join\n";
    //    free(array1);
    //    free(array2);
    free(array);
    return 0;
}
