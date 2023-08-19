#pragma once

#include "tesselator.h"
#include "tess.h"
#include "raymath.h"

#include <vector>

namespace App {

    class Tesselator {
    public:
        static Tesselator& getInstance();
        void tesselate(const std::vector<Vector2>& points, std::vector<Vector2>& outPoints);
    private:
        TESStesselator * m_tess;

        Tesselator();
        ~Tesselator();
    };

}