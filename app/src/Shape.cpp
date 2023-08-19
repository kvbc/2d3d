#include "Shape.hpp"
#include "Tesselator.hpp"

namespace App {

    Shape::Shape() {}

    bool Shape::CanAddVertex(Vector3 vertex) const {
        // skip 1st point check!
        for(size_t i = 1; i < m_vertices.size(); i++)
            if(Vector3Equals(m_vertices[i], vertex))
                return false;
        return true;
    }

    bool Shape::IsComplete() const {
        return m_isComplete;
    }

    void Shape::MarkAsComplete() {
        m_isComplete = true;
    }

    void Shape::AddVertex(Vector3 vertex) {
        m_vertices.push_back(vertex);
    }

    size_t Shape::GetVertexCount() {
        return m_vertices.size();
    }

    const Vector3& Shape::GetVertex(size_t idx) {
        return m_vertices[idx];
    }

    void Shape::PopVertex() {
        m_vertices.pop_back();
    }

    const std::vector<Vector3>& Shape::GetVertices() const {
        return m_vertices;
    }
}
