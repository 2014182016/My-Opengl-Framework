#include "stdafx.h"
#include "Actor.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Shader.h"

Actor::Actor() : Object() { }

Actor::~Actor() { }

void Actor::Bind(Shader* shader) const
{
	if (m_Material != nullptr)
	{
		m_Material->Bind(shader);
	}

	glUniformMatrix4fv(0, 1, GL_FALSE, &m_World[0][0]);
}

void Actor::Render() const
{
	if (m_Mesh == nullptr)
		return;

	m_Mesh->Render();
}