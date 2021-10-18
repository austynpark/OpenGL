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
#include "ogpch.h"
#include "Mesh.h"
#include "ArrayBufferObject.h"
#include "BufferObject.h"
#include "VertexAttributeManager.h"

namespace OG 
{
    Mesh::Mesh()
    {
        boundingBox[0] = glm::vec3(0, 0, 0);
        normalLength = 0.0f;
    }

    Mesh::Mesh(Mesh&& mesh) noexcept : pVAO(std::move(mesh.pVAO)), pVBO(std::move(mesh.pVBO)), pEBO(std::move(mesh.pEBO)),
        vertexBuffer_(mesh.vertexBuffer_), indices_(mesh.indices_), vertexPosition(mesh.vertexPosition),
        vertexPosIndices(mesh.vertexPosIndices), vertexUVsIndices(mesh.vertexUVsIndices), vertexNormalsIndices(mesh.vertexNormalsIndices),
        vertexUVs(mesh.vertexUVs), vertexNormals(mesh.vertexNormals), vertexNormalDisplay(mesh.vertexNormalDisplay) 
    {
        boundingBox[0] = mesh.boundingBox[0];
        boundingBox[1] = mesh.boundingBox[1];
        normalLength = mesh.normalLength;
    }
    Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
        : vertexBuffer_(std::move(vertices)), indices_(std::move(indices))
    {
        SetBuffer();
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Draw(GLenum mode)
    {
        pVAO->Bind();
        glDrawElements(mode, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);
        pVAO->UnBind();
    }

    void Mesh::DrawNormal(bool isFaceNormal)
    {
        if (isFaceNormal)
        {
            pVAO_FaceNorm->Bind();
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(faceNormalDisplay.size()));
            pVAO_FaceNorm->UnBind();
        }
        else
        {
            pVAO_VertNorm->Bind();
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexNormalDisplay.size()));
            pVAO_VertNorm->UnBind();
        }
    }

    void Mesh::SetBuffer()
    {
        // Bind function is called in the constructor
        pVAO = std::make_unique<ArrayBufferObject>(); // pVAO->Bind();
        pVBO = std::make_unique<BufferObject>(vertexBuffer_.data(), static_cast<GLuint>(vertexBuffer_.size() * sizeof(Vertex))); // pVBO->Bind();
        pEBO = std::make_unique<BufferObject>(indices_.data(), static_cast<GLuint>(indices_.size() * sizeof(GLuint)), GL_ELEMENT_ARRAY_BUFFER); // pEBO->Bind();

        // Vertex Attribute Setting
        VertexAttributeManager attributeManager;
        // Store VertexBufferElement as a vector that has size, type, type size, and store stride at VertexAttributeManager
        attributeManager.AddAttribute(3, GL_FLOAT, sizeof(GLfloat));
        attributeManager.AddAttribute(2, GL_FLOAT, sizeof(GLfloat));
        attributeManager.AddAttribute(3, GL_FLOAT, sizeof(GLfloat));
        // VertexBufferElement have size, type, type_size
        // Get stored VertexAttribute to set glVertexAttribPointer
        std::vector<VertexBufferElement> layouts = attributeManager.GetLayout();

        unsigned int lastIndex = static_cast<unsigned int>(layouts.size());
        uint64_t offset = 0;
        
        for (unsigned int i = 0; i < lastIndex; ++i)
        {
            const VertexBufferElement elem = layouts[i];
            glVertexAttribPointer(i, elem.count_, elem.type_, elem.isNormalized_, attributeManager.GetStride(), (const void*)(offset));
            glEnableVertexAttribArray(i);
            offset += elem.count_ * elem.typeSize_;
        }

        SetNormalDrawBuffer();
    }

    void Mesh::SetNormalDrawBuffer()
    {
        pVAO_FaceNorm = std::make_unique<ArrayBufferObject>(); // pVAO->Bind();
        pVBO_FaceNorm = std::make_unique<BufferObject>(faceNormalDisplay.data(), static_cast<GLuint>(faceNormalDisplay.size() * sizeof(glm::vec3))); // pVBO->Bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        pVAO_VertNorm = std::make_unique<ArrayBufferObject>(); // pVAO->Bind();
        pVBO_VertNorm = std::make_unique<BufferObject>(vertexNormalDisplay.data(), static_cast<GLuint>(vertexNormalDisplay.size() * sizeof(glm::vec3))); // pVBO->Bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // sectorCount = column count, stackCount = row count
    Mesh* Mesh::CreateSphere(float radius, int sectorCount, int stackCount)
    {
        const float PI = static_cast<float>(acos(-1));

        Mesh* sphere = new Mesh();

        float x, y, z, xy;                              // vertex position
        float lengthInv = 1.0f / radius;    // vertex normal

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                sphere->vertexPosition.emplace_back(x, y, z);
            }
        }

        int index = 0;
        int k1, k2;

        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0)
                {
                    sphere->vertexPosIndices.push_back(k1);
                    sphere->vertexPosIndices.push_back(k2);
                    sphere->vertexPosIndices.push_back(k1 + 1);

                    sphere->indices_.push_back(index++);
                    sphere->indices_.push_back(index++);
                    sphere->indices_.push_back(index++);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1))
                {
                    sphere->vertexPosIndices.push_back(k1 + 1);
                    sphere->vertexPosIndices.push_back(k2);
                    sphere->vertexPosIndices.push_back(k2 + 1);

                    sphere->indices_.push_back(index++);
                    sphere->indices_.push_back(index++);
                    sphere->indices_.push_back(index++);
                }

            }
        }

        sphere->calcVertexNormals();
        sphere->calcUVs(Mesh::UVType::SPHERICAL_UV);

        for (int i = 0; i < index; ++i)
        {
            Vertex vertex;
            vertex.vertex_position = sphere->vertexPosition.at(sphere->vertexPosIndices.at(i));
            vertex.vertex_uv = sphere->vertexUVs.at(sphere->vertexPosIndices.at(i));
            vertex.vertex_normal = sphere->vertexNormals.at(sphere->vertexPosIndices.at(i));

            sphere->vertexBuffer_.emplace_back(std::move(vertex));
        }

        sphere->SetBuffer();

        return sphere;
    }

    /////////////////////////////////////////////
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    GLfloat* Mesh::getVertexBuffer()
    {
        return reinterpret_cast<GLfloat*>(vertexPosition.data());
    }

    GLfloat* Mesh::getVertexNormals()
    {
        return reinterpret_cast<GLfloat*>(vertexNormals.data());
    }

    GLfloat* Mesh::getVertexUVs()
    {
        return reinterpret_cast<GLfloat*>(vertexUVs.data());
    }

    GLfloat* Mesh::getVertexNormalsForDisplay()
    {
        return reinterpret_cast<GLfloat*>(vertexNormalDisplay.data());
    }

    GLuint* Mesh::getIndexBuffer()
    {
        return vertexPosIndices.data();
    }

    ////////////////////////////////////
    ////////////////////////////////////
    ////////////////////////////////////
    unsigned int Mesh::getVertexBufferSize()
    {
        return (unsigned int)vertexPosition.size();
    }

    unsigned int Mesh::getIndexBufferSize()
    {
        return (unsigned int)vertexPosIndices.size();
    }

    unsigned int Mesh::getTriangleCount()
    {
        return getIndexBufferSize() / 3;
    }

    unsigned int Mesh::getVertexCount()
    {
        return getVertexBufferSize();
    }

    unsigned int Mesh::getVertexNormalCount()
    {
        return static_cast<unsigned>(vertexNormalDisplay.size());
    }

    glm::vec3  Mesh::getModelScale()
    {
        glm::vec3 scale = boundingBox[1] - boundingBox[0];

        if (scale.x == 0.0)
            scale.x = 1.0;

        if (scale.y == 0.0)
            scale.y = 1.0;

        if (scale.z == 0.0)
            scale.z = 1.0;

        return scale;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    glm::vec3  Mesh::getModelCentroid()
    {
        return glm::vec3(boundingBox[0] + boundingBox[1]) * 0.5f;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    glm::vec3 Mesh::getCentroidVector(glm::vec3 vVertex)
    {
        return glm::normalize(vVertex - getModelCentroid());
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    struct compareVec
    {
        bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
        {
            glm::highp_bvec3 result = glm::lessThan(lhs, rhs);

            return glm::epsilonEqual(lhs.x, rhs.x, 0.00001f) ?
                (glm::epsilonEqual(lhs.y, rhs.y, 0.00001f) ? (glm::epsilonEqual(lhs.x, rhs.x, 0.00001f) ? false : result.z) : result.y) : result.x;
        }
    };

    /////////////////////////////////////////////////////////
    int Mesh::calcVertexNormals(GLboolean bFlipNormals)
    {
        int rFlag = -1;

        // vertices and indices must be populated
        //if (vertexPosition.empty() || vertexPosIndices.empty())
        //{
        //    std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
        //    return rFlag;
        //}

        //// Pre-built vertex normals
        //if( !vertexNormals.empty() )
        //{
        //    std::cout << "Vertex normals specified by the file. Skipping this step." << std::endl;
        //    return rFlag;
        //}

        // Initialize vertex normals
        GLuint numVertices = getVertexCount();
        GLuint numFaces = static_cast<GLuint>(vertexPosIndices.size()) / 3;
        vertexNormals.resize(numVertices, glm::vec3(0.0f));
        vertexNormalDisplay.resize(static_cast<unsigned long long>(numVertices) * 2, glm::vec3(0.0f));
        faceNormalDisplay.resize(static_cast<unsigned long long>(numFaces) * 2, glm::vec3(0.0f));
        std::vector< std::set< glm::vec3 , compareVec> >  vNormalSet;
        vNormalSet.resize(numVertices);

        setNormalLength(1.0f);

        // For every face
        GLuint index = 0, face_index = 0;
        for (; index < vertexPosIndices.size(); )
        {
            GLuint a = vertexPosIndices.at(index++);
            GLuint b = vertexPosIndices.at(index++);
            GLuint c = vertexPosIndices.at(index++);

            glm::vec3  vA = vertexPosition[a];
            glm::vec3  vB = vertexPosition[b];
            glm::vec3  vC = vertexPosition[c];

            // Edge vectors
            glm::vec3  E1 = vB - vA;
            glm::vec3  E2 = vC - vA;

            glm::vec3  N = glm::normalize(glm::cross(E1, E2));

            glm::vec3 midPoint = (vA + vB + vC) / 3.0f;

            faceNormalDisplay.at(face_index++) = midPoint;
            faceNormalDisplay.at(face_index++) = midPoint + (N * normalLength);


            if (bFlipNormals)
                N = N * -1.0f;

            vNormalSet.at(a).insert(N);
            vNormalSet.at(b).insert(N);
            vNormalSet.at(c).insert(N);
        }

        // Now sum up the values per vertex
        for (int index = 0; index < vNormalSet.size(); ++index)
        {
            glm::vec3  vNormal(0.0f);

            //        if( vNormalSet[index].size() <= 0 )
            //            std::cout << "[Normal Set](" << index << ") Size = " << vNormalSet[index].size() << std::endl;

            // save normal to display
            glm::vec3  vA = vertexPosition[index];

            if (index == 5)
                int a = 5;
            auto nIt = vNormalSet[index].begin();
            while (nIt != vNormalSet[index].end())
            {
                vNormal += (*nIt);
                ++nIt;
            }

            int size = (int)vNormalSet[index].size();
            // save vertex normal[set
            vertexNormals[index] = glm::normalize(vNormal);

            vertexNormalDisplay[static_cast<unsigned long long>(index) * 2] = vA;
            vertexNormalDisplay[(static_cast<unsigned long long>(index) * 2) + 1] = vA + (normalLength * vertexNormals[index]);

        }

        // success
        rFlag = 0;

        return rFlag;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    void Mesh::calcVertexNormalsForDisplay(GLboolean bFlipNormals)
    {
        GLuint numVertices = getVertexCount();
        vertexNormalDisplay.resize(static_cast<unsigned long long>(numVertices) * 2, glm::vec3(0.0f));

        for (int iNormal = 0; iNormal < vertexNormals.size(); ++iNormal)
        {
            glm::vec3 normal = vertexNormals[iNormal] * normalLength;

            vertexNormalDisplay[static_cast<unsigned long long>(iNormal) * 2] = vertexPosition[iNormal];
            vertexNormalDisplay[(static_cast<unsigned long long>(iNormal) * 2) + 1] = vertexPosition[iNormal] + normal;
        }
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    GLfloat& Mesh::getNormalLength()
    {
        return normalLength;
    }

    void Mesh::setNormalLength(GLfloat nLength)
    {
        normalLength = nLength;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    int Mesh::calcUVs(Mesh::UVType uvType)
    {
        int rFlag = -1;

        // clear any existing UV
        vertexUVs.clear();

        glm::vec3 delta = getModelScale();
        int vBsize = static_cast<int>(vertexPosition.size());
        for (int nVertex = 0; nVertex < vBsize; ++nVertex)
        {
            glm::vec3 V = vertexPosition[nVertex];
            glm::vec2 uv(0.0f);

            glm::vec3 normVertex = glm::vec3((V.x - boundingBox[0].x) / delta.x,
                (V.y - boundingBox[0].y) / delta.y,
                (V.z - boundingBox[0].z) / delta.z);

            //        normVertex = (2.0f * normVertex) - glm::vec3(1.0f);

            glm::vec3 centroidVec = getCentroidVector(V);

            float theta(0.0f);
            float z(0.0f);
            float phi(0.0f);

            switch (uvType)
            {
            case Mesh::UVType::PLANAR_UV:
                uv.x = (normVertex.x - (-1.0f)) / (2.0f);
                uv.y = (normVertex.y - (-1.0f)) / (2.0f);
                break;

            case Mesh::UVType::CYLINDRICAL_UV:
                theta = glm::degrees(static_cast<float>(atan2(centroidVec.y, centroidVec.x)));
                theta += 180.0f;

                z = (centroidVec.z + 1.0f) * 0.5f;

                uv.x = theta / 360.0f;
                uv.y = z;
                break;

            case Mesh::UVType::SPHERICAL_UV:
                theta = glm::degrees(static_cast<float>(glm::atan(centroidVec.y, centroidVec.x)));
                theta += 180.0f;

                z = centroidVec.z;
                phi = glm::degrees(glm::acos(z / centroidVec.length()));

                uv.x = theta / 360.0f;
                uv.y = 1.0f - (phi / 180.0f);
                break;

            case Mesh::UVType::CUBE_MAPPED_UV:
                uv = calcCubeMap(centroidVec);
                break;
            }

            vertexUVs.push_back(uv);
        }

        return rFlag;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    glm::vec2 Mesh::calcCubeMap(glm::vec3 vEntity)
    {
        float x = vEntity.x;
        float y = vEntity.y;
        float z = vEntity.z;

        float absX = abs(x);
        float absY = abs(y);
        float absZ = abs(z);

        int isXPositive = x > 0 ? 1 : 0;
        int isYPositive = y > 0 ? 1 : 0;
        int isZPositive = z > 0 ? 1 : 0;

        float maxAxis, uc, vc;
        glm::vec2 uv = glm::vec2(0.0);

        // POSITIVE X
        if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
        {
            // u (0 to 1) goes from +z to -z
            // v (0 to 1) goes from -y to +y
            maxAxis = absX;
            uc = -z;
            vc = y;
        }

        // NEGATIVE X
        else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
        {
            // u (0 to 1) goes from -z to +z
            // v (0 to 1) goes from -y to +y
            maxAxis = absX;
            uc = z;
            vc = y;
        }

        // POSITIVE Y
        else if (bool(isYPositive) && absY >= absX && absY >= absZ)
        {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from +z to -z
            maxAxis = absY;
            uc = x;
            vc = -z;
        }

        // NEGATIVE Y
        else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
        {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -z to +z
            maxAxis = absY;
            uc = x;
            vc = z;
        }

        // POSITIVE Z
        else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
        {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -y to +y
            maxAxis = absZ;
            uc = x;
            vc = y;
        }

        // NEGATIVE Z
        else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
        {
            // u (0 to 1) goes from +x to -x
            // v (0 to 1) goes from -y to +y
            maxAxis = absZ;
            uc = -x;
            vc = y;
        }

        // Convert range from -1 to 1 to 0 to 1
        uv.s = 0.5f * (uc / maxAxis + 1.0f);
        uv.t = 0.5f * (vc / maxAxis + 1.0f);

        return uv;
    }

    /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////

    void Mesh::calcVertexPositionForBoundingBox(glm::vec3 centroid)
    {
        float distX = abs(boundingBox[1].x - boundingBox[0].x) * 0.5f;
        float distY = abs(boundingBox[1].y - boundingBox[0].y) * 0.5f;
        float distZ = abs(boundingBox[1].z - boundingBox[0].z) * 0.5f;

        distX = distX != 0.0f ? distX : 1;
        distY = distY != 0.0f ? distY : 1;
        distZ = distZ != 0.0f ? distZ : 1;

        float maxDist = std::max(std::max(distX, distY), distZ);

        for (int i = 0; i < vertexPosition.size(); ++i) {
            vertexPosition.at(i) -= centroid;
            vertexPosition.at(i) /= maxDist;
        }
    }
}