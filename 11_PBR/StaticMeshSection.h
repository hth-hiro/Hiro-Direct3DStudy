#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include "VertexTypes.h"
#include "../Common/Helper.h"

using Microsoft::WRL::ComPtr;

// 메시 안에서 하나의 렌더 단위(SubMesh)를 담당

class StaticMeshSection
{
public:
    std::vector<Vertex> Vertices;
    std::vector<WORD> Indices;
    int materialIndex;
    ComPtr<ID3D11Buffer> IndexBuffer;
    ComPtr<ID3D11Buffer> VertexBuffer;

    void Create(ID3D11Device* device);
};

