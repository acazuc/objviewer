#include "ObjParser.h"
#include "Debug.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstring>

namespace objviewer
{

	ObjParser::ObjParser(std::string file)
	: file(file)
	{
		std::ifstream ifs(file);
		if (!ifs.is_open())
			ERROR("can't open file");
		std::string line;
		while (std::getline(ifs, line))
		{
			if (line.length() == 0 || line[0] == '#' || line[0] == ' ')
				continue;
			parseLine(line);
		}
		this->currentMaterial = this->mtl.getDefaultMaterial();
	}

	void ObjParser::parseLine(std::string &line)
	{
		size_t pos = line.find_first_of(' ');
		if (pos == std::string::npos)
			return;
		std::string header = line.substr(0, pos);
		if (!header.compare("v"))
			parseVertice(line);
		else if (!header.compare("vt"))
			parseUv(line);
		else if (!header.compare("vn"))
			parseNormal(line);
		else if (!header.compare("f"))
			parseFace(line);
		else if (!header.compare("usemtl"))
			parseUsemtl(line);
		else if (!header.compare("mtllib"))
			parseMtllib(line);
		else if (!header.compare("o"))
			{}
		else if (!header.compare("s"))
			{}
		else if (!header.compare("g"))
			{}
		else
			WARN("Unknown line: " << line);
	}

	void ObjParser::parseVertice(std::string &line)
	{
		glm::vec3 vertice;
		size_t pos = line.find(' ', 2);
		if (pos == std::string::npos || pos == 2)
		{
			WARN("Invalid vertice line 1");
			return;
		}
		std::string tmp = line.substr(2, pos - 2);
		vertice.x = std::stod(tmp);
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid vertice line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		vertice.y = std::stod(tmp);
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid vertice line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		vertice.z = std::stod(tmp);
		this->verticesIndexes.push_back(vertice);
	}

	void ObjParser::parseUv(std::string &line)
	{
		glm::vec2 uv;
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid uv line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		uv.x = std::stod(tmp);
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid uv line 2");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		uv.y = std::stod(tmp);
		this->uvsIndexes.push_back(uv);
	}

	void ObjParser::parseNormal(std::string &line)
	{
		glm::vec3 normal;
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid normal line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		normal.x = std::stod(tmp);
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid normal line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		normal.y = std::stod(tmp);
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid normal line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		normal.z = std::stod(tmp);
		this->normalsIndexes.push_back(normal);
	}

	void ObjParser::parseFace(std::string &line)
	{
		uint32_t vertexesIndices[3];
		uint32_t normalsIndices[3];
		uint32_t uvsIndices[3];
		std::memset(uvsIndices, 0, sizeof(uvsIndices));
		size_t pos = line.find('/', 2);
		if (pos == std::string::npos || pos == 2)
		{
			WARN("Invalid face line 1");
			return;
		}
		std::string tmp = line.substr(2, pos - 2);
		vertexesIndices[0] = std::stoi(tmp);
		size_t oldPos = pos;
		pos = line.find('/', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
			{
				WARN("Invalid face line 2");
				return;
			}
			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			std::cout << "tmp: " << tmp << ", pos: " << pos << ", oldPos: " << oldPos << std::endl;
			uvsIndices[0] = std::stoi(tmp);
		}
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
			{
				WARN("Invalid face line 3");
				return;
			}
			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			normalsIndices[0] = std::stoi(tmp);
		}
		oldPos = pos;
		pos = line.find('/', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid face line 4");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		vertexesIndices[1] = std::stoi(tmp);
		oldPos = pos;
		pos = line.find('/', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
			{
				WARN("Invalid face line 5");
				return;
			}
			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			uvsIndices[1] = std::stoi(tmp);
		}
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
			{
				WARN("Invalid face line 6");
				return;
			}
			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			normalsIndices[1] = std::stoi(tmp);
		}
		oldPos = pos;
		pos = line.find('/', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid face line 7");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		vertexesIndices[2] = std::stoi(tmp);
		oldPos = pos;
		pos = line.find('/', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
			{
				WARN("Invalid face line 8");
				return;
			}
			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			uvsIndices[2] = std::stoi(tmp);
		}
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos != oldPos + 1)
		{
			if (pos == std::string::npos)
				tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
			else

				tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
			normalsIndices[2] = std::stoi(tmp);
		}
		if (this->verticesIndexes.size() < vertexesIndices[0])
		{
			WARN("Invalid face vertex 1");
			return;
		}
		if (this->verticesIndexes.size() < vertexesIndices[1])
		{
			WARN("Invalid face vertex 2");
			return;
		}
		if (this->verticesIndexes.size() < vertexesIndices[2])
		{
			WARN("Invalid face vertex 3");
			return;
		}
		if (this->normalsIndexes.size() < normalsIndices[0])
		{
			WARN("Invalid face normal 1");
			return;
		}
		if (this->normalsIndexes.size() < normalsIndices[1])
		{
			WARN("Invalid face normal 2");
			return;
		}
		if (this->normalsIndexes.size() < normalsIndices[2])
		{
			WARN("Invalid face normal 3");
			return;
		}
		if (uvsIndices[0] && this->uvsIndexes.size() < uvsIndices[0])
		{
			WARN("Invalid face uv 1");
			return;
		}
		if (uvsIndices[1] && this->uvsIndexes.size() < uvsIndices[1])
		{
			WARN("Invalid face uv 2");
			return;
		}
		if (uvsIndices[3] && this->uvsIndexes.size() < uvsIndices[2])
		{
			WARN("Invalid face uv 3");
			return;
		}
		this->currentMaterial->vertices.push_back(this->verticesIndexes[vertexesIndices[0] - 1]);
		this->currentMaterial->vertices.push_back(this->verticesIndexes[vertexesIndices[1] - 1]);
		this->currentMaterial->vertices.push_back(this->verticesIndexes[vertexesIndices[2] - 1]);
		this->currentMaterial->normals.push_back(this->normalsIndexes[normalsIndices[0] - 1]);
		this->currentMaterial->normals.push_back(this->normalsIndexes[normalsIndices[1] - 1]);
		this->currentMaterial->normals.push_back(this->normalsIndexes[normalsIndices[2] - 1]);
		if (uvsIndices[0])
			this->currentMaterial->uvs.push_back(this->uvsIndexes[uvsIndices[0] - 1]);
		else
			this->currentMaterial->uvs.push_back(glm::vec3(0, 0, 0));
		if (uvsIndices[1])
			this->currentMaterial->uvs.push_back(this->uvsIndexes[uvsIndices[1] - 1]);
		else
			this->currentMaterial->uvs.push_back(glm::vec3(0, 0, 0));
		if (uvsIndices[2])
			this->currentMaterial->uvs.push_back(this->uvsIndexes[uvsIndices[2] - 1]);
		else
			this->currentMaterial->uvs.push_back(glm::vec3(0, 0, 0));
	}

	void ObjParser::parseUsemtl(std::string &line)
	{
		std::string materialName = line.substr(7, line.length() - 7);
		this->currentMaterial = this->mtl.getMaterial(materialName);
	}

	void ObjParser::parseMtllib(std::string &line)
	{
		std::string directory;
		size_t dirPos = this->file.find_last_of('/');
		if (dirPos == std::string::npos)
			directory = "./";
		else
			directory = this->file.substr(0, dirPos + 1);
		std::string file = directory + line.substr(7, line.length() - 7);
		this->mtl.loadFile(file);
	}
}
