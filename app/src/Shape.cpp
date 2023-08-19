#include "Shape.hpp"
#include "Tesselator.hpp"

namespace App {

    Shape::Shape() {}

    bool Shape::canAddVertex(Vector3 vertex) const {
        // skip 1st point check!
        for(size_t i = 1; i < m_vertices.size(); i++)
            if(Vector3Equals(m_vertices[i], vertex))
                return false;
        return true;
    }

    bool Shape::isComplete() const {
        return m_isComplete;
    }

    void Shape::markAsComplete() {
        m_isComplete = true;
    }

    void Shape::addVertex(Vector3 vertex) {
        m_vertices.push_back(vertex);
    }

    size_t Shape::getVertexCount() {
        return m_vertices.size();
    }

    const Vector3& Shape::getVertex(size_t idx) {
        return m_vertices[idx];
    }

    void Shape::popVertex() {
        m_vertices.pop_back();
    }

    const std::vector<Vector3>& Shape::getVertices() const {
        return m_vertices;
    }
}
