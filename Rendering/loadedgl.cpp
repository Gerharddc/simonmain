#ifndef GLES

#include "loadedgl.h"
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <stdexcept>

QOpenGLFunctions *glFuncs = NULL;

void LoadedGL::ActivateGL()
{
    glFuncs = QOpenGLContext::currentContext()->functions();
}

void LoadedGL::DeactivateGL()
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

void glDeleteBuffers(GLsizei n, const GLuint *buffers)
{
    ThrowInactive();
    glFuncs->glDeleteBuffers(n, buffers);
}

GLint glGetAttribLocation(GLuint program, const char *name)
{
    ThrowInactive();
    return glFuncs->glGetAttribLocation(program, name);
}

GLint glGetUniformLocation(GLuint program, const char *name)
{
    ThrowInactive();
    return glFuncs->glGetUniformLocation(program, name);
}

void glGenBuffers(GLsizei n, GLuint *buffers)
{
    ThrowInactive();
    glFuncs->glGenBuffers(n, buffers);
}

void glBindBuffer(GLenum target, GLuint buffer)
{
    ThrowInactive();
    glFuncs->glBindBuffer(target, buffer);
}

void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    ThrowInactive();
    glFuncs->glBufferData(target, size, data, usage);
}

void glUseProgram(GLuint program)
{
    ThrowInactive();
    glFuncs->glUseProgram(program);
}

void glEnableVertexAttribArray(GLuint index)
{
    ThrowInactive();
    glFuncs->glEnableVertexAttribArray(index);
}

void glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *ptr)
{
    ThrowInactive();
    glFuncs->glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    ThrowInactive();
    glFuncs->glUniformMatrix4fv(location, count, transpose, value);
}

#endif //GLES
