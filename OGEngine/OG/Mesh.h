/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Contains Vertex Information and Draw Function, Move Model Vertices to fit [-1, 1] scale at the origin.
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef SIMPLE_OBJ_SCENE_MESH_H
#define SIMPLE_OBJ_SCENE_MESH_H

#include <glad/glad.h>
#include <gl/GL.h>
#include <glm/fwd.hpp>

#include <memory>

namespace OG 
{
    class BufferObject;
    class ArrayBufferObject;

    struct Vertex
    {
        Vertex()
        {
            vertex_position.x = 0.0f;
            vertex_position.y = 0.0f;
            vertex_position.z = 0.0f;

            vertex_uv.x = 0.0f;
            vertex_uv.y = 0.0f;

            vertex_normal.x = 0.0f;
            vertex_normal.y = 0.0f;
            vertex_normal.z = 0.0f;
        }

        glm::vec3 vertex_position;
        glm::vec2 vertex_uv;
        glm::vec3 vertex_normal;
    };

    class Mesh
    {
    public:
        friend class OBJReader;

        Mesh();
        Mesh(Mesh&& mesh) noexcept;
        Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices /*, std::vector<Texture>& textures*/);
        ~Mesh();

        // Get attribute values buffer
        GLfloat* getVertexBuffer();             // attribute 0
        GLfloat* getVertexNormals();            // attribute 1
        GLfloat* getVertexUVs();                // attribute 2

        GLfloat* getVertexNormalsForDisplay();  // attribute 0

        unsigned int getVertexBufferSize();
        unsigned int getVertexCount();
        unsigned int getVertexNormalCount();

        // Get vertex index buffer
        GLuint* getIndexBuffer();
        unsigned int getIndexBufferSize();
        unsigned int getTriangleCount();

        glm::vec3   getModelScale();
        glm::vec3   getModelCentroid();
        glm::vec3   getCentroidVector(glm::vec3 vVertex);

        GLfloat& getNormalLength();
        void setNormalLength(GLfloat nLength);

        // calculate and move vertex position to fit in [-1,1] box
        void calcVertexPositionForBoundingBox();


        // calculate vertex normals
        int calcVertexNormals(GLboolean bFlipNormals = false);

        // calculate the "display" normals
        void calcVertexNormalsForDisplay(GLboolean bFlipNormals = false);

        // calculate texture coordinates
        enum class UVType {
            PLANAR_UV = 0,
            CYLINDRICAL_UV,
            SPHERICAL_UV,
            CUBE_MAPPED_UV
        };

        int         calcUVs(Mesh::UVType uvType = Mesh::UVType::PLANAR_UV);
        glm::vec2   calcCubeMap(glm::vec3 vEntity);

        void Draw(GLenum mode);
        void DrawNormal(bool isFaceNormal);
        void SetBuffer();
        void SetNormalDrawBuffer();

        static Mesh* CreateSphere(float radius, int sectorCount, int stackCount);

    private:
        // Vertex, Index Buffer and Vertex Array Buffer holding Mesh data 
        std::unique_ptr<ArrayBufferObject> pVAO;
        std::unique_ptr<BufferObject> pVBO;
        std::unique_ptr<BufferObject> pEBO;

        // Interleaved Mesh Data
        std::vector<Vertex>       vertexBuffer_;
        std::vector<GLuint>       indices_;

        // Temporary container to make Interleaved Data
        std::vector<glm::vec3>    vertexPosition;
        std::vector<GLuint>       vertexPosIndices;
        std::vector<GLuint>       vertexUVsIndices;
        std::vector<GLuint>       vertexNormalsIndices;
        std::vector<glm::vec2>    vertexUVs;
        std::vector<glm::vec3>    vertexNormals;

        // Model vertex maximum, minimum value
        glm::vec3               boundingBox[2];
        GLfloat                 normalLength;

        // Vertex Buffer for Normal Draw
        std::vector<glm::vec3>    vertexNormalDisplay, faceNormalDisplay;
        std::unique_ptr<ArrayBufferObject> pVAO_FaceNorm;
        std::unique_ptr<BufferObject> pVBO_FaceNorm;

        std::unique_ptr<ArrayBufferObject> pVAO_VertNorm;
        std::unique_ptr<BufferObject> pVBO_VertNorm;
    };
}



#endif //SIMPLE_OBJ_SCENE_MESH_H
