#version 450


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;

out gl_PerVertex
{
	vec4 gl_Position;
};

const vec2 kVertices[4] = vec2[]
(
 vec2(-1.0,  1.0),
 vec2(-1.0, -1.0),
 vec2( 1.0,  1.0),
 vec2( 1.0, -1.0)
 );

layout (location = 0) out vec2 outUV;

void main() 
{
    outUV = kVertices[gl_VertexIndex].xy / 2.0 + 0.5;
	gl_Position = vec4(kVertices[gl_VertexIndex], 0.0f, 1.0f);
}