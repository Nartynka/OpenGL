#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void LoadAndCreateTexture(const std::string& filepath)
{

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

	// "vsync"
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "ERROR!!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	
	// vertices positions
	//float positions[] = {
	//	-0.5f, -0.5f,
	//	 0.5f, -0.5f,
	//	 0.5f, 0.5f,
	//	-0.5f, 0.5f,
	//};

	float vertices[] = {
		// positions   // colors          // texture coords
		-0.5f, -0.5f,  1.0f, 0.5f, 0.1f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.4f, 1.0f, 0.6f,  1.0f, 0.0f,
		 0.5f, 0.5f,   0.4f, 0.6f, 1.0f,  1.0f, 1.0f,
		-0.5f, 0.5f,   0.8f, 0.5f, 0.5f,  0.0f, 1.0f
	};

	
	// indices of positions of vertices of two triangles that will make a rectangle
	// counterclockwise drawing
	unsigned int indices[]
	{
		0, 1, 2,
		2, 3, 0
	};

	// vertex array object
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Buffer for our positions
	unsigned int buffer;
	glGenBuffers(1, &buffer); // create a buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // if we want to change something about the buffer, we have to first "select it" aka. bind it
	// 4 * 2 -> 4 vertices, 2 floats each
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * 3 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

	// vertex attribute that will be passed into the vertex shader
	// we have only one attribute - position of a single vertex that is represented by two floats
	glEnableVertexAttribArray(0);
	// this line actually binds/links the current bind buffer to the vao
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0); // structure of a vertex buffer (each vertex consists of 2 floats and etc.)

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));


	// we use index buffer to eliminate the need to store some vertices twice 
	// (rectangle is made of to triangles next to each other so two vertices would be stored twice and that is not good for memory)
	unsigned int ibo; // index/element buffer object
	glGenBuffers(1, &ibo); // create a buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // "select" buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);
	
	// clear all gl states
	//glBindVertexArray(0); // vao stores binding and unbinding so if we want to just clear states, we have to unbind vao first. Otherwise vao will store the unbinds and we will have to bind everything again before draw.
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
	//glUseProgram(0);
	// "wireframe mode"
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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

	
	unsigned int transformLoc = glGetUniformLocation(shader, "transform");

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// vao stores the binds so we don't have to bind them here, just binding the vao will suffice
		//glBindBuffer(GL_ARRAY_BUFFER, buffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		
		glm::mat4 transform = glm::mat4(1.0f);
		float scaleAmount = (sin(glfwGetTime())+1.f) / 2.f;
		transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(vao);
		// Draw the triangle
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}