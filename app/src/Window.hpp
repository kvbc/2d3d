#pragma once

#include "raylib.h"

namespace App {

    class Window {
    protected:
        Window();
        ~Window();

        void redrawTexture();
        void renderTexture();
        virtual void drawTexture() = 0;

    private:
        bool m_redraw;
        RenderTexture2D m_renderTexture;
    };

}