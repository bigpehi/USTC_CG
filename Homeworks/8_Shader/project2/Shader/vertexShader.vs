#version 330 core

layout (location = 0) in vec3 mPosition;

out vec4 mOutColor;

uniform mat4 transformMatrix;

void main()
{
    gl_Position = transformMatrix * vec4(mPosition.x,mPosition.y,mPosition.z, 1.0);

    mOutColor = vec4(mPosition.x + 0.5,mPosition.y + 0.2,mPosition.z, 1.0);
}