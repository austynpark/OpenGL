/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: shader.cpp
Purpose: Read glsl file ,compile and link loaded shaders into program
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "shader.hpp"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace OG 
{
    void Shader::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
    {

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if (VertexShaderStream.is_open()) {
            std::string Line;
            while (getline(VertexShaderStream, Line))
                VertexShaderCode += "\n" + Line;
            VertexShaderStream.close();
        }
        else {
            printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
            getchar();
            return;
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if (FragmentShaderStream.is_open()) {
            std::string Line;
            while (getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
            FragmentShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int InfoLogLength;

        
        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertex_file_path);
        char const* VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }


        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragment_file_path);
        char const* FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }


        // Link the program
        printf("Linking program\n");
        programID = glCreateProgram();
        glAttachShader(programID, VertexShaderID);
        glAttachShader(programID, FragmentShaderID);
        glLinkProgram(programID);

        // Check the program
        glGetProgramiv(programID, GL_LINK_STATUS, &Result);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(programID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }


        //glDetachShader(programID, VertexShaderID);
        //glDetachShader(programID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
    }

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////

    void Shader::LoadPipeline(const char* vertex_file_path, const char* fragment_file_path, GLuint* programIDs)
    {
        // Hardcoding the values to 2

        // Vertex Shader + Program
        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if (VertexShaderStream.is_open()) {
            std::string Line;
            while (getline(VertexShaderStream, Line))
                VertexShaderCode += "\n" + Line;
            VertexShaderStream.close();
        }
        else {
            printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
            getchar();
            return;
        }

        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertex_file_path);
        char const* VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        programIDs[0] = glCreateProgram();
        glAttachShader(programIDs[0], VertexShaderID);

        // IMP: Do this BEFORE linking the progam
        glProgramParameteri(programIDs[0],
            GL_PROGRAM_SEPARABLE,
            GL_TRUE);

        glLinkProgram(programIDs[0]);

        // Check the program
        glGetProgramiv(programIDs[0], GL_LINK_STATUS, &Result);
        glGetProgramiv(programIDs[0], GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(programIDs[0], InfoLogLength, nullptr, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }





        // Fragment Shader + Program

        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if (FragmentShaderStream.is_open()) {
            std::string Line;
            while (getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
            FragmentShaderStream.close();
        }

        Result = GL_FALSE;
        InfoLogLength = 0;

        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragment_file_path);
        char const* FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        programIDs[1] = glCreateProgram();
        glAttachShader(programIDs[1], FragmentShaderID);

        // IMP: Do this BEFORE linking the progam
        glProgramParameteri(programIDs[1],
            GL_PROGRAM_SEPARABLE,
            GL_TRUE);

        glLinkProgram(programIDs[1]);

        glGenProgramPipelines(1, &pipeLineID);

        glUseProgramStages(pipeLineID, GL_VERTEX_SHADER_BIT, programIDs[0]);
        glUseProgramStages(pipeLineID, GL_FRAGMENT_SHADER_BIT, programIDs[1]);
    }

    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////

    void Shader::LoadMultiShaders(const char* vertex_file_path, const char* fragment_file_path,
        const char* geom_file_path,
        Primitive_Enum out_primitive_type)
    {
        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;

        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if (VertexShaderStream.is_open())
        {
            std::string Line;
            while (getline(VertexShaderStream, Line))
                VertexShaderCode += "\n" + Line;
            VertexShaderStream.close();
        }
        else
        {
            printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n",
                vertex_file_path);
            getchar();
            return;
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if (FragmentShaderStream.is_open())
        {
            std::string Line;
            while (getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
            FragmentShaderStream.close();
        }


        // array of char * to hold the strings
        std::vector<const char*> shaderStrings;
        shaderStrings.clear();

        // First string
        std::string geometryShaderHeader("#version 450 core\n"
            "\n"
            "const int maxVerts = 4;");
        shaderStrings.push_back(geometryShaderHeader.c_str());

        // Second string
        std::string geometryInOut = "// Type of primitive input\n  // from VS (CPU)\n layout (points) in;";

        std::string temp;
        switch (out_primitive_type)
        {
        case TriangleStrip:
            temp = "triangle_strip";
            break;

        case LineStrip:
            temp = "line_strip";
            break;

        case Points:
            temp = "points";
            break;
        };

        geometryInOut += "layout (" + temp + ", max_vertices = maxVerts) out;\n";
        shaderStrings.push_back(geometryInOut.c_str());

        // Third string
        // Read the Geometry Shader from the file
        std::string GeometryShaderCode;
        std::ifstream GeometryShaderStream(geom_file_path, std::ios::in);
        if (GeometryShaderStream.is_open())
        {
            std::string Line;
            while (getline(GeometryShaderStream, Line))
                GeometryShaderCode += "\n" + Line;
            GeometryShaderStream.close();
        }

        shaderStrings.push_back(GeometryShaderCode.c_str());
        //////////////////////////////////////////////////////////


        GLint Result = GL_FALSE;
        int InfoLogLength;


        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertex_file_path);
        char const* VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }


        // Compile Geometry Shader
        printf("Compiling shader : %s\n", geom_file_path);
        char const** GeometrySourcePointer = shaderStrings.data(); //GeometryShaderCode.c_str();
        glShaderSource(GeometryShaderID, 3, GeometrySourcePointer, nullptr);
        glCompileShader(GeometryShaderID);

        // Check Fragment Shader
        glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> GeometryShaderErrorMessage((unsigned long)(InfoLogLength + 1));
            glGetShaderInfoLog(GeometryShaderID, InfoLogLength, nullptr, &GeometryShaderErrorMessage[0]);
            printf("%s\n", &GeometryShaderErrorMessage[0]);
        }


        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragment_file_path);
        char const* FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        programID = glCreateProgram();
        glAttachShader(programID, VertexShaderID);
        glAttachShader(programID, GeometryShaderID);
        glAttachShader(programID, FragmentShaderID);
        glLinkProgram(programID);

        // Check the program
        glGetProgramiv(programID, GL_LINK_STATUS, &Result);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(static_cast<unsigned long>(InfoLogLength + 1));
            glGetProgramInfoLog(programID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }


        glDetachShader(programID, VertexShaderID);
        glDetachShader(programID, GeometryShaderID);
        glDetachShader(programID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(GeometryShaderID);
        glDeleteShader(FragmentShaderID);
    }
    void Shader::Use()
    {
        glUseProgram(programID);
    }
    void Shader::CleanUp()
    {
        glDeleteProgram(programID);
    }
    void Shader::SetUniform1b(const std::string& name, GLboolean value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform1i(location, value);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform1i(const std::string& name, GLint value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform1i(location, value);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform1ui(const std::string& name, GLuint value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform1ui(location, value);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform1f(const std::string& name, GLfloat value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform1f(location, value);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform1d(const std::string& name, const GLdouble value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform1d(location, value);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform3fv(const std::string& name, const glm::vec3& value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform3fv(location, 1, glm::value_ptr(value));
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform3f(const std::string& name, GLfloat x, GLfloat y, GLfloat z) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform3f(location, x, y, z);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform4fv(const std::string& name, const glm::vec4& value) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform4fv(location, 1, glm::value_ptr(value));
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniform4f(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniform4f(location, x, y, z, w);
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }
    void Shader::SetUniformMatrix3fv(const std::string& name, const glm::mat3& mat) const
    {
        GLint location = glGetUniformLocation(programID, name.c_str());

        if (location >= 0)
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        else
            std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
    }

    void Shader::SetUniformBlock(const std::string& name, GLuint blockIndex)
    {
        GLuint location = glGetUniformBlockIndex(programID, name.c_str());

        if (location != GL_INVALID_VALUE)
            glUniformBlockBinding(programID, location, blockIndex);
        else
            std::cout << "Uniform block " << name << " doesn't exist" << std::endl;
    }
}


