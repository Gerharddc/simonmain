#ifndef LOADEDGL_H
#define LOADEDGL_H

#include <QOpenGLFunctions>

extern void ActivateGL();
extern void DeactivateGL();

extern GLuint glCreateShader(GLenum shaderType);
extern void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
extern void glCompileShader(GLuint shader);
extern void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
extern void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
extern GLuint glCreateProgram();
extern void glDeleteProgram(GLuint program);
extern void glDeleteShader(GLuint shader);
extern void glAttachShader(GLuint program, GLuint shader);
extern void glLinkProgram(GLuint program);
extern void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
extern void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei *length, char *infoLog);

#endif // LOADEDGL_H
