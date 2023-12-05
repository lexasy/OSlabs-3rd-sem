#include "foos.h"
#include <math.h>

// Using method of left rectangles
float SinIntegral(float A, float B, float e) {
    float result = 0;
    for (float i = A; i < B; i += e) {
        result += sin(i);
    }
    return result * e;
}

// Using Leibniz row
float Pi(int K) {
    float result = 0;
    for (int i = 0; i <= K; i++) {
        result += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
    return result * 4;
}