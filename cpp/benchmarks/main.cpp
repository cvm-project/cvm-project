#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "../src/utils/timing.cpp"


#define MAX 1l<<18
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

double add(double a, double b) {
    return a + b;
}

double simple_sum(double *array) {
    TICK1
    double acc = 0;
    for (size_t j = 0; j < 1; j++) {
        for (size_t i = 0; i < MAX; i++) {
            acc += array[i];
        }
    }
    TOCK1
    std::cout << DIFF1 << std::endl;
    return acc;
}

double unrolled_sum(double *array) {
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

result_struct1 map_filter(double *array) {

    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_1 *result = (tuple_1 *) malloc(sizeof(tuple_1) * allocatedSize);
    for (size_t i = 0; i < MAX; i++) {
        tuple_1 t1 = {array[i], array[i] * 3 + 7};
        if (t1.v0 > 0.5) {
            continue;
        }
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = (tuple_1 *) realloc(result, sizeof(tuple_1) * allocatedSize);
        }
        result[resSize] = t1;
        resSize++;

    }
    TOCK1
    std::cout << DIFF1 << " outer loop map filter\n";

    return {resSize, result};
}

result_struct1 map(double *array) {
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_1 *result = (tuple_1 *) malloc(sizeof(tuple_1) * MAX);

    for (size_t i = 0; i < MAX; i++) {
        tuple_1 t1 = {array[i], array[i] * 3 + 7};
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = (tuple_1 *) realloc(result, sizeof(tuple_1) * allocatedSize);
        }
        result[resSize] = t1;
        resSize++;

    }
    TOCK1
    std::cout << DIFF1 << " outer loop \n";

    return {resSize, result};
}


struct result_struct2 {
    size_t size;
    double *data;
};

result_struct2 filter(double *array) {
    TICK1
    size_t allocatedSize = 2;
    size_t resSize = 0;
    double *result = (double *) malloc(sizeof(double) * MAX);

    for (size_t i = 0; i < MAX; i++) {
        if (array[i] * 100 > 50) {
            if (allocatedSize <= resSize) {
                allocatedSize *= 2;
                result = (double *) realloc(result, sizeof(double) * allocatedSize);
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
    long v0;
    long v1;
};


struct tuple_3 {
    long v0;
    long v1;
};


struct tuple_4 {
    long v0;
    long v1;
    long v2;
};

struct result_struct {
    size_t size;
    tuple_4 *data;
};


size_t len2 = MAX >> 1;

result_struct join(tuple_2 *array1, tuple_3 *array2) {

    size_t acc = 0;

    //build ht
    TICK1
    std::unordered_map<long, vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        }
        else {
            vector<tuple_3> values;
            values.push_back(array2[i]);
            ht.emplace(key, values);
        }
    }
    TOCK1
    std::cout << DIFF1 << " build \n";


    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_4 *result = (tuple_4 *) malloc(sizeof(tuple_4) * allocatedSize);

    for (size_t i = 0; i < MAX; i++) {
        auto key = array1[i].v0;
        if (ht.count(key)) {
            for (auto t : ht[key]) {
                //build a tuple for every combination
                tuple_4 r = {key, array1[i].v1, t.v1};
                if (allocatedSize <= resSize) {
                    allocatedSize *= 2;
                    result = (tuple_4 *) realloc(result, sizeof(tuple_4) * allocatedSize);
                }
                result[resSize] = r;
                resSize++;
            }
        }
        else {
            continue;
        }
    }
    result_struct ret;
    ret.data = result;
    ret.size = resSize;
    return ret;
}

result_struct map_filter_join(long *array1, tuple_3 *array2) {

    //build ht
    TICK1
    std::unordered_map<long, vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        }
        else {
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
    tuple_4 *result = (tuple_4 *) malloc(sizeof(tuple_4) * allocatedSize);

    for (size_t i = 0; i < MAX; i++) {
        tuple_2 t1 = {array1[i], array1[i] * 3 + 7};
        if (t1.v0 <= 0.5) {
            continue;
        }
        auto key = t1.v0;
        if (ht.count(key)) {
            for (auto t : ht[key]) {
                //build a tuple for every combination
                tuple_4 r = {key, t1.v1, t.v1};
                if (allocatedSize <= resSize) {
                    allocatedSize *= 2;
                    result = (tuple_4 *) realloc(result, sizeof(tuple_4) * allocatedSize);
                }
                result[resSize] = r;
                resSize++;
            }
        }
        else {
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



int main() {

    srand (time(NULL));
    double *array = new double[MAX];
    for (size_t i = 0; i < MAX; i++) {
        array[i] = (double) rand() / RAND_MAX;
    }

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
//    auto res = simple_sum(array);
//    cout << "result is " << res << endl;
//
//    res = unrolled_sum(array);
//    cout << "result is " << res << endl;
//
//    free(array);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
//
//    std::cout << res << std::endl;
    auto duration = duration_cast<milliseconds>(t2 - t1).count();
//    std::cout << duration << " simple sum\n";
//
//    t1 = high_resolution_clock::now();
//
//    t2 = high_resolution_clock::now();
//
//    std::cout << res << std::endl;
//    duration = duration_cast<milliseconds>(t2 - t1).count();
//    std::cout << duration << " unrolled sum\n";

//    t1 = high_resolution_clock::now();
//    auto res = map(array);
//    t2 = high_resolution_clock::now();
//    free(array);
//    std::cout << (res.data)[rand() % res.size].v0 << " " << (res.data)[rand() % res.size].v1 << " "
//              << std::endl;


//    t1 = high_resolution_clock::now();
//    auto res = filter(array);
//    t2 = high_resolution_clock::now();
//    free(array);
//    std::cout << (res.data)[rand() % res.size] << " " << (res.data)[rand() % res.size] << " "
//              << std::endl;

//    t1 = high_resolution_clock::now();
//    auto res = map_filter(array);
//    t2 = high_resolution_clock::now();
//    free(array);
//    std::cout << (res.data)[rand() % res.size].v0 << " " << (res.data)[rand() % res.size].v1 << " "
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
//    std::cout << (res.data)[rand() % res.size].v0 << " " << (res.data)[rand() % res.size].v1 << " "
//            << (res.data)[rand() % res.size].v2 << std::endl;
//    std::cout << DIFF1 << " whole join\n";
//    free(array1);
//    free(array2);

    long *array1 = new long[MAX];
    for (size_t i = 0; i < MAX; i++) {
        array1[i] = rand() % 1000;
    }

    tuple_3 *array2 = new tuple_3[len2];
    for (size_t i = 0; i < len2; i++) {
        array2[i] = {rand() % 1000, rand() % 1000};
    }
////
    TICK1
    auto res = map_filter_join(array1, array2);
    TOCK1
//
    std::cout << (res.data)[rand() % res.size].v0 << " " << (res.data)[rand() % res.size].v1 << " "
              << (res.data)[rand() % res.size].v2 << std::endl;
    std::cout << DIFF1 << " whole join\n";
    free(array1);
    free(array2);
    return 0;
}
