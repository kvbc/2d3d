#include "Shape.hpp"
#include "Tesselator.hpp"

#include <algorithm>

namespace App {

    Shape::Shape() {}

    // Face

    void Shape::AddFace(const Shape::Face& face) {
        m_faces.push_back(face);
    }

    const std::vector<Shape::Face>& Shape::GetFaces() const {
        return m_faces;
    }

    std::vector<Vector3> Shape::GetFaceVertices(const Face& face) const {
        std::vector<Vector3> vertices;
        for(size_t vertexIndex : face) {
            Vector3 vertex = GetVertex(vertexIndex);
            vertices.push_back(vertex);
        }
        return vertices;
    }

    // Vertices

    void Shape::AddVertex(Vector3 vertex) {
        m_vertices.push_back(vertex);
    }

    size_t Shape::GetVertexCount() const {
        return m_vertices.size();
    }

    const Vector3& Shape::GetVertex(size_t idx) const {
        return m_vertices[idx];
    }

    // void Shape::RemoveVertex(Vector3 vertex) {
    //     m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(), [=](Vector3 itVertex) {
    //         return Vector3Equals(itVertex, vertex);
    //     }), m_vertices.end());
    // }

    // void Shape::PopVertex() {
    //     m_vertices.pop_back();
    // }

    const std::vector<Vector3>& Shape::GetVertices() const {
        return m_vertices;
    }
}
