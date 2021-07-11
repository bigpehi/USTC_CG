#pragma once
#include "Vector.h"
#include "Matrices.h"
using namespace GLMath;

class Camera {
public:
    //_up(0,1,0)
    Camera(const Vector3& _pos, const Vector3& _target, const Vector3& _up) : mPosition(_pos), mTarget(_target), mUp(_up) {
    }

    // return view matrix
    const Matrix4& getLookAt() {
        Matrix4 retMatrx;
        return retMatrx.lookAt(mPosition, mTarget, mUp);
    }

private:
    Vector3 mPosition;
    Vector3 mTarget;
    Vector3 mUp;
};