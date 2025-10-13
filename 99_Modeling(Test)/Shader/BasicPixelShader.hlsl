#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
     // ÅØ½ºÃ³ »ùÇÃ¸µ
    float4 surface;
    
    if (hasTexrue_solidColor.x <= 0.5f)
    {
        surface = txDiffuse.Sample(samLinear, input.Tex);
    }
    else
    {
        //surface = float4(hasTexture_solidColor.yzw, 1.0f);
    }
    
    float4 txNormal = normalMap.Sample(samLinear, input.Tex);
    float4 txSpecular = specularMap.Sample(samLinear, input.Tex);
    float4 txEmissive = emissiveMap.Sample(samLinear, input.Tex);
    
    float3 normalVector = normalize(input.Norm);                                // ³ë¸Ö º¤ÅÍ (N)      
    //float3 normalVector = DecodeNormal(txNormal.rgb);
    float3 lightVector = normalize(vLightDir.xyz);                              // ºûÀÇ ¹æÇâ (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector));       // ¹Ý»ç º¤ÅÍ (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos);              // ºä º¤ÅÍ (V)
    float3 harfVector = normalize(-lightVector + viewVector);                   // ÇÏÇÁ º¤ÅÍ (H)
    
    float4 ambient =
    vAmbientColor 
    * vMaterialAmbient
    * surface;
    
    float4 diffuse =
    vDiffuseColor
    * vMaterialDiffuse 
    * surface
    * saturate(dot(normalVector, -lightVector));
    
    // ºí¸° Æþ °è»ê
    float4 specular =
    vSpecularColor
    * vMaterialSpecular 
    //* txSpecular.r
    * pow(saturate(dot(normalVector, harfVector)), (float) vShininess);
    
    // Æþ °è»ê
    //float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess);

    if (UseLighting.x > 0.5f)
    {
        finalColor = saturate(ambient + diffuse + specular);
        finalColor.a = surface.a;
    }
    else
    {
        finalColor = surface;
    }
    
    return finalColor += txEmissive;
}