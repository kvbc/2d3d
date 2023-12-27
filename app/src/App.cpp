#include "App.hpp"
#include <iostream>

namespace App {

    // 
    // Public
    // 

    App& App::Get() {
        static App app;
        return app;
    }

    void App::Update() {
        m_XEditWindow.Update();
        m_YEditWindow.Update();
        m_ZEditWindow.Update();
        m_viewWindow.Update();
    }

    Color App::GetRandomColor() {
        std::vector<Color> colors = { LIGHTGRAY, GRAY, DARKGRAY, YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN, SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN, MAGENTA };
        return colors[GetRandomValue(0, colors.size() - 1)];
    }

    App::WindingOrder App::GetReversedWindingOrder(App::WindingOrder windingOrder) {
        return
            (windingOrder == WindingOrder::CLOCKWISE)
            ? WindingOrder::COUNTER_CLOCKWISE
            : WindingOrder::CLOCKWISE;
    }

    App::WindingOrder App::DetermineWindingOrder2D(const std::vector<Vector2>& points) {
        // magic
        // https://stackoverflow.com/a/1165943/21398468
        float curve = 0;
        for(size_t i = 0; i < points.size(); i++) {
            size_t nextIdx = (i + 1) % points.size();
            float diffX = points[nextIdx].x - points[i].x;
            float sumY = points[nextIdx].y + points[i].y;
            curve += diffX * sumY;
        }
        return (curve >= 0) ? WindingOrder::CLOCKWISE : WindingOrder::COUNTER_CLOCKWISE;
    }

    // Shape

    Shape& App::GetShape(size_t idx) {
        return m_shapes.at(idx);
    }

    size_t App::GetShapeCount() const {
        return m_shapes.size();
    }

    const std::vector<Shape>& App::GetShapes() const {
        return m_shapes;
    }

    void App::PushShape(const Shape& shape) {
        m_shapes.push_back(shape);
    }

    void App::PopShape() {
        m_shapes.pop_back();
    }

    // 
    // Private
    // 

    App::App():
        m_XEditWindow(EditWindow::View::Right),
        m_YEditWindow(EditWindow::View::Top),
        m_ZEditWindow(EditWindow::View::Front)
    {}
}