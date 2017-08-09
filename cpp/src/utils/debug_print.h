//
// Created by sabir on 19.07.17.
//
#include "config.h"

#ifdef DEBUG
//#define DEBUG_PRINT(x) do { std::clog<<"DEBUG " << #x << ": " << x << std::endl; } while (0)
#define DEBUG_PRINT(x) do {} while (0)
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif