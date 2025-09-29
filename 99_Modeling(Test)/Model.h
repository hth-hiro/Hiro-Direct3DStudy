#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#include <directxtk/SimpleMath.h>

#include "ImGuiManager.h"

using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

struct Vertex
{
    Vector3 Pos;
    Vector3 Normal;
    Vector2 Tex; 
    Vector3 Tangent;
};

struct Model
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;
};