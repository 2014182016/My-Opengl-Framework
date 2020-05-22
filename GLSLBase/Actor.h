#pragma once

#include "Object.h"

struct Material;
class Shader;
class StaticMesh;

class Actor : public Object
{
public:
	Actor();
	~Actor();

public:
	// ½¦ÀÌ´õ¿¡ ¹­À» uniform º¯¼ö¸¦ ¿©±â¼­ ¹­´Â´Ù.
	virtual void Bind(Shader* shader) const;

public:
	void Render() const;

public:
	Material* m_Material = nullptr;
	StaticMesh* m_Mesh = nullptr;

};

