#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT_SKYBOX input) : SV_Target
{
    return txCube.Sample(samLinear, input.Tex);
}