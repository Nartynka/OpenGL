#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
	float positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f, 0.5f,
		-0.5f, 0.5f,
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
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	// vertex attribute that will be passed into the vertex shader
	// we have only one attribute - position of a single vertex that is represented by two floats
	glEnableVertexAttribArray(0);
	// this line actually binds/links the current bind buffer to the vao
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // structure of a vertex buffer (each vertex consists of 2 floats and etc.)

	// we use index buffer to eliminate the need to store some vertices twice 
	// (rectangle is made of to triangles next to each other so two vertices would be stored twice and that is not good for memory)
	unsigned int ibo; // index/element buffer object
	glGenBuffers(1, &ibo); // create a buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // "select" buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);


	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");

	float r = 0.0f;
	float increment = 0.01f;
	
	// clear all gl states
	glBindVertexArray(0); // vao stores binding and unbinding so if we want to just clear states, we have to unbind vao first. Otherwise vao will store the unbinds and we will have to bind everything again before draw.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
	glUseProgram(0);
	// "wire frame mode"
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.5f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// vao stores the binds so we don't have to bind them here, just binding the vao will suffice
		//glBindBuffer(GL_ARRAY_BUFFER, buffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		glBindVertexArray(vao);
		glUseProgram(shader);
	
		float time = glfwGetTime();
		float g = sin(time) / 2.0f + 0.5f;

		glUniform4f(location, r, g, 0.8f, 1.f);

		// Draw the triangle
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		if (r > 1.0f)
			increment = -0.01f;
		else if (r < 0.0f)
			increment = 0.01f;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}