
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include <iostream>

#include "App.hpp"
#include "EditWindow.hpp"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1900, 900, "raylib [ImGui] example - ImGui Demo");
    rlImGuiSetup(true);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    bool quit = false;
    while(!WindowShouldClose() && !quit) {
        BeginDrawing();
        rlImGuiBegin();

        ClearBackground(DARKGRAY);

        // create an ImGui window that covers the entire viewport, so that we can have a menu bar at the top of the application
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));                                               // always at the window origin
            ImGui::SetNextWindowSize(ImVec2(float(GetScreenWidth()), float(GetScreenHeight()))); // always at the window size
            ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoBringToFrontOnFocus | // we just want to use this window as a host for the menubar and docking
                ImGuiWindowFlags_NoNavFocus |            // so turn off everything that would make it act like a window
                ImGuiWindowFlags_NoDocking  |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize   |
                ImGuiWindowFlags_NoMove     |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_MenuBar    |
                ImGuiWindowFlags_NoBackground;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // we don't want any padding for windows docked to this window frame
            bool show = (ImGui::Begin("Main", NULL, windowFlags));          // show the "window"
            ImGui::PopStyleVar();                                           // restore the style so inner windows have fames

            if(show) {
                if(ImGui::BeginMenuBar()) {
                    if(ImGui::BeginMenu("File")) {
                        if(ImGui::MenuItem("Exit"))
                            quit = true;
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }
                // create a docking space inside our inner window that lets prevents anything from docking in the central node
                ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f),  ImGuiDockNodeFlags_PassthruCentralNode);
                ImGui::End();
            }
        }

        ImGui::ShowDemoWindow(NULL);

        ::App::App::Get().Update();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow(); 
}