///////////////////////////////////////////////////////////////////////////////
// Vectors.h
// =========
// 2D/3D/4D vectors
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2007-02-14
// UPDATED: 2013-01-20
//
// Copyright (C) 2007-2013 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////


#ifndef VECTORS_H_DEF
#define VECTORS_H_DEF

#include <cmath>
#include <iostream>

typedef float scalar;

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2
{
    scalar x;
    scalar y;

    // ctors
    Vector2() : x(0), y(0) {};
    Vector2(scalar x, scalar y) : x(x), y(y) {};

    // utils functions
    void        set(scalar x, scalar y);
    scalar       length() const;                         //
    scalar       distance(const Vector2& vec) const;     // distance between two vectors
    Vector2&    normalize();                            //
    scalar       dot(const Vector2& vec) const;          // dot product
    bool        equal(const Vector2& vec, scalar e) const; // compare with epsilon

    // operators
    Vector2     operator-() const;                      // unary operator (negate)
    Vector2     operator+(const Vector2& rhs) const;    // add rhs
    Vector2     operator-(const Vector2& rhs) const;    // subtract rhs
    Vector2&    operator+=(const Vector2& rhs);         // add rhs and update this object
    Vector2&    operator-=(const Vector2& rhs);         // subtract rhs and update this object
    Vector2     operator*(const scalar scale) const;     // scale
    Vector2     operator*(const Vector2& rhs) const;    // multiply each element
    Vector2&    operator*=(const scalar scale);          // scale and update this object
    Vector2&    operator*=(const Vector2& rhs);         // multiply each element and update this object
    Vector2     operator/(const scalar scale) const;     // inverse scale
    Vector2&    operator/=(const scalar scale);          // scale and update this object
    bool        operator==(const Vector2& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector2& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector2& rhs) const;    // comparison for sort
    scalar       operator[](int index) const;            // subscript operator v[0], v[1]
    scalar&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector2 operator*(const scalar a, const Vector2 vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3
{
    scalar x;
    scalar y;
    scalar z;

    // ctors
    Vector3() : x(0), y(0), z(0) {};
    Vector3(scalar x, scalar y, scalar z) : x(x), y(y), z(z) {};

    // utils functions
    void        set(scalar x, scalar y, scalar z);
    scalar       length() const;                        //
    scalar       squaredLength() const;
    scalar       distance(const Vector3& vec) const;     // distance between two vectors
    Vector3&    normalize();                            //
    scalar       dot(const Vector3& vec) const;          // dot product
    Vector3     cross(const Vector3& vec) const;        // cross product
    bool        equal(const Vector3& vec, scalar e) const; // compare with epsilon

    // operators
    Vector3     operator-() const;                      // unary operator (negate)
    Vector3     operator+(const Vector3& rhs) const;    // add rhs
    Vector3     operator-(const Vector3& rhs) const;    // subtract rhs
    Vector3&    operator+=(const Vector3& rhs);         // add rhs and update this object
    Vector3&    operator-=(const Vector3& rhs);         // subtract rhs and update this object
    Vector3     operator*(const scalar scale) const;     // scale
    Vector3     operator*(const Vector3& rhs) const;    // multiplay each element
    Vector3&    operator*=(const scalar scale);          // scale and update this object
    Vector3&    operator*=(const Vector3& rhs);         // product each element and update this object
    Vector3     operator/(const scalar scale) const;     // inverse scale
    Vector3&    operator/=(const scalar scale);          // scale and update this object
    bool        operator==(const Vector3& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector3& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector3& rhs) const;    // comparison for sort
    scalar       operator[](int index) const;            // subscript operator v[0], v[1]
    scalar&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector3 operator*(const scalar a, const Vector3 vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4
{
    scalar x;
    scalar y;
    scalar z;
    scalar w;

    // ctors
    Vector4() : x(0), y(0), z(0), w(0) {};
    Vector4(scalar x, scalar y, scalar z, scalar w) : x(x), y(y), z(z), w(w) {};

    // utils functions
    void        set(scalar x, scalar y, scalar z, scalar w);
    scalar       length() const;                         //
    scalar       distance(const Vector4& vec) const;     // distance between two vectors
    Vector4&    normalize();                            //
    scalar       dot(const Vector4& vec) const;          // dot product
    bool        equal(const Vector4& vec, scalar e) const; // compare with epsilon

    // operators
    Vector4     operator-() const;                      // unary operator (negate)
    Vector4     operator+(const Vector4& rhs) const;    // add rhs
    Vector4     operator-(const Vector4& rhs) const;    // subtract rhs
    Vector4&    operator+=(const Vector4& rhs);         // add rhs and update this object
    Vector4&    operator-=(const Vector4& rhs);         // subtract rhs and update this object
    Vector4     operator*(const scalar scale) const;     // scale
    Vector4     operator*(const Vector4& rhs) const;    // multiply each element
    Vector4&    operator*=(const scalar scale);          // scale and update this object
    Vector4&    operator*=(const Vector4& rhs);         // multiply each element and update this object
    Vector4     operator/(const scalar scale) const;     // inverse scale
    Vector4&    operator/=(const scalar scale);          // scale and update this object
    bool        operator==(const Vector4& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector4& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector4& rhs) const;    // comparison for sort
    scalar       operator[](int index) const;            // subscript operator v[0], v[1]
    scalar&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector4 operator*(const scalar a, const Vector4 vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);
};



// fast math routines from Doom3 SDK
inline scalar invSqrt(scalar x)
{
    scalar xhalf = 0.5f * x;
    int i = *(int*)&x;          // get bits for floating value
    i = 0x5f3759df - (i>>1);    // gives initial guess
    x = *(scalar*)&i;            // convert bits back to scalar
    x = x * (1.5f - xhalf*x*x); // Newton step
    return x;
}



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2
///////////////////////////////////////////////////////////////////////////////
inline Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

inline Vector2 Vector2::operator+(const Vector2& rhs) const {
    return Vector2(x+rhs.x, y+rhs.y);
}

inline Vector2 Vector2::operator-(const Vector2& rhs) const {
    return Vector2(x-rhs.x, y-rhs.y);
}

inline Vector2& Vector2::operator+=(const Vector2& rhs) {
    x += rhs.x; y += rhs.y; return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& rhs) {
    x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2 Vector2::operator*(const scalar a) const {
    return Vector2(x*a, y*a);
}

inline Vector2 Vector2::operator*(const Vector2& rhs) const {
    return Vector2(x*rhs.x, y*rhs.y);
}

inline Vector2& Vector2::operator*=(const scalar a) {
    x *= a; y *= a; return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& rhs) {
    x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2 Vector2::operator/(const scalar a) const {
    return Vector2(x/a, y/a);
}

inline Vector2& Vector2::operator/=(const scalar a) {
    x /= a; y /= a; return *this;
}

inline bool Vector2::operator==(const Vector2& rhs) const {
    return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2::operator!=(const Vector2& rhs) const {
    return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2::operator<(const Vector2& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    return false;
}

inline scalar Vector2::operator[](int index) const {
    return (&x)[index];
}

inline scalar& Vector2::operator[](int index) {
    return (&x)[index];
}

inline void Vector2::set(scalar x, scalar y) {
    this->x = x; this->y = y;
}

inline scalar Vector2::length() const {
    return sqrtf(x*x + y*y);
}

inline scalar Vector2::distance(const Vector2& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y));
}

inline Vector2& Vector2::normalize() {
    //@@const scalar EPSILON = 0.000001f;
    scalar xxyy = x*x + y*y;
    //@@if(xxyy < EPSILON)
    //@@    return *this;

    //scalar invLength = invSqrt(xxyy);
    scalar invLength = 1.0f / sqrtf(xxyy);
    x *= invLength;
    y *= invLength;
    return *this;
}

inline scalar Vector2::dot(const Vector2& rhs) const {
    return (x*rhs.x + y*rhs.y);
}

inline bool Vector2::equal(const Vector2& rhs, scalar epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline Vector2 operator*(const scalar a, const Vector2 vec) {
    return Vector2(a*vec.x, a*vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}
// END OF VECTOR2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3
///////////////////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(const Vector3& rhs) const {
    return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
}

inline Vector3 Vector3::operator-(const Vector3& rhs) const {
    return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
}

inline Vector3& Vector3::operator+=(const Vector3& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3 Vector3::operator*(const scalar a) const {
    return Vector3(x*a, y*a, z*a);
}

inline Vector3 Vector3::operator*(const Vector3& rhs) const {
    return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3& Vector3::operator*=(const scalar a) {
    x *= a; y *= a; z *= a; return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3 Vector3::operator/(const scalar a) const {
    return Vector3(x/a, y/a, z/a);
}

inline Vector3& Vector3::operator/=(const scalar a) {
    x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3::operator==(const Vector3& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3::operator!=(const Vector3& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3::operator<(const Vector3& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    return false;
}

inline scalar Vector3::operator[](int index) const {
    return (&x)[index];
}

inline scalar& Vector3::operator[](int index) {
    return (&x)[index];
}

inline void Vector3::set(scalar x, scalar y, scalar z) {
    this->x = x; this->y = y; this->z = z;
}

inline scalar Vector3::length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline scalar Vector3::squaredLength() const {
    return (x*x + y*y + z*z);
}

inline scalar Vector3::distance(const Vector3& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z));
}

inline Vector3& Vector3::normalize() {
    //@@const scalar EPSILON = 0.000001f;
    scalar xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is ~zero vector

    //scalar invLength = invSqrt(xxyyzz);
    scalar invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline scalar Vector3::dot(const Vector3& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

inline Vector3 Vector3::cross(const Vector3& rhs) const {
    return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

inline bool Vector3::equal(const Vector3& rhs, scalar epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline Vector3 operator*(const scalar a, const Vector3 vec) {
    return Vector3(a*vec.x, a*vec.y, a*vec.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
// END OF VECTOR3 /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4
///////////////////////////////////////////////////////////////////////////////
inline Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator+(const Vector4& rhs) const {
    return Vector4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

inline Vector4 Vector4::operator-(const Vector4& rhs) const {
    return Vector4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

inline Vector4& Vector4::operator+=(const Vector4& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4 Vector4::operator*(const scalar a) const {
    return Vector4(x*a, y*a, z*a, w*a);
}

inline Vector4 Vector4::operator*(const Vector4& rhs) const {
    return Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4& Vector4::operator*=(const scalar a) {
    x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4 Vector4::operator/(const scalar a) const {
    return Vector4(x/a, y/a, z/a, w/a);
}

inline Vector4& Vector4::operator/=(const scalar a) {
    x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4::operator==(const Vector4& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4::operator!=(const Vector4& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4::operator<(const Vector4& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    if(w < rhs.w) return true;
    if(w > rhs.w) return false;
    return false;
}

inline scalar Vector4::operator[](int index) const {
    return (&x)[index];
}

inline scalar& Vector4::operator[](int index) {
    return (&x)[index];
}

inline void Vector4::set(scalar x, scalar y, scalar z, scalar w) {
    this->x = x; this->y = y; this->z = z; this->w = w;
}

inline scalar Vector4::length() const {
    return sqrtf(x*x + y*y + z*z + w*w);
}

inline scalar Vector4::distance(const Vector4& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
}

inline Vector4& Vector4::normalize() {
    //NOTE: leave w-component untouched
    //@@const scalar EPSILON = 0.000001f;
    scalar xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is zero vector

    //scalar invLength = invSqrt(xxyyzz);
    scalar invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline scalar Vector4::dot(const Vector4& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
}

inline bool Vector4::equal(const Vector4& rhs, scalar epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
           fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline Vector4 operator*(const scalar a, const Vector4 vec) {
    return Vector4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

inline std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}
// END OF VECTOR4 /////////////////////////////////////////////////////////////

#endif
