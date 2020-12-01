#include <cmath>
#include "Vector3f.h"

Vector3f::Vector3f(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

float Vector3f::magnitude() const {
    float length = ((float) pow(this->x, 2.0f) + (float) pow(this->y, 2.0f));
    return (float) sqrt(pow(this->z, 2.0f) + length);
}

float Vector3f::dot(Vector3f other) const {
    float xm = (this->x * other.x) + (this->x * other.y) + (this->x * other.z);
    float ym = (this->y * other.x) + (this->y * other.y) + (this->y * other.z);
    float zm = (this->z * other.x) + (this->z * other.y) + (this->z * other.z);
    return xm + ym + zm;
}

Vector3f Vector3f::normalized() const {
    Vector3f out;

    float length = magnitude();

    out.x = this->x / length;
    out.y = this->y / length;
    out.z = this->z / length;
    return out;
}

Vector3f Vector3f::cross(Vector3f other) const {
    return Vector3f(this->z * other.y - this->y * other.z, this->x * other.z - this->z * other.x, this->x * other.y - this->y * other.x);
}

Vector3f Vector3f::multiply(float scalar) const {
    Vector3f out;
    out.x = this->x * scalar;
    out.y = this->y * scalar;
    out.z = this->z * scalar;
    return out;
}

Vector3f Vector3f::divide(float devider) const {
    Vector3f out;
    out.x = this->x / devider;
    out.y = this->y / devider;
    out.z = this->z / devider;
    return out;
}

Vector3f Vector3f::reverse() const {
    Vector3f out;
    out.x = -this->x;
    out.y = -this->y;
    out.z = -this->z;
    return out;
}

Vector3f Vector3f::substract(Vector3f other) const {
    return add(other.reverse());
}

Vector3f Vector3f::add(Vector3f other) const {
    Vector3f out;
    out.x = this->x + other.x;
    out.y = this->y + other.y;
    out.z = this->z + other.z;
    return out;
}

Vector3f Vector3f::operator-() const {
    return this->reverse();
}

Vector3f operator-(const Vector3f& vector) {
    return vector.reverse();
}

Vector3f operator+(const Vector3f& a, const Vector3f& b) {
    return a.add(b);
}

Vector3f operator-(const Vector3f& a, const Vector3f& b) {
    return a.substract(b);
}

float operator* (const Vector3f& a, const Vector3f& other) {
    return a.dot(other);
}

Vector3f operator* (const Vector3f& vector, float scalar) {
    return vector.multiply(scalar);
}

Vector3f operator/ (const Vector3f& vector, float divider) {
    return vector.divide(divider);
}
