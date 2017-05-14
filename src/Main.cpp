#include "Main.h"
#include "ObjParser.h"
#include <librender/Shader/FragmentShader.h>
#include <librender/Shader/VertexShader.h>
#include <librender/Shader/Program.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "Debug.h"

using librender::FragmentShader;
using librender::VertexShader;
using librender::Program;

static char vshad[] = {"#version 120\n\
\n\
attribute vec3 vertexPosition;\n\
//attribute vec4 vertexColor;\n\
//attribute vec2 vertexUV;\n\
\n\
//varying vec4 fragmentColor;\n\
//varying vec2 UV;\n\
uniform mat4 MVP;\n\
\n\
void main(){\n\
\n\
	gl_Position =  MVP * vec4(vertexPosition, 1);\n\
	//fragmentColor = vertexColor;\n\
	//UV = vertexUV;\n\
}\n\
"};

static char fshad[] = {"#version 120\n\
\n\
//varying vec4 fragmentColor;\n\
//varying vec2 UV;\n\
\n\
//uniform sampler2D textureSampler;\n\
\n\
void main(){\n\
	gl_FragColor = vec4(1, 0, 0, 1);\n\
	//vec4(fragmentColor) * texture2D(textureSampler, UV);\n\
}\n\
"};

# define MOVE_SPEED 0.01

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
		window = new Window("ObjViewer", 1280, 720);
		glewExperimental = GL_TRUE;
		GLenum glewErr = glewInit();
		if (glewErr != GLEW_OK)
			ERROR("Failed to init GLEW: " << glewGetErrorString(glewErr));
		glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		window->setVSync(true);
		FragmentShader *frag = new FragmentShader(fshad, sizeof(fshad));
		VertexShader *vert = new VertexShader(vshad, sizeof(vshad));
		Program *prog = new Program(frag, vert);
		GLuint MatrixID = glGetUniformLocation(prog->getId(), "MVP");
		GLuint vertexPositionId = glGetAttribLocation(prog->getId(), "vertexPosition");
		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj.getVertices().size() * sizeof(vec3), &obj.getVertices()[0], GL_STATIC_DRAW);
		while (!window->closeRequested())
		{
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
			glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) 1280 / (float)720, 0.1f, 100.0f);
			glm::mat4 View = glm::mat4(1.0f);
			View = glm::rotate(View, glm::vec2(window->getMouseY() / 4000. * M_PI, 0).x, glm::vec3(1, 0, 0));
			View = glm::rotate(View, glm::vec2(window->getMouseX() / 4000. * M_PI, 0).x, glm::vec3(0, 1, 0));
			View = glm::translate(View, glm::vec3(posX, -posY, -posZ));
			glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0));
			glm::mat4 mvp = Projection * View * Model;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
			glEnableVertexAttribArray(vertexPositionId);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(vertexPositionId, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDrawArrays(GL_TRIANGLES, 0, obj.getVertices().size());
			glDisableVertexAttribArray(vertexPositionId);
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
