#include "loadedgl.h"
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <exception>

QOpenGLFunctions *glFuncs = NULL;

void ActivateGL()
{
    glFuncs = QOpenGLContext::currentContext()->functions();
}

void DeactivateGL()
{
    glFuncs = NULL;
}

inline void ThrowInactive()
{
    if (glFuncs == NULL)
        throw std::runtime_error("The gl context has not been activated.");
}

GLuint glCreateShader(GLenum shaderType)
{
    ThrowInactive();
    return glFuncs->glCreateShader(shaderType);
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
    ThrowInactive();
    glFuncs->glShaderSource(shader, count, string, length);
}

void glCompileShader(GLuint shader)
{
    ThrowInactive();
    glFuncs->glCompileShader(shader);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    ThrowInactive();
    glFuncs->glGetShaderiv(shader, pname, params);
}

void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    ThrowInactive();
    glFuncs->glGetShaderInfoLog(shader, maxLength, length, infoLog);
}

GLuint glCreateProgram()
{
    ThrowInactive();
    return glFuncs->glCreateProgram();
}

void glDeleteProgram(GLuint program)
{
    ThrowInactive();
    glFuncs->glDeleteProgram(program);
}

void glDeleteShader(GLuint shader)
{
    ThrowInactive();
    glFuncs->glDeleteShader(shader);
}

void glAttachShader(GLuint program, GLuint shader)
{
    ThrowInactive();
    glFuncs->glAttachShader(program, shader);
}

void glLinkProgram(GLuint program)
{
    ThrowInactive();
    glFuncs->glLinkProgram(program);
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    ThrowInactive();
    glFuncs->glGetProgramiv(program, pname, params);
}

void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei *length, char *infoLog)
{
    ThrowInactive();
    glFuncs->glGetProgramInfoLog(program, bufsize, length, infoLog);
}
