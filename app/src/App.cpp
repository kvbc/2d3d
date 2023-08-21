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

    Shape& App::GetShape(size_t idx) {
        return m_shapes[idx];
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