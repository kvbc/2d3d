#pragma once

#include "tesselator.h"
#include "tess.h"
#include "raylib.h"
#include "raymath.h"

#include <vector>

namespace App {

    class Tesselator {
    public:
        static Tesselator& Get();
        std::vector<Vector2> Tesselate2D(const std::vector<Vector2>& points);
        std::vector<Vector3> Tesselate3D(const std::vector<Vector3>& vertices);
    private:
        TESStesselator * m_tess;

        Tesselator();
        ~Tesselator();
    };

}