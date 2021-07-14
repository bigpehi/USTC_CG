// #version 330 core
// out vec4 FragColor;

// in vec2 TexCoords;

// uniform sampler2D texture_diffuse1;

// void main()
// {    
//     // GLSL内建的texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标
//     FragColor = texture(texture_diffuse1, TexCoords);
// }

#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform sampler2D texture_diffuse1;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec4 objectColor = texture(texture_diffuse1, TexCoords);
    FragColor = (vec4(ambient,1.0) + vec4(diffuse,1.0)) * objectColor;
    // FragColor = objectColor;
    // vec4 result = vec4(0.1,0.1,0.1,0.1)* texture(texture_diffuse1, TexCoords);
    // FragColor = result;
}