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
    }

    Shape& App::GetShape(size_t idx) {
        return m_shapes[idx];
    }

    size_t App::GetShapeCount() const {
        return m_shapes.size();
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
        m_XEditWindow("X View", EditWindow::View::Right),
        m_YEditWindow("Y View", EditWindow::View::Up),
        m_ZEditWindow("Z View", EditWindow::View::Front)
    {}
}