//
//  ShaderProgram.hpp
//  Turbo
//
//  Created by Hareem Mughis on 16/01/16.
//  Copyright © 2016 Deniz Kesmez. All rights reserved.
//

#ifndef ShaderProgram_hpp
#define ShaderProgram_hpp

#include <string>
#include <GL/glew.h>

class Shader
{
public:
    Shader(const std::string& fileName);   
	GLuint getUniform(int i);
    void Bind();
    virtual ~Shader();
protected:
private:
    static const unsigned int NUM_SHADERS = 2;
    static const unsigned int NUM_UNIFORMS = 3;
    void operator=(const Shader& shader) {}
    Shader(const Shader& shader) {}
    
    std::string LoadShader(const std::string& fileName);
    void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
    GLuint CreateShader(const std::string& text, unsigned int type);
    
    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
	GLuint m_uniforms[NUM_UNIFORMS];
};

#endif /* ShaderProgram_hpp */
