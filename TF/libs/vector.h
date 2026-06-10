#ifndef _VECTOR_H
#define _VECTOR_H

typedef struct {
    float x;
    float y;
    float z;
} vector;

vector addVectors(const vector u, const vector v);
vector mulVector(const vector u, float alpha);

#endif