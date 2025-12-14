#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // World-space position
    fragPos = vec3(model * vec4(aPos, 1.0));

    // World-space normal (already correct)
    fragNormal = mat3(transpose(inverse(model))) * aNormal;

    texCoord = aTexCoord;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}