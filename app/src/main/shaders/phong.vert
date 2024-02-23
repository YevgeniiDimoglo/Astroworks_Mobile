#version 450

layout(set = 0, binding = 0) uniform UBOScene
{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 lightMVP;
    vec4 lightDirection;
    vec4 lightColor;
    vec4 viewPos;
    vec4 timerConstants;
} uboScene;

layout(push_constant) uniform PushModel{
      mat4 model;
      vec4 baseColor;
      vec4 timer;
	  vec4 additionalValues;
} pushModel;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outVertPos;
layout(location = 3) out vec4 outTangent;

layout(location = 4) out vec3 outLightDirection;
layout(location = 5) out vec4 outLightColor;
layout(location = 6) out vec4 outBaseColor;
layout(location = 7) out vec4 outFragPosLightSpace;

layout(location = 8) out vec4 outCameraPos;
layout(location = 9) out vec4 outTimerConstants;


const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() 
{
    outNormal = inNormal;
    outBaseColor = pushModel.baseColor;
    outUV = inUV;
    outTangent = inTangent;
    gl_Position = uboScene.projection * uboScene.view * pushModel.model * vec4(inPos, 1.0);
    
    vec4 vertPos4 = pushModel.model * vec4(inPos, 1.0);
    outVertPos = vertPos4.xyz / vertPos4.w;
    outLightDirection = vec3(uboScene.lightDirection);
    outLightColor = uboScene.lightColor;
    outFragPosLightSpace = biasMat * uboScene.lightMVP * vec4(outVertPos, 1.0);

    outNormal = mat3(pushModel.model) * inNormal;

    outCameraPos = uboScene.viewPos;
    outTimerConstants = pushModel.timer;
}