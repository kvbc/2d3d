#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

namespace App {

    class Shape {
    public:
        // a face is a collection of vertex indices
        using Face = std::vector<size_t>;

    private:
        std::vector<Vector3> m_vertices;
        std::vector<Face> m_faces;

    public:
        Color color = BLUE;

        Shape();

        void AddFace(const Face& face);

        void AddVertex(Vector3 vertex);
        size_t GetVertexCount() const;
        const Vector3& GetVertex(size_t idx) const;
        const std::vector<Vector3>& GetVertices() const;
        // void RemoveVertex(Vector3 vertex);
        // void PopVertex();
    };

}