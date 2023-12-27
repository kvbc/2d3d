#include "Shape.hpp"
#include "Tesselator.hpp"
#include "App.hpp"

#include <iostream>
#include <algorithm>

namespace App {

    // 
    // Public
    // 

    Shape::Shape() {
        m_model = { 0 };
        m_model.transform = MatrixIdentity();
        m_model.materialCount = 1;
        m_model.materials = (Material*)MemAlloc(sizeof(Material));
        m_model.materials[0] = LoadMaterialDefault();
        // addMesh(GenMeshCube(3, 3, 3));
    }

    Shape::~Shape() {
        // UnloadModel(m_model);
    }

    // Face

    void Shape::AddFace(const Shape::Face& face, Vector3 normal) {
        m_faces.push_back(face);

        std::vector<Vector3> faceVertices = GetFaceVertices(face);
        std::reverse(faceVertices.begin(), faceVertices.end());
        const std::vector<Vector3>& tessFaceVertices = Tesselator::Get().Tesselate3D(faceVertices);

        Mesh mesh = { 0 };
        mesh.vertexCount = tessFaceVertices.size();
        mesh.triangleCount = mesh.vertexCount / 3;
        
        mesh.vertices  = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
        mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
        mesh.normals   = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
        mesh.colors = (unsigned char*)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));

        for(size_t i = 0; i < tessFaceVertices.size(); i++) {
            Vector3 vertex = tessFaceVertices[i];
            
            mesh.vertices[i * 3 + 0] = vertex.x;
            mesh.vertices[i * 3 + 1] = vertex.y;
            mesh.vertices[i * 3 + 2] = vertex.z;

            mesh.texcoords[i * 2 + 0] = 0;
            mesh.texcoords[i * 2 + 1] = 0;

            mesh.normals[i * 3 + 0] = normal.x;
            mesh.normals[i * 3 + 1] = normal.y;
            mesh.normals[i * 3 + 2] = normal.z;

            // mesh.colors[i * 4 + 0] = GetRandomValue(127, 255);
            // mesh.colors[i * 4 + 1] = GetRandomValue(127, 255);
            // mesh.colors[i * 4 + 2] = GetRandomValue(127, 255);
            // mesh.colors[i * 4 + 3] = 255;
            Color color = App::GetRandomColor();
            mesh.colors[i * 4 + 0] = color.r;
            mesh.colors[i * 4 + 1] = color.g;
            mesh.colors[i * 4 + 2] = color.b;
            mesh.colors[i * 4 + 3] = 255;
        }

        UploadMesh(&mesh, false);
        addMesh(mesh);
    }

    const std::vector<Shape::Face>& Shape::GetFaces() const {
        return m_faces;
    }

    std::vector<Vector3> Shape::GetFaceVertices(const Face& face) const {
        std::vector<Vector3> vertices;
        for(size_t vertexIndex : face) {
            Vector3 vertex = GetVertex(vertexIndex);
            vertices.push_back(vertex);
        }
        return vertices;
    }

    // Vertices

    void Shape::AddVertex(Vector3 vertex) {
        m_vertices.push_back(vertex);
    }

    size_t Shape::GetVertexCount() const {
        return m_vertices.size();
    }

    const Vector3& Shape::GetVertex(size_t idx) const {
        return m_vertices.at(idx);
    }

    // void Shape::RemoveVertex(Vector3 vertex) {
    //     m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(), [=](Vector3 itVertex) {
    //         return Vector3Equals(itVertex, vertex);
    //     }), m_vertices.end());
    // }

    // void Shape::PopVertex() {
    //     m_vertices.pop_back();
    // }

    const std::vector<Vector3>& Shape::GetVertices() const {
        return m_vertices;
    }

    const Model& Shape::GetModel() const {
        return m_model;
    }

    // 
    // Private
    // 

    void Shape::addMesh(Mesh mesh) {
        size_t idx = m_model.meshCount;

        m_model.meshCount++;
        m_model.meshes = (Mesh*)MemRealloc(m_model.meshes, sizeof(Mesh) * m_model.meshCount);
        m_model.meshMaterial = (int*)MemRealloc(m_model.meshMaterial, sizeof(int) * m_model.meshCount);

        m_model.meshes[idx] = mesh;
        m_model.meshMaterial[idx] = 0; // first (default) material

        //UploadMesh(&mesh, false);
    }
}
