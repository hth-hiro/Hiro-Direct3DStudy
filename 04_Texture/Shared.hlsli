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
    float4 vLightColor;
    float4 vOutputColor;
    
    //float3 cameraPos;
    //float padding;
}

cbuffer SkyboxCB : register(b1)
{
    matrix SkyboxView;
    matrix SkyboxProjection;
}

// ���� �ٸ� �ؽ�ó�� ���÷��� ����ؾ� �ȼ����̴����� ���� ������ ����
Texture2D txDiffuse : register(t0);
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
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};





