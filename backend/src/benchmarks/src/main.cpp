#include <cmath>

#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

static constexpr size_t kNumElements = 1UL << 29U;

template <typename T>
auto realloc_or_throw(T *old_ptr, const size_t n_elements) -> T * {
    void *ptr = realloc(old_ptr, sizeof(T) * n_elements);
    if (ptr == nullptr) throw std::bad_alloc();
    return reinterpret_cast<T *>(ptr);
}

auto my_add(double a, double b) -> double { return a + b; }

auto simple_sum(const double *const array) -> double {
    auto const start = std::chrono::steady_clock::now();
    double acc = 0;
    for (size_t j = 0; j < 1; j++) {
        for (size_t i = 0; i < kNumElements; i++) {
            acc = my_add(acc, array[i]);
        }
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << "outer loop sum "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << std::endl;
    return acc;
}

auto unrolled_sum(const double *const array) -> double {
    auto const start = std::chrono::steady_clock::now();
    std::array<double, 4> acc = {0.0, 0.0, 0.0, 0.0};
    for (size_t j = 0; j < 1; j++) {
        for (size_t i = 0; i < kNumElements; i += 4) {
            acc[0] += array[i];
            acc[1] += array[i + 1];
            acc[2] += array[i + 2];
            acc[3] += array[i + 3];
        }
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << std::endl;
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

auto gap_filter(const double *const array) -> result_struct1 {
    auto const start = std::chrono::steady_clock::now();
    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result = reinterpret_cast<tuple_1 *>(
            malloc(sizeof(tuple_1) * allocatedSize));
    for (size_t i = 0; i < kNumElements; i++) {
        // NOLINTNEXTLINE(readability-magic-numbers)
        tuple_1 t1 = {array[i], array[i] * 3 + 7};
        // NOLINTNEXTLINE(readability-magic-numbers)
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
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " outer loop map filter\n";

    return {resSize, result};
}

auto filter_sum(const double *const array) -> double {
    double res = 0;
    auto const start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNumElements; i++) {
        if (array[i] > 0) {
            res += array[i];
            //            tot--;
        }
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " filter sum\n";
    return res;
}

auto filter_sum2(const double *const array) -> double {
    double res = 0;
    auto const start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < kNumElements; i++) {
        res += array[i] * static_cast<double>(array[i] > 0);
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " filter sum2\n";
    return res;
}

auto filter_sum3(const double *const array) -> double {
    double res = 0;
    auto const start = std::chrono::steady_clock::now();
    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result =  // cppcheck-suppress constArgument
            reinterpret_cast<double *>(malloc(sizeof(double) * allocatedSize));

    for (size_t i = 0; i < kNumElements; i++) {
        result[resSize] = array[i];
        resSize += static_cast<size_t>(array[i] > 0);
    }

    auto const end = std::chrono::steady_clock::now();
    free(result);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " filter sum3\n";
    return res;
}

// NOLINTNEXTLINE(readability-magic-numbers)
auto map_1(double a) -> tuple_1 { return {a, a * 3 + 7}; }

auto map(const double *array) -> result_struct1 {
    auto const start = std::chrono::steady_clock::now();
    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result = reinterpret_cast<tuple_1 *>(
            malloc(sizeof(tuple_1) * allocatedSize));

    for (size_t i = 0; i < kNumElements; i++) {
        tuple_1 t1 = map_1(array[i]);
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = realloc_or_throw<tuple_1>(result, allocatedSize);
        }
        result[resSize] = t1;
        resSize++;
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " outer loop map \n";

    return {resSize, result};
}

struct result_struct2 {
    size_t size;
    double *data;
};

auto filter(const double *const array) -> result_struct2 {
    auto const start = std::chrono::steady_clock::now();
    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result =
            reinterpret_cast<double *>(malloc(sizeof(double) * kNumElements));

    for (size_t i = 0; i < kNumElements; i++) {
        // NOLINTNEXTLINE(readability-magic-numbers)
        if (array[i] * 100 > 50) {
            if (allocatedSize <= resSize) {
                allocatedSize *= 2;
                result = realloc_or_throw<double>(result, allocatedSize);
            }
            result[resSize] = array[i];
            resSize++;
        }
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " outer loop filter \n";

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

const size_t len2 = kNumElements >> 1U;

auto join(tuple_2 *array1, tuple_3 *array2) -> result_struct {
    // build ht
    auto const start = std::chrono::steady_clock::now();
    std::unordered_map<int64_t, std::vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        } else {
            std::vector<tuple_3> values;
            values.push_back(array2[i]);
            ht.emplace(key, values);
        }
    }
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " build \n";

    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result = reinterpret_cast<tuple_4 *>(
            malloc(sizeof(tuple_4) * allocatedSize));

    for (size_t i = 0; i < kNumElements; i++) {
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
    result_struct ret{};
    ret.data = result;
    ret.size = resSize;
    return ret;
}

auto map_filter_join(const int64_t *const array1, const tuple_3 *const array2)
        -> result_struct {
    // build ht
    auto const start1 = std::chrono::steady_clock::now();
    std::unordered_map<int64_t, std::vector<tuple_3>> ht;
    for (size_t i = 0; i < len2; i++) {
        auto key = array2[i].v0;
        if (ht.count(key) > 0) {
            ht[key].push_back(array2[i]);
        } else {
            std::vector<tuple_3> values;
            values.push_back(array2[i]);
            ht.emplace(key, values);
        }
    }
    auto const end1 = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end1 -
                                                                       start1)
                         .count()
              << " build \n";

    auto const start2 = std::chrono::steady_clock::now();
    size_t allocatedSize = 2;
    size_t resSize = 0;
    auto *result = reinterpret_cast<tuple_4 *>(
            malloc(sizeof(tuple_4) * allocatedSize));

    for (size_t i = 0; i < kNumElements; i++) {
        // NOLINTNEXTLINE(readability-magic-numbers)
        tuple_2 t1 = {array1[i], array1[i] * 3 + 7};
        // NOLINTNEXTLINE(readability-magic-numbers)
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
    result_struct ret{};
    ret.data = result;
    ret.size = resSize;
    auto const end2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end2 -
                                                                       start2)
                         .count()
              << " prob \n";
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

auto map_reduce_by_key(const int64_t *const array) -> result_struct_rbk {
    // build ht
    auto const start = std::chrono::steady_clock::now();
    std::unordered_map<int64_t, int64_t> ht;
    for (size_t i = 0; i < kNumElements; i++) {
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
    auto *result = reinterpret_cast<tuple_5 *>(
            malloc(sizeof(tuple_5) * allocatedSize));

    for (auto &it : ht) {
        tuple_5 r = {it.first, it.second};
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = realloc_or_throw<tuple_5>(result, allocatedSize);
        }
        result[resSize] = r;
        resSize++;
    }
    result_struct_rbk ret{};
    ret.data = result;
    ret.size = resSize;
    auto const end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       start)
                         .count()
              << " group by \n";
    return ret;
}

auto main() -> int {
    // NOLINTNEXTLINE(readability-magic-numbers)
    std::mt19937 gen(1337);
    // NOLINTNEXTLINE(readability-magic-numbers)
    std::uniform_real_distribution<double> dis(0.1, 1.0);

    auto *array =
            reinterpret_cast<double *>(malloc((kNumElements) * sizeof(double)));

    for (size_t i = 0; i < kNumElements; i++) {
        array[i] = dis(gen);
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
    //    int64_t *array_rbk = new int64_t[kNumElements];
    //    for (size_t i = 0; i < kNumElements; i++) {
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
    //    tuple_2 *array1 = new tuple_2[kNumElements];
    //    for (size_t i = 0; i < kNumElements; i++) {
    //        array1[i] = {rand() % 1000, rand() % 1000};
    //    }
    //
    //    tuple_3 *array2 = new tuple_3[len2];
    //    for (size_t i = 0; i < len2; i++) {
    //        array2[i] = {rand() % 1000, rand() % 1000};
    //    }
    ////
    //    auto const start = std::chrono::steady_clock::now();
    //    auto res = join(array1, array2);
    //    auto const end = std::chrono::steady_clock::now();
    //
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //            << (res.data)[rand() % res.size].v2 << std::endl;
    //    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end
    //    - start).count() << " whole join\n"; free(array1); free(array2);

    //    int64_t *array1 = new int64_t[kNumElements];
    //    for (size_t i = 0; i < kNumElements; i++) {
    //        array1[i] = rand() % 1000;
    //    }
    //
    //    tuple_3 *array2 = new tuple_3[len2];
    //    for (size_t i = 0; i < len2; i++) {
    //        array2[i] = {rand() % 1000, rand() % 1000};
    //    }
    //////
    //    auto const start = std::chrono::steady_clock::now();
    //    auto res = map_filter_join(array1, array2);
    //    auto const end = std::chrono::steady_clock::now();
    ////
    //    std::cout << (res.data)[rand() % res.size].v0 << " " <<
    //    (res.data)[rand() % res.size].v1 << " "
    //              << (res.data)[rand() % res.size].v2 << std::endl;
    //    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end
    //    - start).count() << " whole join\n"; free(array1); free(array2);
    free(array);
    return 0;
}
