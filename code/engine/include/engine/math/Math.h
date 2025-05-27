//
// Created by Cem on 14.07.2022.
//
#pragma once
#include "glm/vec3.hpp"
#include "glm/fwd.hpp"
#include "glm/detail/type_mat4x4.hpp"
#include "glm/gtx/matrix_decompose.hpp"


namespace engine::math
{
    static inline void decomposeTransform(const glm::mat4 transform, glm::vec3& outTranslation, glm::quat& outRotation,
                                          glm::vec3& outScale)
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, outScale, outRotation, outTranslation, skew, perspective);
    }

    template<typename T>
    static inline T lerp(T a, T b, float t)
    {
        return a * (1.f - t) + b * t;
    }
}