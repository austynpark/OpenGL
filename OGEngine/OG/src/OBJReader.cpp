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
            objectReader_= new OBJReader();
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
                pMesh->vertexPosIndices.push_back(*(pos_Indices.begin()));
                pMesh->vertexPosIndices.push_back(*(pos_Indices.begin() + 1));
                pMesh->vertexPosIndices.push_back(*(pos_Indices.begin() + 2));

                if (!norm_Indices.empty()) {
                    pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin()));
                    pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin() + 1));
                    pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin() + 2));
                }

                if (!uv_Indices.empty()) {
                    pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin()));
                    pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin() + 1));
                    pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin() + 2));
                }

                pMesh->indices_.push_back(indices_index++);
                pMesh->indices_.push_back(indices_index++);
                pMesh->indices_.push_back(indices_index++);

                // For face more than 3 vertices
                int vert_in_face = static_cast<int>(pos_Indices.size());

                for (int i = 0; i < vert_in_face - 3; ++i) 
                {
                    pMesh->vertexPosIndices.push_back(*(pos_Indices.begin()));
                    pMesh->vertexPosIndices.push_back(*(pos_Indices.begin() + i + 2));
                    pMesh->vertexPosIndices.push_back(*(pos_Indices.begin() + i + 3));

                    if (!norm_Indices.empty()) {
                        pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin()));
                        pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin() + i + 2));
                        pMesh->vertexNormalsIndices.push_back(*(norm_Indices.begin() + i + 3));
                    }

                    if (!uv_Indices.empty()) {
                        pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin()));
                        pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin() + i + 2));
                        pMesh->vertexUVsIndices.push_back(*(uv_Indices.begin() + i + 3));
                    }

                    pMesh->indices_.push_back(indices_index++);
                    pMesh->indices_.push_back(indices_index++);
                    pMesh->indices_.push_back(indices_index++);
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

        bool hasNormals = pMesh->vertexNormalsIndices.size();
        bool hasTextureCoords = pMesh->vertexUVsIndices.size();

        pMesh->boundingBox[0] = minVec3;
        pMesh->boundingBox[1] = maxVec3;

        pMesh->calcVertexPositionForBoundingBox();
        // Now calculate vertex normals
        pMesh->calcVertexNormals(bFlipNormals);
        pMesh->calcUVs(Mesh::UVType::CYLINDRICAL_UV);

        for (int i = 0; i < indices_index; ++i)
        {
            Vertex vertex;
            vertex.vertex_position = pMesh->vertexPosition.at(pMesh->vertexPosIndices[i]);
            vertex.vertex_normal = pMesh->vertexNormals.at(pMesh->vertexPosIndices[i]);
            vertex.vertex_uv = pMesh->vertexUVs.at(pMesh->vertexPosIndices[i]);

            pMesh->vertexBuffer_.emplace_back(std::move(vertex));
        }

        pMeshes.emplace_back(std::move(pMesh));
   
        return timeDuration;
    }


    /////////////////////////////////////////////
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    // Read the OBJ file in blocks -- works for files smaller than 1GB
    //int OBJReader::ReadOBJFile_BlockIO(std::string filepath)
    //{
    //    int rFlag = -1;
    //    long int OneGBinBytes = 1024 * 1024 * 1024 * sizeof(char);

    //    glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    //    glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);


    //    // Check the file size, if > 1 GB, abort
    //    std::ifstream inFile(filepath, std::ifstream::in | std::ifstream::binary);

    //    if (inFile.bad() || inFile.eof())
    //        return rFlag;

    //    char* fileContents = NULL;
    //    long int count = 0;

    //    // get the file size
    //    inFile.seekg(0, std::ifstream::end);
    //    count = inFile.tellg();
    //    inFile.seekg(0, std::ifstream::beg);

    //    if (count <= 0 || count >= OneGBinBytes)
    //    {
    //        std::cout << " Error reading file " << filepath << std::endl;
    //        std::cout << "File size reported as : " << count << " bytes." << std::endl;
    //    }
    //    else if (count > 0)
    //    {
    //        const char* delims = "\n\r";
    //        fileContents = (char*)malloc(sizeof(char) * (count + 1));
    //        inFile.read(fileContents, count);
    //        fileContents[count] = '\0';

    //        rFlag = 0;

    //        // Now parse the obj file
    //        char* currPtr = fileContents;
    //        char* token = strpbrk(currPtr, delims);

    //        while (token != nullptr)
    //        {
    //            int numChars = token - currPtr;
    //            char ObjLine[256];
    //            strncpy(ObjLine, currPtr, numChars);
    //            ObjLine[numChars] = '\0';

    //            ParseOBJRecord(ObjLine, min, max);

    //            currPtr = token + 1;
    //            token = strpbrk(currPtr, delims);
    //        }

    //        free(fileContents);

    //        _currentMesh->boundingBox[0] = min;
    //        _currentMesh->boundingBox[1] = max;
    //    }

    //    return rFlag;
    //}

    /////////////////////////////////////////////
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    //void OBJReader::ParseOBJRecord(char* buffer, glm::vec3& min, glm::vec3& max)
    //{
    //    const char* delims = " \r\n\t";
    //    GLfloat x, y, z;

    //    GLfloat temp;
    //    GLuint firstIndex, secondIndex, thirdIndex;

    //    char* token = strtok(buffer, delims);

    //    // account for empty lines
    //    if (token == nullptr)
    //        return;

    //    switch (token[0])
    //    {
    //    case 'v':
    //        // vertex coordinates
    //        if (token[1] == '\0')
    //        {
    //            token = strtok(nullptr, delims);
    //            temp = static_cast<GLfloat&&>(atof(token));
    //            if (min.x > temp)
    //                min.x = temp;
    //            if (max.x <= temp)
    //                max.x = temp;
    //            x = temp;

    //            token = strtok(nullptr, delims);
    //            temp = static_cast<GLfloat&&>(atof(token));
    //            if (min.y > temp)
    //                min.y = temp;
    //            if (max.y <= temp)
    //                max.y = temp;
    //            y = temp;

    //            token = strtok(nullptr, delims);
    //            temp = static_cast<GLfloat&&>(atof(token));
    //            if (min.z > temp)
    //                min.z = temp;
    //            if (max.z <= temp)
    //                max.z = temp;
    //            z = temp;

    //            _currentMesh->vertexBuffer.emplace_back(x, y, z);
    //        }
    //        // vertex normals
    //        else if (token[1] == 'n')
    //        {
    //            glm::vec3 vNormal;

    //            token = strtok(nullptr, delims);
    //            if (token == nullptr)
    //                break;

    //            vNormal[0] = static_cast<GLfloat&&>(atof(token));

    //            token = strtok(nullptr, delims);
    //            if (token == nullptr)
    //                break;

    //            vNormal[1] = static_cast<GLfloat&&>(atof(token));

    //            token = strtok(nullptr, delims);
    //            if (token == nullptr)
    //                break;

    //            vNormal[2] = static_cast<GLfloat&&>(atof(token));

    //            _currentMesh->vertexNormals.push_back(glm::normalize(vNormal));
    //        }

    //        break;

    //    case 'f':
    //        token = strtok(nullptr, delims);
    //        if (token == nullptr)
    //            break;
    //        firstIndex = static_cast<unsigned int&&>(atoi(token) - 1);

    //        token = strtok(nullptr, delims);
    //        if (token == nullptr)
    //            break;
    //        secondIndex = static_cast<unsigned int&&>(atoi(token) - 1);

    //        token = strtok(nullptr, delims);
    //        if (token == nullptr)
    //            break;
    //        thirdIndex = static_cast<unsigned int&&>(atoi(token) - 1);

    //        // push back first triangle
    //        _currentMesh->vertexIndices.push_back(firstIndex);
    //        _currentMesh->vertexIndices.push_back(secondIndex);
    //        _currentMesh->vertexIndices.push_back(thirdIndex);

    //        token = strtok(nullptr, delims);

    //        while (token != nullptr)
    //        {
    //            secondIndex = thirdIndex;
    //            thirdIndex = static_cast<unsigned int&&>(atoi(token) - 1);

    //            _currentMesh->vertexIndices.push_back(firstIndex);
    //            _currentMesh->vertexIndices.push_back(secondIndex);
    //            _currentMesh->vertexIndices.push_back(thirdIndex);

    //            token = strtok(nullptr, delims);
    //        }

    //        break;

    //    case '#':
    //    default:
    //        break;
    //    }

    //    return;
    //}
}

