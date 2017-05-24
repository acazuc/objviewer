#ifndef MTL_PARSER_H
# define MTL_PARSER_H

# include <glm/vec4.hpp>
# include <glm/vec3.hpp>
# include <glm/vec2.hpp>
# include <vector>
# include <string>


namespace objviewer
{

	typedef struct SMtlMaterial
	{
		std::string name;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec4 specular;
		glm::vec3 emissive;
		float density;
		uint8_t illumination;
		std::string diffuseMap;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
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
		void parseKe(std::string &line);
		void parseNs(std::string &line);
		void parseNi(std::string &line);
		void parseIllum(std::string &line);
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
