#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
	gl_Position = transform*vec4(aPosition, 1.0f);
	ourColor = aColor;
	TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); 
};
