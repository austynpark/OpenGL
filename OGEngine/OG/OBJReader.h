/* Start Header -------------------------------------------------------
File Name: OBJReader.h
Purpose: Object Reader is made as a Singleton Instance and Called from the Model to read model (mesh)-
info from *.obj file
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef SIMPLE_SCENE_OBJREADER_H
#define SIMPLE_SCENE_OBJREADER_H
#include "Mesh.h"

namespace OG
{
    class OBJReader
    {
    protected:
        OBJReader();
    public:
        OBJReader(const OBJReader& objectReader) = delete;
        OBJReader(OBJReader& objectReader) = delete;
        void operator=(const OBJReader&) = delete;
        void operator=(OBJReader&&) = delete;

        static OBJReader* getInstance();

        virtual ~OBJReader();

        double ReadOBJFile(const std::string& filepath,
            std::unique_ptr<Mesh>& pMesh,
            GLboolean bFlipNormals = false);

    private:
        static OBJReader* objectReader_;
    };
}




#endif //SIMPLE_SCENE_OBJREADER_H
