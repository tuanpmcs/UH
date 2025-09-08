#pragma once

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& origin, const vec3& direction)
        : m_orig(origin), m_dir(direction)
    {}

    vec3 origin() const { return m_orig; }
    vec3 direction() const { return m_dir; }

    vec3 at(float t) const {
        return m_orig + t * m_dir;
    }
private:
    vec3 m_orig;
    vec3 m_dir;
};