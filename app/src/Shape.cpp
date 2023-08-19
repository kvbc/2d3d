#include "Shape.hpp"
#include "Tesselator.hpp"

namespace App {

    Shape::Shape() {}

    bool Shape::canAddPoint(Vector2 gridPoint) {
        // skip 1st point check!
        for(size_t i = 1; i < points.size(); i++)
            if(Vector2Equals(points[i], gridPoint))
                return false;
        return true;
    }

    bool Shape::isComplete() {
        return m_isComplete;
    }

    void Shape::markAsComplete() {
        m_isComplete = true;

        std::vector<Vector2> gridPoints = points;
        gridPoints.pop_back();
        Tesselator::getInstance().tesselate(gridPoints, m_tesselatedPoints);
    }

    const std::vector<Vector2>& Shape::getTesselatedPoints() {
        return m_tesselatedPoints;
    }

}
