
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1900;
    int screenHeight = 900;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [ImGui] example - ImGui Demo");
    SetTargetFPS(75);
    rlImGuiSetup(true);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    // setup a background texture
    Image img = GenImageChecked(64, 64, 16, 16, DARKGRAY, GRAY);
    Texture bg = LoadTextureFromImage(img);
    UnloadImage(img);

    // flag so we can quit the game from the file menu
    bool quit = false;
    // Main game loop
    while (!WindowShouldClose() && !quit)    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        // fill the background with something interesting
        DrawTextureRec(bg, Rectangle{ 0, 0, float(GetScreenWidth()), float(GetScreenHeight()) }, Vector2Zero(), WHITE);
        DrawRectanglePro(Rectangle{ GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f,200,200 }, Vector2{ 100,100 }, float(GetTime()) * 45, ColorAlpha(RED,0.75f));
        // start the GUI
        rlImGuiBegin();

        // create an ImGui window that covers the entire viewport, so that we can have a menu bar at the top of the applications
        ImGui::SetNextWindowPos(ImVec2(0, 0));                                                  // always at the window origin
        ImGui::SetNextWindowSize(ImVec2(float(GetScreenWidth()), float(GetScreenHeight())));    // always at the window size

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus |                 // we just want to use this window as a host for the menubar and docking
            ImGuiWindowFlags_NoNavFocus |                                                      // so turn off everything that would make it act like a window
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoBackground;                                                      // we want our game content to show through this window, so turn off the background.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));                          // we don't want any padding for windows docked to this window frame
        bool show = (ImGui::Begin("Main", NULL, windowFlags));                                   // show the "window"
        ImGui::PopStyleVar();                                                                    // restore the style so inner windows have fames
        // create a docking space inside our inner window that lets prevents anything from docking in the central node (so we can see our game content)
        ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f),  ImGuiDockNodeFlags_PassthruCentralNode);
        if (show)
        {
            // Do a menu bar with an exit menu
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit"))
                        quit = true;

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
        }
        ImGui::End();
        // show any other windows that we want to be dockable
        ImGui::ShowDemoWindow(NULL);

        // end ImGui
        rlImGuiEnd();

        // Finish drawing
        DrawFPS(0, GetScreenHeight() - 20);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    rlImGuiShutdown();
    UnloadTexture(bg);
    CloseWindow(); 
    return 0;
}