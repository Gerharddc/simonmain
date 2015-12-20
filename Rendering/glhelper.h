#ifndef GLHELPER
#define GLHELPER

#ifdef GLES
#include <GLES2/gl2.h>
#else
//#include <GL/gl.h>
//#include <GL/glext.h>
#include "loadedgl.h"
#endif

#include <exception>

#ifdef QT_APPLICATION
#include <QFile>
#include <QString>
#include <QTextStream>
#define String QString
#else
#include <fstream>
#include <string>
#define String std::string
#endif

namespace GLHelper
{
    std::string ReadEntireFile(const String &path)
    {
#ifdef QT_APPLICATION
        QFile f(path);
        if (!f.open(QFile::ReadOnly))
            throw std::runtime_error("Could not open file: " + path.toStdString());

        QTextStream in(&f);
        return in.readAll().toStdString();
#else
        std::ifstream in(path, std::ios::in);

        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        else
            throw(errno); // TODO: make this better
#endif
    }

    GLuint CompileShader(GLenum type, const std::string &source)
    {
        GLuint shader = glCreateShader(type);

        const char *sourceArray = source.c_str();
        glShaderSource(shader, 1, &sourceArray, NULL);
        glCompileShader(shader);

        GLint compileResult;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

        if (compileResult == 0)
        {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar infoLog[infoLogLength];
            glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

            throw std::runtime_error("Shader compliation failed: " + std::string(infoLog));
            return -1;
        }

        return shader;
    }

    GLuint CompileProgram(const std::string &vsSource, const std::string &fsSource)
    {
        GLuint program = glCreateProgram();

        if (program == 0)
        {
            throw std::runtime_error("GL program creation failed.");
            return -1;
        }

        GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSource);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSource);

        if (vs == 0 || fs == 0)
        {
            glDeleteShader(fs);
            glDeleteShader(vs);
            glDeleteProgram(program);
            return 0;
        }

        glAttachShader(program, vs);
        glDeleteShader(vs);

        glAttachShader(program, fs);
        glDeleteShader(fs);

        glLinkProgram(program);

        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus == 0)
        {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar infoLog[infoLogLength];
            glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);

            throw std::runtime_error("Program link failed: " + std::string(infoLog));
            return -1;
        }

        return program;
    }

    GLuint CompileProgramFromFile(const String &vsFile, const String &fsFile)
    {
        std::string vs = ReadEntireFile(vsFile);
        std::string fs = ReadEntireFile(fsFile);
        return CompileProgram(vs, fs);
    }
}

#endif // GLHELPER
