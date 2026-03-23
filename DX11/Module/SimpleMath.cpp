#include "pch.h"
#include "SimpleMath.h"

using namespace SimpleMath;

const Vector2 Vector2::Zero  = {0.f, 0.f};
const Vector2 Vector2::One   = {1.f, 1.f};
const Vector2 Vector2::UnitX = {1.f, 0.f};
const Vector2 Vector2::UnitY = {0.f, 1.f};

const Vector3 Vector3::Zero    = {0.f, 0.f, 0.f};           
const Vector3 Vector3::One     = {1.f, 1.f, 1.f};
const Vector3 Vector3::UnitX   = {1.f, 0.f, 0.f};
const Vector3 Vector3::UnitY   = {0.f, 1.f, 0.f};
const Vector3 Vector3::UnitZ   = {0.f, 0.f, 1.f};
const Vector3 Vector3::Up      = {0.f, 1.f, 0.f};
const Vector3 Vector3::Down    = {0.f, -1.f, 0.f};
const Vector3 Vector3::Right   = {1.f, 0.f, 0.f};
const Vector3 Vector3::Left    = {-1.f, 0.f, 0.f};
const Vector3 Vector3::Forward = {0.f, 0.f, 1.f};
const Vector3 Vector3::Front   = {0.f, 0.f, 1.f};
const Vector3 Vector3::Backward= {0.f, 0.f, -1.f};


Vector2 Vector2::Normalized() const
{
    using namespace DirectX;
    Vector2 Temp = *this;
    XMVECTOR v1 = XMLoadFloat2(&Temp);
    XMVECTOR X = XMVector2Normalize(v1);
    XMStoreFloat2(&Temp, X);
    return Temp;
}

Vector3 Vector3::Normalized() const
{
    using namespace DirectX;
    Vector3 Temp = *this;
    XMVECTOR v1 = XMLoadFloat3(&Temp);
    XMVECTOR X = XMVector3Normalize(v1);
    XMStoreFloat3(&Temp, X);
    return Temp;
}
