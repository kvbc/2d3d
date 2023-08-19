#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

namespace App {

    class Shape {
    private:
        bool m_isComplete = false;
        std::vector<Vector2> m_tesselatedPoints;

    public:
        std::vector<Vector2> points;
        Color color = BLUE;

        Shape();
        bool isComplete();
        void markAsComplete();
        bool canAddPoint(Vector2 point);
        const std::vector<Vector2>& getTesselatedPoints();
    };

}