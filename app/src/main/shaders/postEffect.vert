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

out gl_PerVertex
{
	vec4 gl_Position;
};

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 time;

void main() 
{
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	time = pushModel.timer;
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}