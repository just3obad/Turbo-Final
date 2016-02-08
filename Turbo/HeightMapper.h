#ifndef HeightMapper_h
#define HeightMapper_h

#pragma once
#include <GL/glew.h>
#include <vector>
#include "glm.hpp"



class HeightMapper
{
public:
	HeightMapper();

	GLuint loadHeightMap(const std::string& path);
	void drawBase();
	void drawWall();
	int getVertexHeight(int x, int y);
	float getInterpolatetHeight(float x, float y);	

	~HeightMapper();
private:
	std::vector<unsigned char> _image;
	std::vector<unsigned char> _imageBorder;
	unsigned int _height, _width;
	GLuint _vao = 0;
	GLuint _vao2 = 0;
	GLuint _textureID;
	int _WallTriangles;
	int _WallIndices;

	int loadMappWall(const std::string& path);
	int iterate(std::vector<glm::vec3> &vertices, glm::vec2 startPoint, int index);
	bool getNeigbourPixel(glm::vec2 old, glm::vec2 test, glm::vec2 &out);
	inline bool isBlack(glm::vec2 pos) { return (((int)_imageBorder[(unsigned int)((pos.y*_width + pos.x) * 4)] < 50) && ((int)_imageBorder[(unsigned int)((pos.y*_width + pos.x) * 4+1)] < 50)); }

	GLuint loadImage(const std::string& path);

	int getIndicesCount();
	int getTriangleCount();
	GLuint getVAO();



};

#endif /* HeightMapper_h */