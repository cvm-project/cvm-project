//
// Created by sabir on 19.07.17.
//
#define DEBUG 1
#ifdef DEBUG
    #define DEBUG_PRINT(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)
#else
    #define DEBUG_PRINT(x) do {} while (0)
#endif