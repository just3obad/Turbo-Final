//
//  obj_loader.cpp
//  Turbo
//
//  Created by Hareem Mughis on 26/01/16.
//  Copyright © 2016 Deniz Kesmez. All rights reserved.
//

// #include "stdafx.h"
#include "obj_loader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
using namespace std;

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);

material::material(const char* na,float al,float n,float ni2,float* d,float* a,float* s,int i,int t)
{
    name=na;
    alpha=al;
    ni=ni2;
    ns=n;
    dif[0]=d[0];
    dif[1]=d[1];
    dif[2]=d[2];
    
    amb[0]=a[0];
    amb[1]=a[1];
    amb[2]=a[2];
    
    spec[0]=s[0];
    spec[1]=s[1];
    spec[2]=s[2];
    
    illum=i;
    texture=t;
}

OBJModel::OBJModel(const std::string& fileName)
{
    hasUVs = false;
    hasNormals = false;
    ismaterial=false;
    istexture=false;
    int curmat=0;
    std::ifstream file;
    file.open(fileName.c_str(), ios::in);
    
    std::string line;
    if(file.is_open())
    {
        int i=0;
        while(file.good())
        {
            getline(file, line);
            
            unsigned int lineLength = line.length();
            
            if(lineLength < 2)
                continue;
            
            const char* lineCStr = line.c_str();
            
            switch(lineCStr[0])
            {
                case 'v':
                    if(lineCStr[1] == 't')
                        this->uvs.push_back(ParseOBJVec2(line));
                    else if(lineCStr[1] == 'n')
                        this->normals.push_back(ParseOBJVec3(line));
                    else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
                        this->vertices.push_back(ParseOBJVec3(line));
                    break;
                case 'f':
                    CreateOBJFace(line);
                    break;
                case 'm':
                    //ImportMaterials(line,i);
                    break;
                case 'u':
                    UseMaterial(line, i, curmat);
                    break;
                default: break;
            };
            i++;
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
    }
}

void IndexedModel::CalcNormals()
{
    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];
        
        glm::vec3 v1 = positions[i1] - positions[i0];
        glm::vec3 v2 = positions[i2] - positions[i0];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    
    for(unsigned int i = 0; i < positions.size(); i++)
        normals[i] = glm::normalize(normals[i]);
}

IndexedModel OBJModel::ToIndexedModel()
{
    IndexedModel result;
    IndexedModel normalModel;
    
    unsigned int numIndices = OBJIndices.size();
    
    std::vector<OBJIndex*> indexLookup;
    
    for(unsigned int i = 0; i < numIndices; i++)
        indexLookup.push_back(&OBJIndices[i]);
    
    std::sort(indexLookup.begin(), indexLookup.end(), CompareOBJIndexPtr);
    
    std::map<OBJIndex, unsigned int> normalModelIndexMap;
    std::map<unsigned int, unsigned int> indexMap;
    
    for(unsigned int i = 0; i < numIndices; i++)
    {
        OBJIndex* currentIndex = &OBJIndices[i];
        
        glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
        glm::vec2 currentTexCoord;
        glm::vec3 currentNormal;
        
        if(hasUVs)
            currentTexCoord = uvs[currentIndex->uvIndex];
        else
            currentTexCoord = glm::vec2(0,0);
        
        if(hasNormals)
            currentNormal = normals[currentIndex->normalIndex];
        else
            currentNormal = glm::vec3(0,0,0);
        
        unsigned int normalModelIndex;
        unsigned int resultModelIndex;
        
        //Create model to properly generate normals on
        std::map<OBJIndex, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
        if(it == normalModelIndexMap.end())
        {
            normalModelIndex = normalModel.positions.size();
            
            normalModelIndexMap.insert(std::pair<OBJIndex, unsigned int>(*currentIndex, normalModelIndex));
            normalModel.positions.push_back(currentPosition);
            normalModel.texCoords.push_back(currentTexCoord);
            normalModel.normals.push_back(currentNormal);
        }
        else
            normalModelIndex = it->second;
        
        //Create model which properly separates texture coordinates
        unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result);
        
        if(previousVertexLocation == (unsigned int)-1)
        {
            resultModelIndex = result.positions.size();
            
            result.positions.push_back(currentPosition);
            result.texCoords.push_back(currentTexCoord);
            result.normals.push_back(currentNormal);
        }
        else
            resultModelIndex = previousVertexLocation;
        
        normalModel.indices.push_back(normalModelIndex);
        result.indices.push_back(resultModelIndex);
        indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
    }
    
    if(!hasNormals)
    {
        normalModel.CalcNormals();
        
        for(unsigned int i = 0; i < result.positions.size(); i++)
            result.normals[i] = normalModel.normals[indexMap[i]];
    }
    
    return result;
};

unsigned int OBJModel::FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result)
{
    unsigned int start = 0;
    unsigned int end = indexLookup.size();
    unsigned int current = (end - start) / 2 + start;
    unsigned int previous = start;
    
    while(current != previous)
    {
        OBJIndex* testIndex = indexLookup[current];
        
        if(testIndex->vertexIndex == currentIndex->vertexIndex)
        {
            unsigned int countStart = current;
            
            for(unsigned int i = 0; i < current; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current - i];
                
                if(possibleIndex == currentIndex)
                    continue;
                
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                
                countStart--;
            }
            
            for(unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current + i];
                
                if(possibleIndex == currentIndex)
                    continue;
                
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                else if((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex)
                        && (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex))
                {
                    glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
                    glm::vec2 currentTexCoord;
                    glm::vec3 currentNormal;
                    
                    if(hasUVs)
                        currentTexCoord = uvs[currentIndex->uvIndex];
                    else
                        currentTexCoord = glm::vec2(0,0);
                    
                    if(hasNormals)
                        currentNormal = normals[currentIndex->normalIndex];
                    else
                        currentNormal = glm::vec3(0,0,0);
                    
                    for(unsigned int j = 0; j < result.positions.size(); j++)
                    {
                        if(currentPosition == result.positions[j]
                           && ((!hasUVs || currentTexCoord == result.texCoords[j])
                               && (!hasNormals || currentNormal == result.normals[j])))
                        {
                            return j;
                        }
                    }
                }
            }
            
            return -1;
        }
        else
        {
            if(testIndex->vertexIndex < currentIndex->vertexIndex)
                start = current;
            else
                end = current;
        }
        
        previous = current;
        current = (end - start) / 2 + start;
    }
    
    return -1;
}

void OBJModel::CreateOBJFace(const std::string& line)
{
    std::vector<std::string> tokens = SplitString(line, ' ');
    
    this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));
    this->OBJIndices.push_back(ParseOBJIndex(tokens[2], &this->hasUVs, &this->hasNormals));
    this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));
    
    if((int)tokens.size() > 4)
    {
        this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));
        this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));
        this->OBJIndices.push_back(ParseOBJIndex(tokens[4], &this->hasUVs, &this->hasNormals));
    }
}

void OBJModel::UseMaterial(const std::string& line, int i, int curmat)
{
    /*char tmp[200];
    sscanf_s(line.c_str(),"usemtl %s",tmp);      //read the name of the material to tmp
    for(int i=0;i<materials.size();i++)     //go through all of the materials
    {
        if(strcmp(materials[i]->name.c_str(),tmp)==0)   //and compare the tmp with the name of the material
        {
            curmat=i;       //if it's equal then set the current material to that
            break;
        }
    }*/
}

void OBJModel::ImportMaterials(const std::string& line, int i)
{
    char *filen=NULL;
	string add = "./objects/";
    //string add = "./";
    sscanf_s(line.c_str(),"mtllib %s",filen);    //read the filename
    add.append(filen);
    std::ifstream mtlin(add);     //open the file
    if(!mtlin.is_open())    //if not opened error message, clean all memory, return with -1
    {
        std::cout << "connot open the material file" << std::endl;
    }
    else
    {
        ismaterial=true;        //we use materials
        std::vector<std::string> tmp;//contain all of the line of the file
        char c[200];
        while(!mtlin.eof())
        {
            mtlin.getline(c,200);   //read all lines to tmp
            tmp.push_back(c);
        }
        char name[200]; //name of the material
        char filename[200];     //filename of the texture
        float amb[3],dif[3],spec[3],alpha,ns,ni;        //colors, shininess, and something else
        int illum;
        unsigned int texture = 0;
        bool ismat=false;       //do we already have a material read in to these variables?
        strcpy_s(filename,"\0");  //set filename to nullbyte character
        for(int i=0;i<tmp.size();i++) //go through all lines of the mtllib file
        {
            if(tmp[i][0]=='#')      //we don't care about comments
                continue;
            if(tmp[i][0]=='n' && tmp[i][1]=='e' && tmp[i][2]=='w')  //new material
            {
                if(ismat)       //if we have a material
                {
                    if(strcmp(filename,"\0")!=0)    //if we have a texture
                    {
                        materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,texture)); //push back
                        strcpy_s(filename,"\0");
                    }else{
                        materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));              //push back, but use -1 to texture
                    }
                }
                ismat=false;    //we start from a fresh material
                sscanf_s(tmp[i].c_str(),"newmtl %s",name);        //read in the name
            }else if(tmp[i][0]=='N' && tmp[i][1]=='s')      //the shininess
            {
                sscanf_s(tmp[i].c_str(),"Ns %f",&ns);
                ismat=true;
            }else if(tmp[i][0]=='K' && tmp[i][1]=='a')      //the ambient
            {
                sscanf_s(tmp[i].c_str(),"Ka %f %f %f",&amb[0],&amb[1],&amb[2]);
                ismat=true;
            }else if(tmp[i][0]=='K' && tmp[i][1]=='d')      //the diffuse
            {
                sscanf_s(tmp[i].c_str(),"Kd %f %f %f",&dif[0],&dif[1],&dif[2]);
                ismat=true;
            }else if(tmp[i][0]=='K' && tmp[i][1]=='s')      //the specular
            {
				sscanf_s(tmp[i].c_str(), "Ks %f %f %f", &spec[0], &spec[1], &spec[2]);
                ismat=true;
            }else if(tmp[i][0]=='N' && tmp[i][1]=='i')      //the I don't know what is this
            {
				sscanf_s(tmp[i].c_str(), "Ni %f", &ni);
                ismat=true;
            }else if(tmp[i][0]=='d' && tmp[i][1]==' ')      //the alpha
            {
				sscanf_s(tmp[i].c_str(), "d %f", &alpha);
                ismat=true;
            }else if(tmp[i][0]=='i' && tmp[i][1]=='l')      //the illum (don't ask)
            {
				sscanf_s(tmp[i].c_str(), "illum %d", &illum);
                ismat=true;
            }/*else if(tmp[i][0]=='m' && tmp[i][1]=='a')      //and the texture
            {
                sscanf(tmp[i].c_str(),"map_Kd %s",filename);
                texture=loadTexture(filename);  //read the filename, and use the loadTexture function to load it, and get the id.
                ismat=true;
            }*/
        }
        if(ismat)       //there is no newmat after the last newmat, so we have to put the last material 'manually'
        {
            if(strcmp(filename,"\0")!=0)
            {
                materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,texture));
            }else{
                materials.push_back(new material(name,alpha,ns,ni,dif,amb,spec,illum,-1));
            }
        }
    }
}


OBJIndex OBJModel::ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals)
{
    unsigned int tokenLength = token.length();
    const char* tokenString = token.c_str();
    
    unsigned int vertIndexStart = 0;
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    OBJIndex result;
    result.vertexIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    result.uvIndex = 0;
    result.normalIndex = 0;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/s');
    
    result.uvIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    //*hasUVs = true;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    result.normalIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    *hasNormals = true;
    
    return result;
}

glm::vec3 OBJModel::ParseOBJVec3(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 2;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec3(x,y,z);
    
    //glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
}

glm::vec2 OBJModel::ParseOBJVec2(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 3;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
	//added new
	hasUVs = true;

    return glm::vec2(x,y);
}

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b)
{
    return a->vertexIndex < b->vertexIndex;
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while(result < length)
    {
        result++;
        if(str[result] == token)
            break;
    }
    
    return result;
}

static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    
    const char* cstr = s.c_str();
    unsigned int strLength = s.length();
    unsigned int start = 0;
    unsigned int end = 0;
    
    while(end <= strLength)
    {
        while(end <= strLength)
        {
            if(cstr[end] == delim)
                break;
            end++;
        }
        
        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }
    
    return elems;
}