#pragma once

#include "EditWindow.hpp"
#include "Shape.hpp"
#include "ViewWindow.hpp"

#include <vector>

namespace App {

    class App {
    public:
        static App& Get();

        void Update();
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