/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJReader.cpp
Purpose: Object Reader is made as a Singleton Instance and Called from the Model to read model (mesh)-
info from *.obj file
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "OBJReader.h"



namespace OG
{
    OBJReader* OBJReader::objectReader_ = nullptr;

    OBJReader::OBJReader()
    {
    }

    OBJReader* OBJReader::getInstance()
    {
        if (objectReader_ == nullptr)
        {
            objectReader_ = new OBJReader();
        }
        return objectReader_;
    }

    OBJReader::~OBJReader()
    {
        delete objectReader_;
    }

    /////////////////////////////////////////////
    /////////////////////////////////////////////
    /////////////////////////////////////////////

    /////////////////////////////////////////////
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    double OBJReader::ReadOBJFile(const std::string& filepath, std::vector<std::unique_ptr<Mesh>>& pMeshes,
        GLboolean bFlipNormals)
    {
        int rFlag = -1;
        //    clock_t  startTime, endTime;

        auto startTime = std::chrono::high_resolution_clock::now();

        glm::vec3 minVec3(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 maxVec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        Mesh* pMesh = new Mesh();

        std::cout << "Loading : " << filepath << " - ";

        // open file
        std::ifstream stream(filepath);
        if (stream.fail())
        {
            std::cerr << "Can't find the file in the filepath\n";
            return rFlag;
        }

        std::string line = "";
        std::string prefix = "";
        std::stringstream ss;

        // Vertex Index
        int indices_index = 0;

        glm::vec3 positionSum(0.0f, 0.0f, 0.0f);

        while (std::getline(stream, line))
        {
            ss.clear();
            prefix.clear();
            ss.str(line);
            ss >> prefix;

            if (prefix == "v")
            {
                glm::vec3 vertex_position;
                ss >> vertex_position.x >> vertex_position.y >> vertex_position.z;

                positionSum += vertex_position;

                maxVec3.x = std::max(vertex_position.x, maxVec3.x);
                maxVec3.y = std::max(vertex_position.y, maxVec3.y);
                maxVec3.z = std::max(vertex_position.z, maxVec3.z);

                minVec3.x = std::min(vertex_position.x, minVec3.x);
                minVec3.y = std::min(vertex_position.y, minVec3.y);
                minVec3.z = std::min(vertex_position.z, minVec3.z);

                pMesh->vertexPosition.emplace_back(std::move(vertex_position));
            }
            else if (prefix == "vt")
            {
                glm::vec2 vertex_uv;
                ss >> vertex_uv.x >> vertex_uv.y;
                pMesh->vertexUVs.emplace_back(std::move(vertex_uv));
            }
            else if (prefix == "vn")
            {
                glm::vec3 vertex_normal;
                ss >> vertex_normal.x >> vertex_normal.y >> vertex_normal.z;
                pMesh->vertexNormals.emplace_back(std::move(vertex_normal));

            }
            else if (prefix == "f")
            {
                int index = 0;
                int count = 0;

                std::vector<GLuint> pos_Indices;
                std::vector<GLuint> uv_Indices;
                std::vector<GLuint> norm_Indices;

                while (ss >> index)
                {
                    if (count == 0)
                    {
                        pos_Indices.emplace_back(index - 1);
                    }
                    else if (count == 1)
                        uv_Indices.emplace_back(index - 1);
                    else if (count == 2)
                        norm_Indices.emplace_back(index - 1);

                    if (ss.peek() == '/')
                    {
                        while (ss.peek() == '/')
                        {
                            ++count;
                            ss.ignore(1, '/');
                        }
                    }
                    else if (ss.peek() == ' ' || ss.peek() == EOF)
                    {
                        count = 0;
                        ss.ignore(1, ' ');
                    }
                }

                // Insert first 3 vert in face
                pMesh->indices_.push_back(*(pos_Indices.begin()));
                pMesh->indices_.push_back(*(pos_Indices.begin() + 1));
                pMesh->indices_.push_back(*(pos_Indices.begin() + 2));

                // For face more than 3 vertices
                int vert_in_face = static_cast<int>(pos_Indices.size());

                for (int i = 0; i < vert_in_face - 3; ++i)
                {
                    pMesh->indices_.push_back(*(pos_Indices.begin()));
                    pMesh->indices_.push_back(*(pos_Indices.begin() + i + 2));
                    pMesh->indices_.push_back(*(pos_Indices.begin() + i + 3));
                }
            }
        }
        // close file
        stream.close();

        auto endTime = std::chrono::high_resolution_clock::now();

        double timeDuration = std::chrono::duration< double, std::milli >(endTime - startTime).count();

        std::cout << "OBJ file read in "
            << timeDuration
            << "  milli seconds." << std::endl;

        bool hasNormals = pMesh->vertexNormals.size();
        bool hasTextureCoords = pMesh->vertexUVs.size();

        pMesh->boundingBox[0] = minVec3;
        pMesh->boundingBox[1] = maxVec3;

        positionSum /= pMesh->vertexPosition.size();

        pMesh->calcVertexPositionForBoundingBox(positionSum);
        // Now calculate vertex normals
        pMesh->calcVertexNormals(bFlipNormals);

		pMesh->calcUVs();

        pMesh->MapVertexBuffer();
  
        pMeshes.emplace_back(std::move(pMesh));

        return timeDuration;
    }
}
