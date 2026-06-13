#ifndef _VECTOR_H
#define _VECTOR_H

typedef union {
    struct {float x, y, z;};
    vec3 raw;
} vector;

#define VETOR_NULO (vector){0.0f, 0.0f, 0.0f}

vector addVectors(const vector u, const vector v);
vector mulVector(const vector u, float alpha);

#endif