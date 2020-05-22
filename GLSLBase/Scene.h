#pragma once

#include <memory>
#include <unordered_map>
#include <list>

#include "Dependencies\glew.h"
#include "UniformBuffer.hpp"

#define MAX_LIGHT 3

enum RenderLayer
{
	Opaque = 0,
	AlphaTested,
	Transparent,
	Count,
};

struct Material;
class StaticMesh;
class Shader;
class Camera;
class Actor;
class Light;

class Scene
{
public:
	Scene();
	~Scene();

public:
	void Initialize(const int windowWidth, const int windowHeight);
	void Update(float deltaTime);
	void Render(const RenderLayer renderLayer, Shader* shader) const;
	void RenderLightVolume(Shader* shader, Light* light);
	void BindShadowMapArray(Shader* shader) const;

	Camera* GetCamera() const;
	void GetLights(std::vector<Light*>& lights) const;

private:
	void CreateStaticMeshes();
	void CreateTextures();
	void CreateMaterials();
	void CreateLights();
	void CreateShadowMapArray();
	void CreateObjects();

	void UpdatePassConstants(float deltaTime);
	
private:
	static GLuint CreatePngTexture(const char* filePath,
		GLint wrapFlag = GL_REPEAT, GLfloat minFlag = GL_LINEAR, bool applyAnistropic = false);

private:
	std::unique_ptr<Camera> m_Camera = nullptr;
	std::unique_ptr<UniformBuffer<PassConstants>> m_PassUniformBuffer = nullptr;
	std::unique_ptr<UniformBuffer<LightConstants>> m_LightUniformBuffer = nullptr;

	std::list<std::shared_ptr<Actor>> m_Actors[RenderLayer::Count];
	std::list<std::unique_ptr<Light>> m_Lights;

	std::unordered_map<std::string, GLuint> m_Textures;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
	std::unordered_map<std::string, std::unique_ptr<StaticMesh>> m_StaticMeshes;

	GLuint m_ShadowMapClearFBO;
	GLuint m_ShadowMapArray;
	GLuint m_ShadowCubeMapArray;
};

