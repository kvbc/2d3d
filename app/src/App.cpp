#include "App.hpp"

namespace App {

    // 
    // Public
    // 

    App& App::getInstance() {
        static App app;
        return app;
    }

    void App::update() {
        m_XEditWindow.update();
        m_YEditWindow.update();
        m_ZEditWindow.update();
    }

    // 
    // Private
    // 

    App::App():
        m_XEditWindow("X View"),
        m_YEditWindow("Y View"),
        m_ZEditWindow("Z View")
    {}

}