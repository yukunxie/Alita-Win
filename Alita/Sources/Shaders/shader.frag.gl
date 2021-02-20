#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_450pack : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(texCoord, 0.0, 1.0);
    outColor = texture(texSampler, texCoord * 2.0) * vec4(fragColor, 1.0);
}
