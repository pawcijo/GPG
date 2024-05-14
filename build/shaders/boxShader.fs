#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D funnyTexture;

uniform bool isSelected;
uniform bool isColliding;

void main()
{
    vec4 texColor = texture(funnyTexture, TexCoord);
 
        vec3 yellow = vec3(1.0,1.0,0.0);
        vec3 red = vec3(1.0,0.0,0.0);
        vec3 orange = vec3(1.0,0.45,0.0);
        vec4 mixedColor  = mix(texColor, vec4(yellow, 1.0), 0.5);

        if(isSelected && isColliding)
        {
         mixedColor  = mix(texColor, vec4(orange, 1.0), 0.5);
         FragColor =  mixedColor;
        }
        else if(isSelected && !isColliding) 
        {
            mixedColor  = mix(texColor, vec4(yellow, 1.0), 0.5);
            FragColor =  mixedColor;
        }
        else if(isColliding)
        {
             mixedColor  = mix(texColor, vec4(red, 1.0), 0.5);
             FragColor =  mixedColor;
        }
        else
        {
            FragColor = texColor;   
        }
       
 
}

