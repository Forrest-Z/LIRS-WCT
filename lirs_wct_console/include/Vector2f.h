
#ifndef VECTOR2F_H
#define VECTOR2F_H

struct Vector2f
{
    struct
    {
        float x, y;
    };

    Vector2f(float x = 0.0f, float y = 0.0f);

    float magnitude() const;

    float dot(Vector2f other) const;

    Vector2f normalized() const;

    Vector2f cross(Vector2f other) const;

    Vector2f multiply(float scalar) const;

    Vector2f reverse() const;

    Vector2f substract(Vector2f other) const;

    Vector2f add(Vector2f other) const;

    Vector2f operator-() const;

    friend Vector2f operator-(const Vector2f& vector);

    friend Vector2f operator+(const Vector2f& a, const Vector2f& b);

    friend Vector2f operator-(const Vector2f& a, const Vector2f& b);

    friend float operator* (const Vector2f& a, const Vector2f& other);

    friend Vector2f operator* (const Vector2f& vector, float scalar);
};

#endif //VECTOR2F_H
