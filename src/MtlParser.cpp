#include "MtlParser.h"
#include "Debug.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstring>

namespace objviewer
{

	MtlParser::MtlParser()
	: currentMaterial(NULL)
	{
		std::memset(&this->defaultMaterial, 0, sizeof(this->defaultMaterial));
		this->defaultMaterial.diffuse.x = 0.5;
		this->defaultMaterial.diffuse.y = 0.5;
		this->defaultMaterial.diffuse.z = 0.5;
		this->defaultMaterial.opacity = 1;
		this->defaultMaterial.specular.x = 1;
		this->defaultMaterial.specular.y = 1;
		this->defaultMaterial.specular.z = 1;
		this->defaultMaterial.specular.w = 1;
	}

	void MtlParser::loadFile(std::string &file)
	{
		this->file = file;
		std::ifstream ifs(file);
		if (!ifs.is_open())
			ERROR("can't open file (" << file << ")");
		std::string line;
		while (std::getline(ifs, line))
		{
			if (line.length() == 0 || line[0] == '#' || line[0] == ' ')
				continue;
			parseLine(line);
		}
	}

	void MtlParser::parseLine(std::string &line)
	{
		size_t pos = line.find_first_of(' ');
		if (pos == std::string::npos)
			return;
		std::string header = line.substr(0, pos);
		if (!header.compare("newmtl"))
			parseNewmtl(line);
		else if (!header.compare("Ka"))
			parseKa(line);
		else if (!header.compare("Kd"))
			parseKd(line);
		else if (!header.compare("Ks"))
			parseKs(line);
		else if (!header.compare("Ke"))
			parseKe(line);
		else if (!header.compare("Ns"))
			parseNs(line);
		else if (!header.compare("Ni"))
			parseNi(line);
		else if (!header.compare("illum"))
			parseIllum(line);
		else if (!header.compare("map_Kd"))
			parseMap_Kd(line);
		else if (!header.compare("d"))
			parseD(line);
		else if (!header.compare("Tr"))
			parseTr(line);
		else
			WARN("Unknown line: " << line);
	}

	void MtlParser::parseNewmtl(std::string &line)
	{
		std::string materialName = line.substr(7, line.length() - 7);
		MtlMaterial material;
		std::memset(&material, 0, sizeof(material));
		material.name = materialName;
		material.diffuse.x = .5;
		material.diffuse.y = .5;
		material.diffuse.z = .5;
		material.opacity = 1;
		material.specular.x = 1;
		material.specular.y = 1;
		material.specular.z = 1;
		material.specular.w = 1;
		this->materials.push_back(material);
		this->currentMaterial = &this->materials.back();
	}

	void MtlParser::parseKa(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseKa: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid Ka line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		this->currentMaterial->ambient.x = std::min(1., std::max(0., std::stod(tmp)));
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid Ka line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->ambient.y = std::min(1., std::max(0., std::stod(tmp)));
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid Ka line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->ambient.z = std::min(1., std::max(0., std::stod(tmp)));
	}

	void MtlParser::parseKd(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseKd: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid Kd line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		this->currentMaterial->diffuse.x = std::min(1., std::max(0., std::stod(tmp)));
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid Kd line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->diffuse.y = std::min(1., std::max(0., std::stod(tmp)));
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid Kd line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->diffuse.z = std::min(1., std::max(0., std::stod(tmp)));
	}

	void MtlParser::parseKs(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseKs: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid Ks line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		this->currentMaterial->specular.x = std::min(1., std::max(0., std::stod(tmp)));
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid Ks line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->specular.y = std::min(1., std::max(0., std::stod(tmp)));
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid Ks line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->specular.z = std::min(1., std::max(0., std::stod(tmp)));
	}

	void MtlParser::parseKe(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseKe: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		if (pos == std::string::npos || pos == 3)
		{
			WARN("Invalid Ke line 1");
			return;
		}
		std::string tmp = line.substr(3, pos - 3);
		this->currentMaterial->emissive.x = std::min(1., std::max(0., std::stod(tmp)));
		size_t oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == std::string::npos || pos == oldPos)
		{
			WARN("Invalid Ke line 2");
			return;
		}
		tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->emissive.y = std::min(1., std::max(0., std::stod(tmp)));
		oldPos = pos;
		pos = line.find(' ', pos + 1);
		if (pos == oldPos)
		{
			WARN("Invalid Ke line 3");
			return;
		}
		if (pos == std::string::npos)
			tmp = line.substr(oldPos + 1, line.length() - (oldPos + 1));
		else

			tmp = line.substr(oldPos + 1, pos - (oldPos + 1));
		this->currentMaterial->emissive.z = std::min(1., std::max(0., std::stod(tmp)));
	}

	void MtlParser::parseNs(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseNs: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		std::string tmp;
		if (pos == std::string::npos)
			tmp = line.substr(3, line.length() - 3);
		else

			tmp = line.substr(3, pos - 3);
		this->currentMaterial->specular.w = std::min(1., std::max(0., std::stod(tmp)));
	}

	void MtlParser::parseNi(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseNi: no current material");
			return;
		}
		size_t pos = line.find(' ', 3);
		std::string tmp;
		if (pos == std::string::npos)
			tmp = line.substr(3, line.length() - 3);
		else

			tmp = line.substr(3, pos - 3);
		this->currentMaterial->density = std::min(10., std::max(0.001, std::stod(tmp)));
	}

	void MtlParser::parseIllum(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseIllum: no current material");
			return;
		}
		size_t pos = line.find(' ', 6);
		std::string tmp;
		if (pos == std::string::npos)
			tmp = line.substr(6, line.length() - 6);
		else

			tmp = line.substr(6, pos - 6);
		this->currentMaterial->illumination = std::min(10, std::max(0, std::stoi(tmp)));
	}

	void MtlParser::parseMap_Kd(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseMap_Kd: no current material");
			return;
		}
		std::string directory;
		size_t dirPos = this->file.find_last_of('/');
		if (dirPos == std::string::npos)
			directory = "./";
		else
			directory = this->file.substr(0, dirPos + 1);
		this->currentMaterial->diffuseMap = directory + line.substr(7, line.length() - 7);
	}

	void MtlParser::parseD(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseD: no current material");
			return;
		}
		size_t pos = line.find(' ', 2);
		std::string tmp;
		if (pos == std::string::npos)
			tmp = line.substr(2, line.length() - 2);
		else

			tmp = line.substr(2, pos - 2);
		this->currentMaterial->opacity = std::min(1., std::max(0., std::stod(tmp)));
	
	}
	
	void MtlParser::parseTr(std::string &line)
	{
		if (!this->currentMaterial)
		{
			WARN("parseTr: no current material");
			return;
		}
		size_t pos = line.find(' ', 2);
		std::string tmp;
		if (pos == std::string::npos)
			tmp = line.substr(2, line.length() - 2);
		else

			tmp = line.substr(2, pos - 2);
		this->currentMaterial->opacity = 1 - std::min(1., std::max(0., std::stod(tmp)));
	
	}

	MtlMaterial *MtlParser::getMaterial(std::string &material)
	{
		for (uint32_t i = 0; i < this->materials.size(); ++i)
		{
			if (!material.compare(this->materials[i].name))
				return (&this->materials[i]);
		}
		return (&this->defaultMaterial);
	}

}
