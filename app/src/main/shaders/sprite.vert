#version 450

layout(push_constant) uniform PushModel{
      mat4 model;
      vec4 baseColor;
      vec4 timer;
	  vec4 additionalValues;
} pushModel;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = pushModel.model * vec4(inPosition, 0.0, 1.0);
    fragColor = pushModel.baseColor;
    fragTexCoord = inTexCoord;
}