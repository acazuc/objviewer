#include "Main.h"
#include "ObjParser.h"
#include <librender/Shader/FragmentShader.h>
#include <librender/Shader/VertexShader.h>
#include <librender/Shader/Program.h>
#include <glm/gtc/matrix_transform.hpp>
#include <libformat/TGA.h>
#include <glm/glm.hpp>
#include <cstring>
#include "FpsManager.h"
#include "Debug.h"

using librender::FragmentShader;
using librender::VertexShader;
using librender::Program;

static char vshad[] = {"#version 120\n\
\n\
attribute vec3 vertexPosition;\n\
attribute vec3 vertexNormal;\n\
attribute vec2 vertexUV;\n\
\n\
varying vec3 Normal_cameraspace;\n\
varying vec3 LightDirection_cameraspace;\n\
varying vec3 Position_worldspace;\n\
varying vec3 EyeDirection_cameraspace;\n\
varying vec2 UV;\n\
\n\
uniform mat4 MVP;\n\
uniform mat4 M;\n\
uniform mat4 V;\n\
uniform vec3 LightPosition_worldspace;\n\
\n\
void main()\n\
{\n\
	gl_Position =  MVP * vec4(vertexPosition, 1);\n\
	Position_worldspace = (M * vec4(vertexPosition, 1)).xyz;\n\
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition, 1)).xyz;\n\
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n\
	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;\n\
	vec3 EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;\n\
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;\n\
	Normal_cameraspace = (V * M * vec4(vertexNormal, 0)).xyz;\n\
	UV = vertexUV;\n\
}\n\
"};

static char fshad[] = {"#version 120\n\
\n\
varying vec3 Normal_cameraspace;\n\
varying vec3 LightDirection_cameraspace;\n\
varying vec3 Position_worldspace;\n\
varying vec3 EyeDirection_cameraspace;\n\
varying vec2 UV;\n\
\n\
uniform vec3 LightPosition_worldspace;\n\
uniform vec3 ambientColor;\n\
uniform vec3 diffuseColor;\n\
uniform vec4 specularColor;\n\
uniform vec3 emissiveColor;\n\
uniform sampler2D tex;\n\
\n\
void main()\n\
{\n\
	vec3 LightColor = vec3(4.5, 4.5, 4.5);\n\
	float distance = length(LightPosition_worldspace - Position_worldspace);\n\
	vec3 n = normalize(Normal_cameraspace);\n\
	vec3 l = normalize(LightDirection_cameraspace);\n\
	float cosTheta = clamp(dot(n, l), 0, 1);\n\
	vec3 E = normalize(EyeDirection_cameraspace);\n\
	vec3 R = reflect(-l, n);\n\
	float cosAlpha = clamp(dot(E, R), 0, 1);\n\
	vec3 color = diffuseColor * LightColor * cosTheta + ambientColor + emissiveColor;\n\
	vec4 texCol = texture2D(tex, UV);\n\
	gl_FragColor = texCol * vec4(color, 1) + vec4(specularColor.xyz * LightColor * pow(cosAlpha, specularColor.w) * cosAlpha, 1);\n\
}\n\
"};

# define MOVE_SPEED 1
/*
static void GLErrors(std::string err)
{
	GLenum glErr;
	while ((glErr = glGetError()) != GL_NO_ERROR)
	{
		std::cout << err << ": ";
		if (glErr == GL_INVALID_ENUM)
			std::cout << "GL_INVALID_ENUM" << std::endl;
		else if (glErr == GL_INVALID_VALUE)
			std::cout << "GL_INVALID_VALUE" << std::endl;
		else if (glErr == GL_INVALID_OPERATION)
			std::cout << "GL_INVALID_OPERATION" << std::endl;
		else if (glErr == GL_STACK_OVERFLOW)
			std::cout << "GL_STACK_OVERFLOW" << std::endl;
		else if (glErr == GL_STACK_UNDERFLOW)
			std::cout << "GL_STACK_UNDERFLOW" << std::endl;
		else if (glErr == GL_OUT_OF_MEMORY)
			std::cout << "GL_OUT_OF_MEMORY" << std::endl;
	}
}
*/
namespace objviewer
{

	Window *Main::window;
	float Main::posX;
	float Main::posY;
	float Main::posZ;

	void Main::main(int ac, char **av)
	{
		posX = 0;
		posY = 0;
		posZ = 0;
		if (ac != 2)
			ERROR("You must specify an obj file");
		ObjParser obj(av[1]);
		glfwWindowHint(GLFW_SAMPLES, 8);
		window = new Window("ObjViewer", 1920, 1080);
		window->enableFullscreen();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glewExperimental = GL_TRUE;
		GLenum glewErr = glewInit();
		if (glewErr != GLEW_OK)
			ERROR("Failed to init GLEW: " << glewGetErrorString(glewErr));
		glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		window->setVSync(true);
		FragmentShader *frag = new FragmentShader(fshad, sizeof(fshad));
		VertexShader *vert = new VertexShader(vshad, sizeof(vshad));
		Program *prog = new Program(frag, vert);
		GLint MatrixID = glGetUniformLocation(prog->getId(), "MVP");
		GLint ViewMatrixID = glGetUniformLocation(prog->getId(), "V");
		GLint ModelMatrixID = glGetUniformLocation(prog->getId(), "M");
		GLint vertexPosition_modelspaceID = glGetAttribLocation(prog->getId(), "vertexPosition");
		GLint vertexNormal_modelspaceID = glGetAttribLocation(prog->getId(), "vertexNormal");
		GLint vertexUV_modelspaceID = glGetAttribLocation(prog->getId(), "vertexUV");
		GLint AmbientColorID = glGetUniformLocation(prog->getId(), "ambientColor");
		GLint DiffuseColorID = glGetUniformLocation(prog->getId(), "diffuseColor");
		GLint SpecularColorID = glGetUniformLocation(prog->getId(), "specularColor");
		GLint EmissiveColorID = glGetUniformLocation(prog->getId(), "emissiveColor");
		GLint LightID = glGetUniformLocation(prog->getId(), "LightPosition_worldspace");
		GLint TexID = glGetUniformLocation(prog->getId(), "tex");
		uint32_t partsCount = obj.getMtlParser().getMaterials().size();
		GLuint vertexBuffer[partsCount];
		glGenBuffers(partsCount, vertexBuffer);
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, obj.getMtlParser().getMaterials()[i].vertices.size() * sizeof(glm::vec3), &obj.getMtlParser().getMaterials()[i].vertices[0], GL_STATIC_DRAW);
		}
		GLuint normalBuffer[partsCount];
		glGenBuffers(partsCount, normalBuffer);
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, obj.getMtlParser().getMaterials()[i].normals.size() * sizeof(glm::vec3), &obj.getMtlParser().getMaterials()[i].normals[0], GL_STATIC_DRAW);
		}
		GLuint textures[partsCount];
		glGenTextures(partsCount, textures);
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			uint32_t width;
			uint32_t height;
			char *datas;
			if (!libformat::TGA::read(obj.getMtlParser().getMaterials()[i].diffuseMap, datas, width, height))
			{
				WARN("Failed to read " << obj.getMtlParser().getMaterials()[i].diffuseMap);
				datas = new char[4];
				std::memset(datas, 0xff, 4);
				width = 1;
				height = 1;
			}
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, datas);
			delete[] (datas);
		}
		GLuint uvBuffer[partsCount];
		glGenBuffers(partsCount, uvBuffer);
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, obj.getMtlParser().getMaterials()[i].uvs.size() * sizeof(glm::vec3), &obj.getMtlParser().getMaterials()[i].uvs[0], GL_STATIC_DRAW);
		}
		FpsManager::init();
		while (!window->closeRequested())
		{
			FpsManager::update();
			std::string title = std::to_string(FpsManager::getFps());
			window->setTitle(title);
			window->clearScreen();
			window->pollEvents();
			prog->use();
			if (window->isKeyDown(GLFW_KEY_W))
				posZ += MOVE_SPEED;
			if (window->isKeyDown(GLFW_KEY_S))
				posZ -= MOVE_SPEED;
			if (window->isKeyDown(GLFW_KEY_A))
				posX -= MOVE_SPEED;
			if (window->isKeyDown(GLFW_KEY_D))
				posX += MOVE_SPEED;
			if (window->isKeyDown(GLFW_KEY_SPACE))
				posY += MOVE_SPEED;
			if (window->isKeyDown(GLFW_KEY_LEFT_SHIFT))
				posY -= MOVE_SPEED;
			glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 1280 / (float)720, 0.1f, 1000.0f);
			glm::mat4 View = glm::mat4(1.0f);
			View = glm::rotate(View, glm::vec2(window->getMouseY() / 4000. * M_PI, 0).x, glm::vec3(1, 0, 0));
			View = glm::rotate(View, glm::vec2(window->getMouseX() / 4000. * M_PI, 0).x, glm::vec3(0, 1, 0));
			View = glm::translate(View, glm::vec3(posX, -posY, -posZ));
			glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0));
			glm::mat4 mvp = Projection * View * Model;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
			glUniform3f(LightID, 0, 0, -5);
			glEnableVertexAttribArray(vertexPosition_modelspaceID);
			glEnableVertexAttribArray(vertexNormal_modelspaceID);
			glEnableVertexAttribArray(vertexUV_modelspaceID);
			for (uint32_t i = 0; i < partsCount; ++i)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glUniform1i(TexID, 0);
				glUniform3f(AmbientColorID, obj.getMtlParser().getMaterials()[i].ambient.x, obj.getMtlParser().getMaterials()[i].ambient.y, obj.getMtlParser().getMaterials()[i].ambient.z);
				glUniform3f(DiffuseColorID, obj.getMtlParser().getMaterials()[i].diffuse.x, obj.getMtlParser().getMaterials()[i].diffuse.y, obj.getMtlParser().getMaterials()[i].diffuse.z);
				glUniform4f(SpecularColorID, obj.getMtlParser().getMaterials()[i].specular.x, obj.getMtlParser().getMaterials()[i].specular.y, obj.getMtlParser().getMaterials()[i].specular.z, obj.getMtlParser().getMaterials()[i].specular.w);
				glUniform3f(EmissiveColorID, obj.getMtlParser().getMaterials()[i].emissive.x, obj.getMtlParser().getMaterials()[i].emissive.y, obj.getMtlParser().getMaterials()[i].emissive.z);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
				glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
				glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
				glVertexAttribPointer(vertexUV_modelspaceID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glDrawArrays(GL_TRIANGLES, 0, obj.getMtlParser().getMaterials()[i].vertices.size());
			}
			glDisableVertexAttribArray(vertexPosition_modelspaceID);
			glDisableVertexAttribArray(vertexNormal_modelspaceID);
			glDisableVertexAttribArray(vertexUV_modelspaceID);
			window->update();
		}
	}

}

int main(int ac, char **av)
{
	if (!glfwInit())
		ERROR("Window: can't init glfw");
	objviewer::Main::main(ac, av);
	return (EXIT_SUCCESS);
}
