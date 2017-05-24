#ifndef OBJ_PARSER_H
# define OBJ_PARSER_H

# include "MtlParser.h"
# include <glm/vec3.hpp>
# include <glm/vec2.hpp>
# include <vector>
# include <string>


namespace objviewer
{

	class ObjParser
	{

	private:
		std::vector<glm::vec3> verticesIndexes;
		std::vector<glm::vec3> normalsIndexes;
		std::vector<glm::vec2> uvsIndexes;
		std::string file;
		MtlMaterial *currentMaterial;
		MtlParser mtl;
		void parseLine(std::string &line);
		void parseVertice(std::string &line);
		void parseNormal(std::string &line);
		void parseUv(std::string &line);
		void parseFace(std::string &line);
		void parseUsemtl(std::string &line);
		void parseMtllib(std::string &line);

	public:
		ObjParser(std::string file);
		inline MtlParser &getMtlParser() {return (this->mtl);};

	};

}

#endif
