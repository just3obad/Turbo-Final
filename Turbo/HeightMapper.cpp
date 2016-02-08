#include "stdafx.h"
#include "HeightMapper.h"

#include <iostream>
#include <math.h>
#include "lodepng.h"
#include "gtc/type_ptr.hpp"


#define WALLHEIGHT 3

std::vector<glm::vec2> testvector{ glm::vec2(0, -1), glm::vec2(1, -1), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1), glm::vec2(-1, 1), glm::vec2(-1, 0), glm::vec2(-1, -1) };

HeightMapper::HeightMapper()
{
}


HeightMapper::~HeightMapper()
{
}

GLuint HeightMapper::loadHeightMap(const std::string& path)
{
	//char * data;
	//float* vertices;
	unsigned int * indices;


	// load image

	//decode
	unsigned error = lodepng::decode(_image, _width, _height, path + ".png");

	//if there's an error, display it
	if (error){
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return -1;
	}

	//generate vertices, indices, normals
	int n = _width*_height;
	std::vector<glm::vec3> vertices(n);
		
	int m = (_height - 1) * 2 * _width + (_height - 2) * 2;//(height-1) * 2*width + (height-2)*2 => (height-1)*(width-1)*2 + (height-2)*4 triagles
	indices = (unsigned int*)malloc(sizeof(unsigned int)*m);

	std::vector<glm::vec3> normals(n);

	//fill vertices
	for (unsigned int j = 0; j < _height; j++)
	{
		for (unsigned int i = 0; i < _width; i++)
		{
			vertices[j*_width + i] = glm::vec3(i, _image[(j*_width + i) * 4], j);
		}
	}

	//fill indices
	int k = 0;
	for (unsigned int j = 0; j < _height - 1; j++)
	{
		for (unsigned int i = 0; i < _width; i++)
		{
			indices[k++] = j*_width + i;
			indices[k++] = (j + 1)*_width + i;
		}
		indices[k++] = (j + 1)*_width + (_width - 1);
		indices[k++] = (j + 1)*_width + 0;
	}

	//calculate normals
	glm::vec3 normal;

	for (unsigned int i = 0; i < _height; i++)
	{
		for (unsigned int j = 0; j < _width; j++)
		{
			//generate vertices[i*width+j]'s normals
			normal = glm::vec3(0, 0, 0);

			if (i > 0) //1,2,3
			{
				if (j > 0) //1
				{
					normal += glm::cross(vertices[(i - 1)*_width + j] - vertices[i*_width + j], vertices[i*_width + (j - 1)] - vertices[i*_width + j]);
				}
				if (j < _width - 1)//2,3
				{
					normal += glm::cross(vertices[(i - 1)*_width + (j + 1)] - vertices[i*_width + j], vertices[(i - 1)*_width + j] - vertices[i*_width + j]);
					normal += glm::cross(vertices[i*_width + (j + 1)] - vertices[i*_width + j], vertices[(i - 1)*_width + (j + 1)] - vertices[i*_width + j]);
				}

			}
			if (i < _height - 1)//4,5,6
			{
				if (j > 0) //4,5
				{
					normal += glm::cross(vertices[i*_width + (j - 1)] - vertices[i*_width + j], vertices[(i + 1)*_width + (j - 1)] - vertices[i*_width + j]);
					normal += glm::cross(vertices[(i + 1)*_width + (j - 1)] - vertices[i*_width + j], vertices[(i + 1)*_width + j] - vertices[i*_width + j]);
				}
				if (j < _width - 1)//6
				{
					normal += glm::cross(vertices[i*_width + (j + 1)] - vertices[i*_width + j], vertices[(i + 1)*_width + j] - vertices[i*_width + j]);
				}
			}
			//normalise
			normals[i*_width + j] = glm::normalize(normal);
		}
	}

	//vertex attribute object (VAO)
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*n * 3, &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//normal buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//index Buffer
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*m, indices, GL_STATIC_DRAW);

	loadMappWall(path);

	return _vao;
}


int HeightMapper::loadMappWall(const std::string& path)
{
	unsigned int height, width;
	// load image

	//decode
	unsigned error = lodepng::decode(_imageBorder, width, height, path + "-B.png");

	//if there's an error, display it
	if (error){
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return -1;
	}

	if (width != _width || height != _height)
		return -1;

	//get black points
	//get start points
	int n = 0;
	int nStartPoints = 0;
	std::vector<glm::vec2> starts(5);

	for (unsigned int i = 0; i < _height*_width; i++)
	{
		//start Points (green  = 128)
		if ((int)_imageBorder[i * 4 + 1] == 128)
		{
			if (nStartPoints == 5)
			{
				std::cout << "error: to many walls in Map, only support up to 5";
				return -1;				
			}
			starts[nStartPoints] = glm::vec2(i - (int)(i / _width)*_width, i / _width);
			nStartPoints++;
		}

		// # wall Points (red, green < 50)
		//if (_imageBorder[i * 4] > 200)
		if (isBlack(glm::vec2(i - (int)(i / _width)*_width, i / _width)))
		{
			n++;
		}
		
		//error pixels
		//if (!(((int)_imageBorder[(unsigned int)(((i / _width)*_width + (i - (int)(i / _width)*_width)) * 4)] == 255) || ((int)_imageBorder[(unsigned int)(((i / _width)*_width + (i - (int)(i / _width)*_width)) * 4)] == 0)))
		//	std::cout << i - (int)(i / _width)*_width << " : " << i / _width << std::endl;
		
	}
	std::cout << "Black Points found: " << n << std::endl;
	std::cout << "Green Points found: " << nStartPoints << std::endl;

	_WallTriangles = 2 * (n + nStartPoints);
	_WallIndices = _WallTriangles+2*(nStartPoints);

	//vertices
	std::vector<glm::vec3> vertices(_WallTriangles);

	//indices
	unsigned int * indices;
	indices = (unsigned int*)malloc(sizeof(unsigned int)*_WallIndices);

	//normals
	std::vector<glm::vec3> normals(_WallTriangles);

	//UV
	std::vector<glm::vec2> uv(_WallTriangles);

	//für alle starts
	int pos = 0;
	int posNew = 0;
	for (int i = 0; i < nStartPoints; i++)
	{
		//std::cout << "Start:" << starts[i].x << " : " << starts[i].y << std::endl;
		posNew = iterate(vertices, starts[i], pos);

		//setze indices
		//ersten index wiederholen
		if (i != 0)
			indices[pos + 2 * i-1] = pos;
		for (int j = pos; j < posNew; j++)
		{
			indices[j+2*i] = j;
		}
		//letzten index wiederholen
		indices[posNew + 2 * i] = posNew-1;
		
		pos = posNew;

		
	}
	//calculate normals
	glm::vec3 normAlt(0.0f);
	glm::vec3 normCurr;
	for (int i = 0; i < _WallTriangles-2; i+=2)
	{
		normCurr = glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), vertices[i + 2] - vertices[i + 1]);
		normals[i] = glm::normalize(normCurr + normAlt);
		normals[i + 1] = normals[i];

		normAlt = normCurr;
	}

	//set UV
	for (int i = 0; i < _WallTriangles - 1; i += 2)
	{
		uv[i] = glm::vec2(i/6.0f, 0.0f);
		uv[i+1] = glm::vec2(i/6.0f, 1.0f);
	}



	//vertex attribute object (VAO)
	glGenVertexArrays(1, &_vao2);
	glBindVertexArray(_vao2);

	//vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * _WallTriangles, &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//normal buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//uv Buffer
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uv.size(), &uv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	//index Buffer
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*_WallIndices, indices, GL_STATIC_DRAW);
	
	loadImage("img\\wall");

	return 0;
}

int HeightMapper::iterate(std::vector<glm::vec3> &vertices ,glm::vec2 startPoint, int index)
{
	glm::vec2 old(0,0);
	glm::vec2 curent = startPoint;
	glm::vec2 next(0,0);
	bool hasNeibour = true;
	while (hasNeibour)
	{
		vertices[index] = glm::vec3(curent.x, (int)getVertexHeight(curent.x, curent.y), curent.y);
		vertices[index + 1] = vertices[index];
		vertices[index + 1].y += WALLHEIGHT;

		hasNeibour = getNeigbourPixel(old, curent, next);
		index += 2;
		old = curent;
		curent = next;
	}
	//back to start
	vertices[index] = glm::vec3(startPoint.x, (int)getVertexHeight(startPoint.x, startPoint.y), startPoint.y);
	vertices[index + 1] = vertices[index];
	vertices[index + 1].y += WALLHEIGHT;
	index += 2;

	return index;
}

bool HeightMapper::getNeigbourPixel(glm::vec2 old, glm::vec2 test, glm::vec2 &out)
{
	for (int i = 0; i < 8; i++)
	{
		if (isBlack(test + testvector[i]))
		{
			if (test + testvector[i] != old)
			{
				out = test + testvector[i];
				//std::cout << "next:" << (test + testvector[i]).x << " : " << (test + testvector[i]).y << std::endl;
				return true;
			}
		}
	}
	return false;
}

int HeightMapper::getIndicesCount()
{
	return (_height - 1) * 2 * _width + (_height - 2) * 2;
}

int HeightMapper::getTriangleCount()
{
	return (_height - 1)*(_width - 1) * 2 + (_height - 2) * 4;
}

GLuint HeightMapper::getVAO()
{
	return _vao;
}

void HeightMapper::drawBase()
{
	glBindVertexArray(_vao);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, getIndicesCount(), getTriangleCount(), GL_UNSIGNED_INT, NULL);


}

void HeightMapper::drawWall()
{
	glBindVertexArray(_vao2);
	glBindTexture(GL_TEXTURE0, _textureID);
	glDrawRangeElements(GL_TRIANGLE_STRIP, 0, _WallIndices, _WallTriangles+2, GL_UNSIGNED_INT, NULL);
}

GLuint HeightMapper::loadImage(const std::string& path)
{
	std::vector<unsigned char> image;
	unsigned int height, width;

	// load image

	//decode
	unsigned error = lodepng::decode(image, width, height, path + ".png");

	//if there's an error, display it
	if (error){
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return -1;
	}
	
	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA , GL_UNSIGNED_BYTE, image.data());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_FLAT);

}

int HeightMapper::getVertexHeight(int x, int y)
{
	return (int)_image[(y*_width + x) * 4];
}

float HeightMapper::getInterpolatetHeight(float x, float y)
{
	//out of map?
	if (x > _width || x < 0.0f || y < 0.0f || y > _height)
		return -1;
	int xI = (int)floor(x);
	int yI = (int)floor(y);

	/*std::cout << x - xI << std::endl;
	std::cout << y - yI << std::endl;*/

	int a, b, c, d ;
	glm::vec2  i, j, k, l, m;

	//upper Triangle
	if ( (y - (float)yI) > (x - (float)xI))
	{
		//std::cout << "up" << std::endl;
		a = getVertexHeight(xI, yI);
		b = getVertexHeight(xI + 1, yI);
		c = getVertexHeight(xI, yI + 1);
		d = b + c - a; //== a + 2* [mitte(b+c) - (b+c)/2)] -a

		/*std::cout << a << std::endl;
		std::cout << b << std::endl;
		std::cout << c << std::endl;
		std::cout << d << std::endl << std::endl;*/



		i = glm::vec2(x - xI, y - yI);
		j = glm::vec2(xI + 1 - x, y - yI);
		k = glm::vec2(x - xI, yI + 1 -y);
		l = glm::vec2(xI + 1 - x,yI + 1 - y);

		/*std::cout << i.x << " : " << i.y << std::endl;
		std::cout << j.x << " : " << j.y << std::endl;
		std::cout << k.x << " : " << k.y << std::endl;
		std::cout << l.x << " : " << l.y << std::endl << std::endl;*/

	}
	//lower Triangle
	else
	{
		/*std::cout << "down" << std::endl;*/
		//get Triagles vertex heights
		a = getVertexHeight(xI + 1, yI + 1);
		b = getVertexHeight(xI , yI + 1);
		c = getVertexHeight(xI + 1, yI);
		d = b + c - a; //== a + 2* [mitte(b+c) - (b+c)/2)]

		/*std::cout << a << std::endl;
		std::cout << b << std::endl;
		std::cout << c << std::endl;
		std::cout << d << std::endl << std::endl;*/

		// get positive distances to point
		i = glm::vec2(xI + 1 - x, yI + 1 - y);
		j = glm::vec2(x - xI, yI+1 - y);
		k = glm::vec2(xI+1- x, y - yI);
		l = glm::vec2(x - xI, y - yI);

		/*std::cout << i.x << " : " << i.y << std::endl;
		std::cout << j.x << " : " << j.y << std::endl;
		std::cout << k.x << " : " << k.y << std::endl;
		std::cout << l.x << " : " << l.y << std::endl << std::endl;*/
	}

	//normalise vector length sum
	m = i + j + k + l;

	i = i / glm::length(m);
	j = j / glm::length(m);
	k = k / glm::length(m);
	l = l / glm::length(m);

	/*std::cout << i.x << " : " << i.y << std::endl;
	std::cout << j.x << " : " << j.y << std::endl;
	std::cout << k.x << " : " << k.y << std::endl;
	std::cout << l.x << " : " << l.y << std::endl << std::endl;*/
		
	return a*glm::length(i) + b*glm::length(j) + c*glm::length(k) + d*glm::length(l);
}