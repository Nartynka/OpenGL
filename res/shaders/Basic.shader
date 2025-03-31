#shader vertex
#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = aPosition;
	ourColor = aColor;
	TexCoord = aTexCoord;
};

#shader fragment
#version 330 core

//uniform vec4 u_Color;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); 
};
