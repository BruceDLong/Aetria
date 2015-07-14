// Math Routines

#include <cmath>

typedef float scaler;

bool cmpf(float A, float B, float epsilon = 0.0000005f){
    return (fabs(A - B) < epsilon);
}
namespace bmath{
// V e c t o r   R o u t i n e s

typedef scaler vector3D[3];

const int _X=0;
const int _Y=1;
const int _Z=2;

inline void inverse(scaler v[3]){
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

inline bool cmp(const scaler v0[3], const scaler v1[3]) {    // compare with epsilon
    return (cmpf(v0[0], v1[0]) && cmpf(v0[1], v1[1]) && cmpf(v0[2], v1[2]));
}

inline void setVec(scaler out[3], scaler x, scaler y, scaler z){
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

inline void copyVec(const scaler v[3], scaler out[3]){
    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
}

inline void translate(const scaler v0[3], const scaler v1[3], scaler out[3]){  // AKA "Add"
    out[0] = v0[0] + v1[0];
    out[1] = v0[1] + v1[1];
    out[2] = v0[2] + v1[2];
}

inline void scale(const scaler v0[3], const scaler v1[3], scaler out[3]){
    out[0] = v0[0] * v1[0];
    out[1] = v0[1] * v1[1];
    out[2] = v0[2] * v1[2];
}

inline scaler dist(const scaler p1[3], const scaler p2[3]){
    scaler x = p1[0]-p2[0];
    scaler y = p1[1]-p2[1];
    scaler z = p1[2]-p2[2];
    return sqrt(x*x + y*y + z*z);
}


inline scaler dot(const scaler vec1[3], const scaler vec2[3]){
    return vec1[_X] * vec2[_X]  +  vec1[_Y] * vec2[_Y]  +  vec1[_Z] * vec2[_Z];
}

inline scaler length(const scaler v[3]){    // "norm()"
    return sqrt(v[_X]*v[_X]  +  v[_Y]*v[_Y]  +  v[_Z]*v[_Z]);
}

inline scaler spance(const scaler v[3]){    // un-normed length
    return (v[_X]*v[_X]  +  v[_Y]*v[_Y]  +  v[_Z]*v[_Z]);
}

inline void  cross(const scaler vec1[3], const scaler vec2[3], scaler out[3]){
    out[_X]=vec1[_Y] * vec2[_Z] - vec1[_Z] * vec2[_Y];
    out[_Y]=vec1[_Z] * vec2[_X] - vec1[_X] * vec2[_Z];
    out[_Z]=vec1[_X] * vec2[_Y] - vec1[_Y] * vec2[_X];
}

inline void normalize(scaler out[3]){
    scaler len=length(out);
    out[_X] /= len;
    out[_Y] /= len;
    out[_Z] /= len;
}

inline void setLength(scaler out[3], scaler LenToSet){
    scaler len = LenToSet / length(out);
    out[_X] *= len;
    out[_Y] *= len;
    out[_Z] *= len;
}

/*
/   For conversions and div-by-zero-proof versions see: http://leetnightshade.com/c-vector3-class
// This uses a Quaternion combined with the Matrix Utility, neither of which are detailed out in this post.
        inline Vector3  Rotate(const Vector3& vec1, Scalar angle, const Vector3&amp; axis)
        {
                return TransformCoord(Quaternion::FromAxis(axis.X,axis.Y,axis.Z,angle).Get_RotationMatrix(),vec1);
        }

        inline Vector3  ToPolar(Scalar x, Scalar y, Scalar z)
        {
                return Vector3(
                        atan2(y,x),
                        sqrt(x * x + y * y),
                        z);
        }

        inline Vector3  ToCartesian(Scalar radius, Scalar angle, Scalar z)
        {
                return Vector3(
                        radius * cos(angle),
                        radius * sin(angle),
                        z);
        }
*/

}
