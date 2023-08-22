#include "Tesselator.hpp"
#include "App.hpp"

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

    Tesselator& Tesselator::Get() {
        static Tesselator instance;
        return instance;
    }

    std::vector<Vector2> Tesselator::Tesselate2D(const std::vector<Vector2>& points) {
        std::vector<Vector2> outPoints;

        //App::WindingOrder windingOrder = App::DetermineWindingOrder2D(points);
        //assert(windingOrder == App::WindingOrder::COUNTER_CLOCKWISE);
        //tessSetOption(m_tess, TESS_REVERSE_CONTOURS, windingOrder == App::WindingOrder::CLOCKWISE ? 0 : 1); // reverse when counter-clockwise

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

        return outPoints;
    }

    std::vector<Vector3> Tesselator::Tesselate3D(const std::vector<Vector3>& vertices) {
        std::vector<float> floatVertices;
        for(const Vector3& vertex : vertices) {
            floatVertices.push_back(vertex.x);
            floatVertices.push_back(vertex.y);
            floatVertices.push_back(vertex.z);
        }

        tessAddContour(m_tess, 3, floatVertices.data(), sizeof(float)*3, vertices.size());
        tessTesselate(m_tess, TESS_WINDING_POSITIVE, TESS_POLYGONS, 3, 3, NULL);

        std::vector<Vector3> outVertices;
        const float * tessVertices = tessGetVertices(m_tess);
        const int elementCount = tessGetElementCount(m_tess);
        const int * elements = tessGetElements(m_tess);
        // https://github.com/memononen/libtess2/blob/fc52516467dfa124bdd967c15c7cf9faf02a34ca/Example/example.c#L356
        for(size_t i = 0; i < elementCount; i++) {
            const int * p = &elements[i * 3];
            for(size_t j = 0; j < 3 && p[j] != TESS_UNDEF; j++) {
                float x = tessVertices[p[j]*3];
                float y = tessVertices[p[j]*3+1];
                float z = tessVertices[p[j]*3+2];
                outVertices.push_back({x, y, z});
            }
        }

        return outVertices;
    }

}