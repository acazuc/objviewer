#ifndef MTL_PARSER_H
# define MTL_PARSER_H

# include <librender/vec/vec4.h>
# include <librender/vec/vec3.h>
# include <librender/vec/vec2.h>
# include <vector>
# include <string>

using librender::vec4;
using librender::vec3;
using librender::vec2;

namespace objviewer
{

	typedef struct SMtlMaterial
	{
		std::string name;
		vec3 ambient;
		vec3 diffuse;
		vec4 specular;
		std::string diffuseMap;
		std::vector<vec3> vertices;
		std::vector<vec3> normals;
		std::vector<vec2> uvs;
	} MtlMaterial;

	class MtlParser
	{

	private:
		std::vector<MtlMaterial> materials;
		MtlMaterial *currentMaterial;
		MtlMaterial defaultMaterial;
		std::string file;
		void parseLine(std::string &line);
		void parseNewmtl(std::string &line);
		void parseKa(std::string &line);
		void parseKd(std::string &line);
		void parseKs(std::string &line);
		void parseNs(std::string &line);
		void parseMap_Kd(std::string &line);

	public:
		MtlParser();
		void loadFile(std::string &file);
		MtlMaterial *getMaterial(std::string &name);
		MtlMaterial *getDefaultMaterial() {return (&this->defaultMaterial);};
		inline std::vector<MtlMaterial> &getMaterials() {return (this->materials);};

	};

};

#endif
