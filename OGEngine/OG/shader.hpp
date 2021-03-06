/* Start Header -------------------------------------------------------
File Name: shader.hpp
Purpose: Read glsl file ,compile and link loaded shaders into program
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>

namespace OG 
{
    class Shader
    {
        std::string vert_file_path;
        std::string frag_file_path;
    public:
        Shader() = default;
        Shader(const std::string vertex_file_path, const std::string fragment_file_path);

        GLuint programID;
        GLuint pipeLineID;

        enum Primitive_Enum
        {
            TriangleStrip = 0,
            Points,
            LineStrip
        };
        ///////////////////
        void LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

        void ReloadShaders();

        ///////////////////
        void LoadPipeline(const char* vertex_file_path, const char* fragment_file_path, GLuint* programIDs);

        // Load shaders where multiple shader files == one complete shader
        // Show how code can be reused across shaders
        void LoadMultiShaders(const char* vertex_file_path, const char* fragment_file_path,
            const char* geom_file_path, Primitive_Enum  out_primitive_type);

        // use/activate the shader
        void Use();
        // clean up/ deactivate the shader
        void CleanUp();

        // utility uniform functions
        void SetUniform1b(const std::string& name, GLboolean value) const;
        void SetUniform1i(const std::string& name, GLint value) const;
        void SetUniform1ui(const std::string& name, GLuint value) const;
        void SetUniform1f(const std::string& name, GLfloat value) const;
        void SetUniform1d(const std::string& name, const GLdouble value) const;
        void SetUniform3fv(const std::string& name, const glm::vec3& value) const;
        void SetUniform3f(const std::string& name, GLfloat x, GLfloat y, GLfloat z) const;
        void SetUniform4fv(const std::string& name, const glm::vec4& value) const;
        void SetUniform4f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
        void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat) const;
        void SetUniformMatrix3fv(const std::string& name, const glm::mat3& mat) const;
        void SetUniformBlock(const std::string& name, GLuint blockIndex);

        ~Shader();
    };
}




#endif
