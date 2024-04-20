#pragma once
#include "stdafx.h"

struct VertexBase abstract {};

struct Vertex : public VertexBase
{
	Vertex() = default;
	Vertex(XMFLOAT3 position) :
		position{position} {}
	XMFLOAT3 position;
};

struct TextureVertex : public VertexBase
{
	TextureVertex() = default;
	TextureVertex(XMFLOAT3 position, XMFLOAT2 uv) : 
		position{ position }, uv{ uv } {}
	XMFLOAT3 position;
	XMFLOAT2 uv;
};

struct TerrainVertex : public VertexBase
{
	TerrainVertex() = default;
	TerrainVertex(XMFLOAT3 position, XMFLOAT2 uv0, XMFLOAT2 uv1, UINT density) :
		position{ position }, uv0{ uv0 }, uv1{ uv1 }, density{ density } {}
	XMFLOAT3 position;
	XMFLOAT2 uv0;
	XMFLOAT2 uv1;
	UINT density;
};