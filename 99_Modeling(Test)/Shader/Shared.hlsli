//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//Shared.hlsli
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    
    float4 vLightDir;
    float4 vOutputColor;
    
    float4 vMaterialAmbient;
    float4 vMaterialDiffuse;
    float4 vMaterialSpecular;
    
    float4 vAmbientColor;
    float4 vDiffuseColor;
    float4 vSpecularColor;
    
    float4 cameraPos;
    
    float4 vShininess;
    
    float4 UseLighting;
}

cbuffer SkyboxCB : register(b1)
{
    matrix SkyboxView;
    matrix SkyboxProjection;
}

// 서로 다른 텍스처와 샘플러를 사용해야 픽셀셰이더에서 서로 간섭이 없음
Texture2D txDiffuse : register(t0);
Texture2D normalMap : register(t2);
Texture2D specularMap : register(t3);
SamplerState samLinear : register(s0);

TextureCube txCube : register(t1);
SamplerState samCube : register(s1);

//--------------------------------------------------------------------------------------

struct VS_INPUT_SKYBOX
{
    float4 Pos : POSITION;
};

struct PS_INPUT_SKYBOX
{
    float4 Pos : SV_POSITION;
    float3 Tex : TEXCOORD0;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : TEXCOORD2;
    float3 Norm : TEXCOORD0;
    float2 Tex : TEXCOORD1;
    float3 Tangent : TEXCOORD3;
    float3 Bitangent : TEXCOORD4;
};

float3 EncodeNormal(float3 N)
{
    return N * 0.5 + 0.5;
}

float3 DecodeNormal(float3 N)
{
    return N * 2 - 1;
}




