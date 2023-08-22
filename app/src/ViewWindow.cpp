#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"

#include "ViewWindow.hpp"
#include "Shape.hpp"
#include "App.hpp"
#include "Tesselator.hpp"

namespace App {

    // 
    // Public
    // 

    ViewWindow::ViewWindow():
        m_orientCubeTxt(LoadRenderTexture(ORIENT_CUBE_WINDOW_SIZE, ORIENT_CUBE_WINDOW_SIZE))
    {
        resetCamera();

        m_orientCubeCamera.fovy = 90;
        m_orientCubeCamera.position = Vector3{ 0, 0, ORIENT_CUBE_SIZE + ORIENT_CUBE_CAMERA_OFFSET };
        m_orientCubeCamera.projection = CAMERA_PERSPECTIVE;
        m_orientCubeCamera.target = Vector3{ 0,0,0 };
        m_orientCubeCamera.up = Vector3{ 0,1,0 };

        generateOrientationCubeModel();
    }

    ViewWindow::~ViewWindow() {
        UnloadRenderTexture(m_orientCubeTxt);
        UnloadModel(m_orientCubeModel);
    }

    void ViewWindow::Update() {
        updateCamera();
        updateOrientationCubeCamera();
        drawWindow();
    }

    // 
    // Private
    // 

    void ViewWindow::resetCamera() {
        m_camera.fovy = 90;
        m_camera.position = Vector3{5,5,5};
        m_camera.projection = CAMERA_PERSPECTIVE;
        m_camera.target = Vector3{0,0,0};
        m_camera.up = Vector3{0,1,0};
    }

    void ViewWindow::updateCamera() {
        // move
        if(IsKeyDown(KEY_W)) CameraMoveForward(&m_camera,  CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_A)) CameraMoveRight  (&m_camera, -CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_S)) CameraMoveForward(&m_camera, -CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_D)) CameraMoveRight  (&m_camera,  CAMERA_MOVE_SPEED, false);
        if(IsKeyDown(KEY_SPACE     )) CameraMoveUp(&m_camera,  CAMERA_MOVE_SPEED);
        if(IsKeyDown(KEY_LEFT_SHIFT)) CameraMoveUp(&m_camera, -CAMERA_MOVE_SPEED);

        // rotate (arrows)
        if(IsKeyDown(KEY_DOWN )) CameraPitch(&m_camera, -CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_UP   )) CameraPitch(&m_camera,  CAMERA_ROTATION_SPEED, false, false, false);
        if(IsKeyDown(KEY_RIGHT)) CameraYaw  (&m_camera, -CAMERA_ROTATION_SPEED, false);
        if(IsKeyDown(KEY_LEFT )) CameraYaw  (&m_camera,  CAMERA_ROTATION_SPEED, false);
        // rotate (mouse)
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            CameraYaw  (&m_camera, -1 * GetMouseDelta().x * CAMERA_MOUSE_MOVE_SENSITIVITY, false);
            CameraPitch(&m_camera, -1 * GetMouseDelta().y * CAMERA_MOUSE_MOVE_SENSITIVITY, false, false, false);
        }

        // zoom
        CameraMoveToTarget(&m_camera, -GetMouseWheelMove());

        // pan
        // TODO: this is terrible for some reason (?)
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            const Vector2 mouseDelta = GetMouseDelta();
            if(mouseDelta.x > 0) CameraMoveRight(&m_camera, -CAMERA_PAN_SPEED, false);
            if(mouseDelta.x < 0) CameraMoveRight(&m_camera,  CAMERA_PAN_SPEED, false);
            if(mouseDelta.y > 0) CameraMoveUp   (&m_camera,  CAMERA_PAN_SPEED);
            if(mouseDelta.y < 0) CameraMoveUp   (&m_camera, -CAMERA_PAN_SPEED);
        }
    }

    void ViewWindow::drawWindow() {
        if(ImGui::Begin("3D View", NULL, ImGuiWindowFlags_MenuBar)) {
            if(ImGui::BeginMenuBar()) {
                if(ImGui::BeginMenu("Camera")) {
                    if(ImGui::MenuItem("Reset"))
                        resetCamera();
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            drawScene();
        }
        ImGui::End();
    }

    void ViewWindow::drawScene() {
        drawOrientationCubeTexture();

        redrawTexture();
        renderTexture();
    }

    void ViewWindow::drawTexture() {
        ClearBackground(DARKGRAY);

        BeginMode3D(m_camera);

        drawGrid(100, 1);

        for(const Shape& shape : App::Get().GetShapes()) {
            const Model& model = shape.GetModel();

            for(size_t meshIdx = 0; meshIdx < model.meshCount; meshIdx++) {
                Mesh * mesh = &model.meshes[meshIdx];
                for(size_t vxIdx = 0; vxIdx < mesh->vertexCount; vxIdx++) {
                    float* fVertex = &mesh->vertices[vxIdx * 3];
                    float* fNormal = &mesh->normals[vxIdx * 3];
                    Vector3 vertex = { fVertex[0], fVertex[1], fVertex[2] };
                    Vector3 normal = { fNormal[0], fNormal[1], fNormal[2] };

                    Vector3 endPos = Vector3Add(vertex, normal);
                    float radius = 0.1;
                    int sides = 5;
                    Color color = WHITE;
                    DrawCylinderEx(vertex, endPos, radius, radius, sides, color);
                }
            }

            DrawModel(
                model,
                Vector3Zero(),
                1,
                WHITE
            );
        }        

        EndMode3D();

        DrawTexture(m_orientCubeTxt.texture, 0, 0, WHITE);
    }

    // modified version of raylib's DrawGrid()
    void ViewWindow::drawGrid(int slices, float spacing) {
        int halfSlices = slices / 2;

        rlBegin(RL_LINES);
        for (int i = -halfSlices; i <= halfSlices; i++)
        {
            Color color = (i == 0) ? GRID_AXIS_COLOR : GRID_COLOR;
            for(size_t j = 0; j < 4; j++)
                rlColor3f(
                    color.r / 255.0,
                    color.g / 255.0,
                    color.b / 255.0
                );

            rlVertex3f((float)i*spacing, 0.0f, (float)-halfSlices*spacing);
            rlVertex3f((float)i*spacing, 0.0f, (float)halfSlices*spacing);

            rlVertex3f((float)-halfSlices*spacing, 0.0f, (float)i*spacing);
            rlVertex3f((float)halfSlices*spacing, 0.0f, (float)i*spacing);
        }
        rlEnd();
    }

    void ViewWindow::drawOrientationCubeTexture() {
        BeginTextureMode(m_orientCubeTxt);
        BeginMode3D(m_orientCubeCamera);
        ClearBackground(BLANK);
        
        //DrawCube(Vector3Zero(), ORIENT_CUBE_SIZE, ORIENT_CUBE_SIZE, ORIENT_CUBE_SIZE, ORIENT_CUBE_COLOR);
        DrawModel(m_orientCubeModel, Vector3Zero(), 1, WHITE);

        EndMode3D();
        EndTextureMode();
    }

    void ViewWindow::updateOrientationCubeCamera() {
        Vector3 lookVector = GetCameraForward(&m_camera);
        Vector3 cubeSize = { ORIENT_CUBE_SIZE, ORIENT_CUBE_SIZE, ORIENT_CUBE_SIZE };
        Vector3 halfCubeSize = Vector3Divide(cubeSize, { 2,2,2 });
        Vector3 camOffset = Vector3AddValue(halfCubeSize, ORIENT_CUBE_CAMERA_OFFSET);
        m_orientCubeCamera.position = Vector3Negate(Vector3Multiply(lookVector, camOffset));
    }

    void ViewWindow::generateOrientationCubeModel() {
        float hw = ORIENT_CUBE_SIZE / 2; // half width  (X)
        float hh = ORIENT_CUBE_SIZE / 2; // half height (Y)
        float hd = ORIENT_CUBE_SIZE / 2; // half depth  (Z

        Mesh frontMesh = generateOrientationCubeMesh({
            -hw,  hh, hd,
            -hw, -hh, hd,
             hw, -hh, hd,
             hw,  hh, hd
        });
        Mesh backMesh = generateOrientationCubeMesh({
             hw,  hh, -hd,
             hw, -hh, -hd,
            -hw, -hh, -hd,
            -hw,  hh, -hd
        });
        Mesh leftMesh = generateOrientationCubeMesh({
            -hw,  hh, -hd,
            -hw, -hh, -hd,
            -hw, -hh,  hd,
            -hw,  hh,  hd
        });
        Mesh rightMesh = generateOrientationCubeMesh({
            hw,  hh,  hd,
            hw, -hh,  hd,
            hw, -hh, -hd,
            hw,  hh, -hd
        });

        Model model = { 0 };
        model.transform = MatrixIdentity();

        model.meshCount = 4;
        model.meshes = (Mesh*)MemAlloc(model.meshCount * sizeof(Mesh));
        model.meshes[0] = frontMesh;
        model.meshes[1] = backMesh;
        model.meshes[2] = leftMesh;
        model.meshes[3] = rightMesh;

        model.materialCount = 1;
        model.materials = (Material*)MemAlloc(model.materialCount * sizeof(Material));
        model.materials[0] = LoadMaterialDefault();

        model.meshMaterial = (int*)MemAlloc(model.meshCount * sizeof(int));
        model.meshMaterial[0] = 0;
        model.meshMaterial[1] = 0;
        model.meshMaterial[2] = 0;
        model.meshMaterial[3] = 0;

        m_orientCubeModel = model;
    }

    Mesh ViewWindow::generateOrientationCubeMesh(const std::array<float,4*3>& vertices) {
        Mesh mesh = { 0 };

        mesh.triangleCount = 2;
        mesh.vertexCount = vertices.size() / 3;

        size_t verticesByteSize = vertices.size() * sizeof(float);
        mesh.vertices = (float*)MemAlloc(verticesByteSize);
        memcpy(mesh.vertices, vertices.data(), verticesByteSize);
        
        unsigned short indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        mesh.indices = (unsigned short*)MemAlloc(sizeof(indices));
        memcpy(mesh.indices, indices, sizeof(indices));
        
        UploadMesh(&mesh, false);
        return mesh;
    }
}