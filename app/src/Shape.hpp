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
        bool IsComplete() const;
        void MarkAsComplete();
        void AddVertex(Vector3 vertex);
        size_t GetVertexCount();
        const Vector3& GetVertex(size_t idx);
        const std::vector<Vector3>& GetVertices() const;
        void PopVertex();
        bool CanAddVertex(Vector3 vertex) const;
    };

}