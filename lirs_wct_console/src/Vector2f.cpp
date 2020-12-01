#include <cmath>
#include "Vector2f.h"

Vector2f::Vector2f(float x, float y)
{
    this->x = x;
    this->y = y;
}

float Vector2f::magnitude() const {
    return (float) sqrt((pow(this->x, 2.0f) + pow(this->y, 2.0f)));
}

float Vector2f::dot(Vector2f vector) const {
    return ((this->x * vector.x) + (this->x * vector.y) + (this->y * vector.x) + (this->y * vector.y));
}

Vector2f Vector2f::normalized() const {
    Vector2f out;

    float length = magnitude();

    out.x = this->x / length;
    out.y = this->y / length;
    return out;
}

Vector2f Vector2f::multiply(float scalar) const {
    Vector2f out;
    out.x = this->x * scalar;
    out.y = this->y * scalar;
    return out;
}

Vector2f Vector2f::reverse() const {
    Vector2f out;
    out.x = -this->x;
    out.y = -this->y;
    return out;
}

Vector2f Vector2f::substract(Vector2f other) const {
    return add(other.reverse());
}

Vector2f Vector2f::add(Vector2f other) const {
    Vector2f out;
    out = this->x + other.x;
    out = this->y + other.y;
    return out;
}

Vector2f Vector2f::operator-() const {
    return this->reverse();
}

Vector2f operator-(const Vector2f& vector) {
    return vector.reverse();
}

Vector2f operator+(const Vector2f& a, const Vector2f& b) {
    return a.add(b);
}

Vector2f operator-(const Vector2f& a, const Vector2f& b) {
    return a.substract(b);
}

float operator* (const Vector2f& a, const Vector2f& other) {
    return a.dot(other);
}

Vector2f operator* (const Vector2f& vector, float scalar) {
    return vector.multiply(scalar);
}
