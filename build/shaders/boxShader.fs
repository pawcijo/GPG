#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D funnyTexture;

uniform bool isSelected;

void main()
{
    vec4 texColor = texture(funnyTexture, TexCoord);
    if(isSelected)
    {
        vec3 yellow = vec3(1.0,1.0,0.0);
        
        vec4 mixedColor  = mix(texColor, vec4(yellow, 1.0), 0.5);
        FragColor = mixedColor;
    }
    else{
        FragColor = texColor;
    }
}

