#ifndef OBJ_PARSER_H
# define OBJ_PARSER_H

# include "MtlParser.h"
# include <librender/vec/vec3.h>
# include <librender/vec/vec2.h>
# include <vector>
# include <string>

using librender::vec3;
using librender::vec2;

namespace objviewer
{

	class ObjParser
	{

	private:
		std::vector<vec3> verticesIndexes;
		std::vector<vec3> normalsIndexes;
		std::vector<vec2> uvsIndexes;
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
