#pragma once

#include <Core/Common.hpp>
#include <Core/Math/Float3.hpp>
#include <Core/Math/Quaternion.hpp>
#include <Jolt/Jolt.h>

namespace Hyper
{
    namespace JoltMath
    {
        inline Float3 ToFloat3(const JPH::Vec3& vector)
        {
            return Float3{
                vector.GetX(),
                vector.GetY(),
                vector.GetZ()
            };
        }

        inline JPH::Vec3 FromFloat3(const Float3& vector)
        {
            return JPH::Vec3{
                vector.X,
                vector.Y,
                vector.Z
            };
        }

        inline Quaternion ToQuaternion(const JPH::Quat& quat)
        {
            return Quaternion{
                quat.GetX(),
                quat.GetY(),
                quat.GetZ(),
                quat.GetW()
            };
        }

        inline JPH::Quat FromQuaternion(const Quaternion& quat)
        {
            return JPH::Quat{
                quat.X,
                quat.Y,
                quat.Z,
                quat.W
            };
        }
    }
}
