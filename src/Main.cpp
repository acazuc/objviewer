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

using librender::ProgramLocation;
using librender::FragmentShader;
using librender::VertexShader;
using librender::VertexBuffer;
using librender::Program;

static char vshad[] = {"#version 120\n\
\n\
attribute vec3 vertexPosition;\n\
attribute vec3 vertexNormal;\n\
attribute vec2 vertexUV;\n\
\n\
varying vec3 normal_cameraspace;\n\
varying vec3 lightDirection_cameraspace;\n\
varying vec3 position_worldspace;\n\
varying vec3 eyeDirection_cameraspace;\n\
varying vec2 UV;\n\
\n\
uniform mat4 MVP;\n\
uniform mat4 M;\n\
uniform mat4 V;\n\
uniform vec3 lightPosition;\n\
\n\
void main()\n\
{\n\
	gl_Position =  MVP * vec4(vertexPosition, 1);\n\
	position_worldspace = (M * vec4(vertexPosition, 1)).xyz;\n\
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition, 1)).xyz;\n\
	eyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n\
	vec3 lightPosition_cameraspace = (V * vec4(lightPosition, 1)).xyz;\n\
	vec3 eyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;\n\
	lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;\n\
	normal_cameraspace = (V * M * vec4(vertexNormal, 0)).xyz;\n\
	UV = vertexUV;\n\
}\n\
"};

static char fshad[] = {"#version 120\n\
\n\
varying vec3 normal_cameraspace;\n\
varying vec3 lightDirection_cameraspace;\n\
varying vec3 position_worldspace;\n\
varying vec3 eyeDirection_cameraspace;\n\
varying vec2 UV;\n\
\n\
uniform vec3 lightPosition;\n\
uniform vec3 ambientColor;\n\
uniform vec3 diffuseColor;\n\
uniform vec4 specularColor;\n\
uniform vec3 emissiveColor;\n\
uniform float ambientValue;\n\
uniform float opacity;\n\
uniform sampler2D tex;\n\
\n\
void main()\n\
{\n\
	vec3 lightColor = vec3(1, 1, 1);\n\
	float lightPower = 1;\n\
	float distance = length(lightPosition - position_worldspace);\n\
	vec3 n = normalize(normal_cameraspace);\n\
	vec3 l = normalize(lightDirection_cameraspace);\n\
	float diffuseFactor = clamp(dot(n, l), 0, 1);\n\
	vec3 E = normalize(eyeDirection_cameraspace);\n\
	vec3 R = reflect(-l, n);\n\
	float specularFactor = clamp(dot(E, R), 0, 1);\n\
	vec3 diffuse = diffuseColor * lightColor * diffuseFactor * lightPower;\n\
	vec3 ambient = ambientColor * vec3(ambientValue, ambientValue, ambientValue);\n\
	vec3 color = diffuse + ambient + emissiveColor;\n\
	vec3 specular = specularColor.xyz * lightColor * clamp(lightPower * pow(specularFactor, specularColor.w), 0, 1);\n\
	vec4 texCol = texture2D(tex, UV);\n\
	gl_FragColor = vec4(vec3(texCol.xyz * color + specular), clamp(opacity * texCol.w, 0, 1));\n\
}\n\
"};

# define MOVE_SPEED 0.1
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

uint64_t nanotime()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000000000 + ts.tv_nsec);
}

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
		uint64_t started = nanotime();
		ObjParser obj(av[1]);
		uint64_t ended = nanotime();
		LOG("parsed in " << (ended - started) / 1000000 << " ms");
		glfwWindowHint(GLFW_SAMPLES, 8);
		window = new Window("ObjViewer", 1920, 1080);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			ERROR("Failed to initialize OpenGL context");
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glClearColor(0, 0, 0, 1);
		window->show();
		//window->enableFullscreen();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		window->setVSync(true);
		FragmentShader *frag = new FragmentShader(fshad);
		VertexShader *vert = new VertexShader(vshad);
		Program *prog = new Program();
		prog->attachShader(vert);
		prog->attachShader(frag);
		prog->link();
		ProgramLocation *matrixLocation = prog->getUniformLocation("MVP");
		std::cout << matrixLocation->getLocation() << std::endl;
		ProgramLocation *viewMatrixLocation = prog->getUniformLocation("V");
		ProgramLocation *modelMatrixLocation = prog->getUniformLocation("M");
		ProgramLocation *vertexPositionLocation = prog->getAttribLocation("vertexPosition");
		vertexPositionLocation->setVertexAttribArray(true);
		std::cout << vertexPositionLocation->getLocation() << std::endl;
		ProgramLocation *vertexNormalLocation = prog->getAttribLocation("vertexNormal");
		vertexNormalLocation->setVertexAttribArray(true);
		ProgramLocation *vertexUVLocation = prog->getAttribLocation("vertexUV");
		vertexUVLocation->setVertexAttribArray(true);
		ProgramLocation *ambientColorLocation = prog->getUniformLocation("ambientColor");
		ProgramLocation *diffuseColorLocation = prog->getUniformLocation("diffuseColor");
		ProgramLocation *specularColorLocation = prog->getUniformLocation("specularColor");
		ProgramLocation *emissiveColorLocation = prog->getUniformLocation("emissiveColor");
		ProgramLocation *lightPositionLocation = prog->getUniformLocation("lightPosition");
		ProgramLocation *texLocation = prog->getUniformLocation("tex");
		ProgramLocation *opacityLocation = prog->getUniformLocation("opacity");
		ProgramLocation *ambientValueLocation = prog->getUniformLocation("ambientValue");
		uint32_t partsCount = obj.getMtlParser().getMaterials().size();
		VertexBuffer *vertexBuffers = new VertexBuffer[partsCount];
		uint32_t trianglesNumber = 0;
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			trianglesNumber += obj.getMtlParser().getMaterials()[i].vertices.size() / 3;
			MtlMaterial &material = obj.getMtlParser().getMaterials()[i];
			vertexBuffers[i].setData(GL_ARRAY_BUFFER, material.vertices.data(), material.vertices.size() * sizeof(glm::vec3), GL_FLOAT, 3, GL_STATIC_DRAW);
		}
		LOG("Triangles number: " << trianglesNumber);
		VertexBuffer *normalBuffers = new VertexBuffer[partsCount];
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			MtlMaterial &material = obj.getMtlParser().getMaterials()[i];
			normalBuffers[i].setData(GL_ARRAY_BUFFER, material.normals.data(), material.normals.size() * sizeof(glm::vec3), GL_FLOAT, 3, GL_STATIC_DRAW);
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, datas);
			delete[] (datas);
		}
		//GLuint uvBuffer[partsCount];
		//glGenBuffers(partsCount, uvBuffer);
		VertexBuffer *uvBuffers = new VertexBuffer[partsCount];
		for (uint32_t i = 0; i < partsCount; ++i)
		{
			MtlMaterial &material = obj.getMtlParser().getMaterials()[i];
			uvBuffers[i].setData(GL_ARRAY_BUFFER, material.uvs.data(), material.uvs.size() * sizeof(glm::vec3), GL_FLOAT, 2, GL_STATIC_DRAW);
		}
		FpsManager::init();
		while (!window->closeRequested())
		{
			FpsManager::update();
			std::string title = std::to_string(FpsManager::getFps()) + " fps";
			window->setTitle(title);
			window->clearScreen();
			window->pollEvents();
			static bool vs = true;
			if (window->isKeyDown(GLFW_KEY_V))
			{
				vs = !vs;
				window->setVSync(vs);
			}
			prog->use();
			{
				double angle = 90 + window->getMouseX() / 4000. * 180;
				bool wDown = window->isKeyDown(GLFW_KEY_W);
				bool sDown = window->isKeyDown(GLFW_KEY_S);
				bool aDown = window->isKeyDown(GLFW_KEY_A);
				bool dDown = window->isKeyDown(GLFW_KEY_D);
				bool spDown = window->isKeyDown(GLFW_KEY_SPACE);
				bool shDown = window->isKeyDown(GLFW_KEY_LEFT_SHIFT);
				if (wDown && sDown)
				{
					wDown = false;
					sDown = false;
				}
				if (aDown && dDown)
				{
					aDown = false;
					dDown = false;
				}
				if (spDown && shDown)
				{
					spDown = false;
					shDown = false;
				}
				if (wDown || aDown || sDown || dDown)
				{
					if (wDown && aDown)
						angle += 135;
					else if (wDown && dDown)
						angle -= 135;
					else if (sDown && aDown)
						angle += 45;
					else if (sDown && dDown)
						angle -= 45;
					else if (wDown)
						angle += 180;
					else if (aDown)
						angle += 90;
					else if (dDown)
						angle -= 90;
					posX += cos(angle / 180 * M_PI) * MOVE_SPEED;
					posZ += sin(angle / 180 * M_PI) * MOVE_SPEED;
				}
				if (shDown)
					posY -= MOVE_SPEED;
				else if (spDown)
					posY += MOVE_SPEED;
			}
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) 1280 / (float)720, 0.1f, 1000.0f);
			glm::mat4 view = glm::mat4(1.0f);
			view = glm::rotate(view, glm::vec2(window->getMouseY() / 4000. * M_PI, 0).x, glm::vec3(1, 0, 0));
			view = glm::rotate(view, glm::vec2(window->getMouseX() / 4000. * M_PI, 0).x, glm::vec3(0, 1, 0));
			view = glm::translate(view, glm::vec3(-posX, -posY, -posZ));
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0));
			glm::mat4 mvp = projection * view * model;
			matrixLocation->setMat4f(mvp);
			viewMatrixLocation->setMat4f(view);
			modelMatrixLocation->setMat4f(model);
			glm::vec3 lightPosition(4.85, 4.32, 5.44);
			lightPositionLocation->setVec3f(lightPosition);
			for (uint32_t i = 0; i < partsCount; ++i)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				texLocation->setVec1i(0);
				ambientValueLocation->setVec1f(.1);
				MtlMaterial &material = obj.getMtlParser().getMaterials()[i];
				ambientColorLocation->setVec3f(material.ambient);
				diffuseColorLocation->setVec3f(material.diffuse);
				specularColorLocation->setVec4f(material.specular);
				emissiveColorLocation->setVec3f(material.emissive);
				opacityLocation->setVec1f(material.opacity);
				vertexPositionLocation->setVertexBuffer(vertexBuffers[i]);
				vertexNormalLocation->setVertexBuffer(normalBuffers[i]);
				vertexUVLocation->setVertexBuffer(uvBuffers[i]);
				glDrawArrays(GL_TRIANGLES, 0, material.vertices.size());
			}
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
