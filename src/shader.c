#include "Shader.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void GetShaderSource(char** result, const char* filepath)
{
    FILE* shaderFile = fopen(filepath, "rb");
    fseek(shaderFile, 0, SEEK_END);
    long size = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);
    char* shaderSource = (char*)malloc(size + 1);
    fread(shaderSource, size, 1, shaderFile);
    shaderSource[size] = '\0';
    fclose(shaderFile);

    *result = (char*)malloc(size + 1);
    memcpy(*result, shaderSource, size + 1);
    free(shaderSource);
}

uint32_t CreateShader(const char* vertexFilepath, const char* fragmentFilepath)
{
    int success;
    char log[512];
    const char* source;

    char* vertexSource;
    GetShaderSource(&vertexSource, vertexFilepath);
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    source = vertexSource;
    glShaderSource(vertexShader, 1, &source, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        printf("Error compiling vertex shader. %s\n", log);
    }

    char* fragmentSource;
    GetShaderSource(&fragmentSource, fragmentFilepath);
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragmentSource;
    glShaderSource(fragmentShader, 1, &source, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        printf("Error compiling fragment shader. %s\n", log);
    }

    uint32_t program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("Error linking shader. %s\n", log);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexSource);
    free(fragmentSource);

    return program;
}