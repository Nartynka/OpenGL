#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream file(filepath);

	if (!file.good())
	{
		std::cout << "Error opening file!" << std::endl;
		return {};
	}
		


	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(file, line))
	{
		// if line is a start of a shader
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;

			}
		}
		else // if line is part of a shader
		{
			ss[(int)type] << line << "\n";
		}

	}

	file.close();

	return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();

	// Compile two shader into one
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// "Link" shaders
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


void FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "HEHEHE", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);

	// "vsync"
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	
	glEnable(GL_DEPTH_TEST);

	// 36 vertices (6 faces * 2 triangles * 3 vertices)
	float cube[] = {
	// position			   // color			   // texture coords
	-0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,   0.4f, 0.6f, 1.0f,   0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.1f,   1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,   0.4f, 1.0f, 0.6f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,   0.4f, 0.6f, 1.0f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,   0.8f, 0.5f, 0.5f,   0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.1f,   0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = 
	{
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// vertex array object
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Buffer for our positions
	unsigned int buffer;
	glGenBuffers(1, &buffer); // create a buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // if we want to change something about the buffer, we have to first "select it" aka. bind it
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	// vertex attribute that will be passed into the vertex shader
	// we have three attributes - position of a single vertex that is represented by 7 floats, color (3 floats) and texture coords (2 floats)
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shaderId = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shaderId);
	

	stbi_set_flip_vertically_on_load(1);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("res/sysop.gif", &width, &height, &nrChannels, 0);
	
	unsigned int texture;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	unsigned int modelLoc = glGetUniformLocation(shaderId, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderId, "view");
	unsigned int projectionLoc = glGetUniformLocation(shaderId, "projection");

	unsigned int transformLoc = glGetUniformLocation(shaderId, "transform");

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		/* Render here */
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		// vao stores the binds so we don't have to bind them here, just binding the vao will suffice
		//glBindBuffer(GL_ARRAY_BUFFER, buffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection;

		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		for (auto cubePosition : cubePositions)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePosition);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shaderId);

	glfwTerminate();
	return 0;
}

