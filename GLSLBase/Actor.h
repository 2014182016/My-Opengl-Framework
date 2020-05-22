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
	// ���̴��� ���� uniform ������ ���⼭ ���´�.
	virtual void Bind(Shader* shader) const;

public:
	void Render() const;

public:
	Material* m_Material = nullptr;
	StaticMesh* m_Mesh = nullptr;

};

