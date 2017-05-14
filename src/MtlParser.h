#ifndef MTL_PARSER_H
# define MTL_PARSER_H

# include <librender/vec/vec4.h>
# include <librender/vec/vec3.h>
# include <unordered_map>

using librender::vec4;
using librender::vec3;

namespace objviewer
{

	typedef struct SMtlMaterial
	{
		std::string name;
		vec3 ambient;
		vec3 diffuse;
		vec4 specular;
		std::string diffuseMap;
	} MtlMaterial;

	class MtlParser
	{

	private:
		std::unordered_map<std::string, MtlMaterial> materials;
		MtlMaterial *currentMaterial;
		MtlMaterial defaultMaterial;
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

	};

};

#endif
