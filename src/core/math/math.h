#pragma once

#include <cstdint>
#include <cmath>

struct Vector2 { float x = 0, y = 0; };
struct Vector3 { float x = 0, y = 0, z = 0; };
struct Vector4 { float x = 0, y = 0, z = 0, w = 0; };

struct Matrix4 {
    float data[16]{};

    static Matrix4 identity();
    Matrix4 operator*(const Matrix4& rhs) const;
    Vector4 operator*(const Vector4& v) const;

    static Matrix4 perspective(float fov, float aspect, float near, float far);
    static Matrix4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
    static Matrix4 translation(const Vector3& t);
    static Matrix4 rotation(const Vector3& euler);
    static Matrix4 scale(const Vector3& s);
};
