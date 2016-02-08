//
//  obj_loader.hpp
//  Turbo
//
//  Created by Hareem Mughis on 26/01/16.
//  Copyright © 2016 Deniz Kesmez. All rights reserved.
//

#ifndef obj_loader_h
#define obj_loader_h

#include <stdio.h>
#include <glm.hpp>
#include <vector>
#include <string>

struct OBJIndex
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;
    
    bool operator<(const OBJIndex& r) const { return vertexIndex < r.vertexIndex; }
};

struct material{
    std::string name;       //the name of the material
    float alpha,ns,ni;      //some property, alpha, shininess, and some other, which we not used
    float dif[3],amb[3],spec[3];    //the color property (diffuse, ambient, specular)
    int illum;      //illum - we not use it
    int texture;    //the id for the texture, if there is no texture than -1
    material(const char* na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t);
};

class IndexedModel
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    
    void CalcNormals();
};

class OBJModel
{
public:
    std::vector<OBJIndex> OBJIndices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<material*> materials;
    bool hasUVs;
    bool hasNormals;
    bool ismaterial,istexture;
    
    OBJModel(const std::string& fileName);
    
    IndexedModel ToIndexedModel();
private:
    unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result);
    void CreateOBJFace(const std::string& line);
    void ImportMaterials(const std::string& line, int i);
    void UseMaterial(const std::string& line, int i, int curmat);
    
    glm::vec2 ParseOBJVec2(const std::string& line);
    glm::vec3 ParseOBJVec3(const std::string& line);
    OBJIndex ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals);
};

#endif /* obj_loader_h */
