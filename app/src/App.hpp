#pragma once

#include "EditWindow.hpp"
#include "Shape.hpp"
#include "ViewWindow.hpp"
#include "raymath.h"

#include <vector>

namespace App {

    class App {
    public:
        enum class WindingOrder {
            CLOCKWISE,
            COUNTER_CLOCKWISE
        };

        static App& Get();

        void Update();

        static Color GetRandomColor();
        static WindingOrder DetermineWindingOrder2D(const std::vector<Vector2>& points);

        Shape& GetShape(size_t idx);
        size_t GetShapeCount() const;
        const std::vector<Shape>& GetShapes() const;
        void PushShape(const Shape& shape);
        void PopShape();

    private:
        App();

        std::vector<Shape> m_shapes;

        EditWindow m_XEditWindow;
        EditWindow m_YEditWindow;
        EditWindow m_ZEditWindow;
        ViewWindow m_viewWindow;
    };

}