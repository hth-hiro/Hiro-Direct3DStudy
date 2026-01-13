//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

#define PI 3.14159265359
#define EPSILON  1e-6f

//Shared.hlsli
SamplerState samLinear : register(s0);
SamplerState ShadowSampler : register(s1);
SamplerState LinearSamp : register(s2);
SamplerState samplerPoint : register(s3);

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    
    float4 vLightDir;
    float4 vOutputColor;
    
    float4 vAmbientColor;
    float4 vDiffuseColor;
    float4 vSpecularColor;
    
    float4 vMaterialAmbient;
    float4 vMaterialDiffuse;
    float4 vMaterialSpecular;
    
    float4 cameraPos;
    
    float4 vShininess;
    
    int UseLighting;
    int UseTexture;
    int UseCustomAlbedo;
    int UseIBL;
    
    int hasTexture;
    int hasNormalMap;
    int hasSpecularMap;
    int hasEmissiveMap;
    
    float4 solidColor;
    
    int hasMetallicMap;
    int hasRoughnessMap;
    uint gIsRigid;
    uint gRefBoneIndex;
    
    matrix ShadowView;
    matrix ShadowProjection;
    
    float roughness;
    float metallic;
    float ao;
    float gamma;
    
    float4 albedo;
}

cbuffer SkyboxCB : register(b1)
{
    matrix SkyboxView;
    matrix SkyboxProjection;
}

cbuffer ModelMatrix : register(b2)
{
    matrix gModelMatrices[256];
}

cbuffer BonePoseMatrix : register(b3)
{
    matrix gBonePose[128];
}

cbuffer BoneOffsetMatrix : register(b4)
{
    matrix gBoneOffset[128];
}

cbuffer ToneMapCB : register(b5)
{
    float Exposure;
    float Gamma;
    float2 pad;
}

cbuffer CBGeometry : register(b6)
{
    matrix GeometryWorld;
    float4 GeometryColor;
}

cbuffer CBDirectionalLight : register(b7)
{
    float4 gDirLightDirectionWS; // w = intensity
    float4 gDirLightColor;
}

cbuffer CBFrame : register(b8)
{
    matrix GeoMetryView;
    matrix GeoMetryProjection;
}

// SkyBox
TextureCube txCube : register(t0);

// Material
Texture2D txDiffuse : register(t1);
Texture2D normalMap : register(t2);
Texture2D specularMap : register(t3);
Texture2D emissiveMap : register(t4);
Texture2D metallicMap : register(t5);
Texture2D roughnessMap : register(t6);

// Shadow
Texture2D<float> txShadow : register(t7);

// IBL
TextureCube txIBL_Diffuse : register(t10);
TextureCube txIBL_Specular : register(t11);
Texture2D txIBL_Specular_LUT : register(t12);

// ToneMapping
Texture2D SceneHDR : register(t13);

// Deferred Shading
Texture2D gGBufferBaseColor : register(t20);
Texture2D gGBufferNormal : register(t21);
Texture2D gGBufferPosition : register(t22);
Texture2D gDepthBuffer : register(t23);

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
    float4 PositionShadow : TEXCOORD5;
};

struct VS_INPUT_BONED
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    
    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHT;
};

//
struct VS_INPUT_TONEMAP
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT_TONEMAP
{
    float4 Pos : SV_Position;
    float2 Tex : TEXCOORD0;
};

// Deferred Shading
struct VS_INPUT_DEFERRED
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_INPUT_DEFERRED
{
    float4 positionCS : SV_Position;
    float3 normalWS : TEXCOORD0;
    float3 positionWS : TEXCOORD1;
    float2 uv : TEXTCOORD2;
};

struct PS_INPUT_QUAD
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct VS_INPUT_QUAD
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
};

float3 EncodeNormal(float3 N)
{
    return N * 0.5 + 0.5;
}

float3 DecodeNormal(float3 N)
{
    return N * 2 - 1;
}

// 입력 : Linear 공간의 HDR RGB 색상값
// 출력 : 0.0 ~ 1.0 범위의 압축된 선형 RGB값 (float3)
float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate(x * (a * x + b) / (x * (c * x + d) + e));
}


