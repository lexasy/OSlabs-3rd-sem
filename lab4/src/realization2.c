#include "foos.h"

// Using method of trapezoids
float SinIntegral(float A, float B, float e) {
    float result = (sin(A) + sin(B)) / 2;
    for (float i = A + e; i <= B - e; i += e) {
        result += sin(i);
    }
    return e * result;
}

// Using Vallis formula
float Pi(int K) {
    float result = 1;
    for (int i = 1; i <= K; i++) {
        result *= (4.0 * i * i) / (4 * i * i - 1);
    }
    return result * 2;
}