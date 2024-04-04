#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D funnyTexture;
void main()
{
    FragColor = texture(funnyTexture, TexCoord);
}

