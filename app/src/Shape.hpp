#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

namespace App {

    class Shape {
    private:
        bool m_isComplete = false;
        std::vector<Vector3> m_vertices;

    public:
        Color color = BLUE;

        Shape();
        bool isComplete() const;
        void markAsComplete();
        void addVertex(Vector3 vertex);
        size_t getVertexCount();
        const Vector3& getVertex(size_t idx);
        const std::vector<Vector3>& getVertices() const;
        void popVertex();
        bool canAddVertex(Vector3 vertex) const;
    };

}