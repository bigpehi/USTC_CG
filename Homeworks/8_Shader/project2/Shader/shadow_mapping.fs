#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos; // 顶点在世界空间中的位置
    vec3 Normal; // 顶点的法向量
    vec2 TexCoords; // 顶点的纹理坐标
    vec4 FragPosLightSpace; // 顶点在光空间中的位置
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap; // 深度纹理

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 顶点在光空间中的位置需要手动执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace); // 片段在阴影中shadow为1，否则为0
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; // 如果是阴影则结果为ambient*color，仅环境光，否则结果使用Blinn-Phong光照模型计算

    FragColor = vec4(lighting, 1.0f);
}