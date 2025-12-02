#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include "VertexTypes.h"

using Microsoft::WRL::ComPtr;

// 메시 안에서 하나의 렌더 단위(SubMesh)를 담당

class StaticMeshSection
{
public:
    std::vector<Vertex> Vertices;
    std::vector<WORD> Indices;
    int materialIndex;
    ID3D11Buffer* IndexBuffer = nullptr;
    ID3D11Buffer* VertexBuffer = nullptr;

    void Create(ID3D11Device* device);
};

