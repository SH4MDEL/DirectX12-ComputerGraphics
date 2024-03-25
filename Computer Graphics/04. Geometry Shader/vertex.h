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

struct DetailVertex : public VertexBase
{
	DetailVertex() = default;
	DetailVertex(XMFLOAT3 position, XMFLOAT2 uv0, XMFLOAT2 uv1) : 
		position{ position }, uv0{ uv0 }, uv1{ uv1 } {}
	XMFLOAT3 position;
	XMFLOAT2 uv0;
	XMFLOAT2 uv1;
};