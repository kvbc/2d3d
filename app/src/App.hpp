#pragma once

#include "EditWindow.hpp"

namespace App {

    class App {
    public:
        static App& getInstance();

        void update();

    private:
        App();

        EditWindow m_XEditWindow;
        EditWindow m_YEditWindow;
        EditWindow m_ZEditWindow;
    };

}