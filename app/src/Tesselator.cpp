#include "Tesselator.hpp"
#include <iostream> 
#include <assert.h>

namespace App {

    // 
    // Private
    // 

    Tesselator::Tesselator() {
        m_tess = tessNewTess(NULL);
        assert(m_tess != NULL);
    }

    Tesselator::~Tesselator() {
        tessDeleteTess(m_tess);
    }

    // 
    // Public
    // 

    Tesselator& Tesselator::getInstance() {
        static Tesselator instance;
        return instance;
    }

    void Tesselator::tesselate(const std::vector<Vector2>& points, std::vector<Vector2>& outPoints) {
        outPoints.clear();

        std::vector<float> floatPoints;
        for(const Vector2& point : points) {
            floatPoints.push_back(point.x);
            floatPoints.push_back(point.y);
        }

        tessAddContour(m_tess, 2, floatPoints.data(), sizeof(float)*2, points.size());
        tessTesselate(m_tess, TESS_WINDING_POSITIVE, TESS_POLYGONS, 3, 2, NULL);

        const float * vertices = tessGetVertices(m_tess);
        const int elementCount = tessGetElementCount(m_tess);
        const int * elements = tessGetElements(m_tess);
        // https://github.com/memononen/libtess2/blob/fc52516467dfa124bdd967c15c7cf9faf02a34ca/Example/example.c#L356
        for(size_t i = 0; i < elementCount; i++) {
            const int * p = &elements[i * 3];
            for(size_t j = 0; j < 3 && p[j] != TESS_UNDEF; j++) {
                float x = vertices[p[j]*2];
                float y = vertices[p[j]*2+1];
                outPoints.push_back({x, y});
            }
        }
    }

}