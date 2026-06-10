#include "libs/vector.h"

vector addVectors(const vector u, const vector v) {
    vector sum;
    sum.x = u.x + v.x;
    sum.y = u.y + v.y;
    sum.z = u.z + v.z;
    return sum;
}

vector mulVector(const vector u, float alpha) {
    vector mul;
    mul.x = u.x * alpha;
    mul.y = u.y * alpha;
    mul.z = u.z * alpha;
    return mul;
}