#include "stdafx.h"
#include "Scene.h"
#include "Renderer.h"
#include "Shader.h"
#include "StaticMesh.h"
#include "Camera.h"
#include "Actor.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Debug.h"

Scene::Scene() { }

Scene::~Scene() 
{
	for (int i = 0; i < RenderLayer::Count; ++i)
		m_Actors[i].clear();
	m_Lights.clear();
	m_Materials.clear();
	m_StaticMeshes.clear();

	for (auto texture : m_Textures)
		glDeleteTextures(1, &texture.second);
	m_Textures.clear();

	glDeleteTextures(1, &m_ShadowMapArray);
	glDeleteTextures(1, &m_ShadowCubeMapArray);
}

Camera* Scene::GetCamera() const
{
	return m_Camera.get();
}

void Scene::Initialize(const int windowWidth, const int windowHeight)
{
	// Create Uniform Buffers
	m_PassUniformBuffer = std::make_unique<UniformBuffer<PassConstants>>(0, 1, GL_STREAM_DRAW);
	m_LightUniformBuffer = std::make_unique<UniformBuffer<LightConstants>>(1, MAX_LIGHT, GL_DYNAMIC_DRAW);

	// Create Camera
	m_Camera = std::make_unique<Camera>();
	m_Camera->SetLens(60.0f, static_cast<float>(windowWidth) / windowHeight, 0.1f, 1000.0f);
	m_Camera->SetPosition(22.73f, 2.62f, 63.63f);
	m_Camera->RotateY(60.0f);
	m_Camera->Pitch(-23.0f);

	// Create Program Objects
	CreateStaticMeshes();
	CreateTextures();
	CreateMaterials();
	CreateLights();
	CreateShadowMapArray();
	CreateObjects();
}

void Scene::CreateStaticMeshes()
{
	m_StaticMeshes["Cube"] = std::make_unique<StaticMesh>("Cube.obj");
	m_StaticMeshes["Sphere"] = std::make_unique<StaticMesh>("Sphere.obj");
	m_StaticMeshes["Cone"] = std::make_unique<StaticMesh>("Cone.obj");
	m_StaticMeshes["Building"] = std::make_unique<StaticMesh>("Building.obj");
	m_StaticMeshes["TableTop"] = std::make_unique<StaticMesh>("TableTop.obj");
	m_StaticMeshes["TableBottom"] = std::make_unique<StaticMesh>("TableBottom.obj");
	m_StaticMeshes["Chair"] = std::make_unique<StaticMesh>("Chair.obj");
	m_StaticMeshes["Candle"] = std::make_unique<StaticMesh>("Candle.obj");
}

void Scene::CreateTextures()
{
	m_Textures["Wood_D"] = Scene::CreatePngTexture("Textures/Wood_D.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood_N"] = Scene::CreatePngTexture("Textures/Wood_N.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood_R"] = Scene::CreatePngTexture("Textures/Wood_R.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood_AO"] = Scene::CreatePngTexture("Textures/Wood_AO.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Table_D"] = Scene::CreatePngTexture("Textures/Table_D.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Table_N"] = Scene::CreatePngTexture("Textures/Table_N.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Table_R"] = Scene::CreatePngTexture("Textures/Table_R.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Table_AO"] = Scene::CreatePngTexture("Textures/Table_AO.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Metal_D"] = Scene::CreatePngTexture("Textures/Metal_D.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Metal_N"] = Scene::CreatePngTexture("Textures/Metal_N.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Metal_R"] = Scene::CreatePngTexture("Textures/Metal_R.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Metal_M"] = Scene::CreatePngTexture("Textures/Metal_M.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood2_D"] = Scene::CreatePngTexture("Textures/Wood2_D.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood2_N"] = Scene::CreatePngTexture("Textures/Wood2_N.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Wood2_R"] = Scene::CreatePngTexture("Textures/Wood2_R.png", GL_REPEAT, GL_LINEAR, true);
	m_Textures["Candle"] = Scene::CreatePngTexture("Textures/Candle.png", GL_REPEAT, GL_LINEAR, true);
}

void Scene::CreateMaterials()
{
	std::unique_ptr<Material> material;

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 1.0f;
	material->m_Metallic = 0.0f;
	material->m_DiffuseMap = m_Textures["Wood_D"];
	material->m_NormalMap = m_Textures["Wood_N"];
	material->m_RoughnessMap = m_Textures["Wood_R"];
	material->m_AOMap = m_Textures["Wood_AO"];
	m_Materials["Wood"] = std::move(material);

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 1.0f;
	material->m_Metallic = 0.0f;
	material->m_DiffuseMap = m_Textures["Table_D"];
	material->m_NormalMap = m_Textures["Table_N"];
	material->m_RoughnessMap = m_Textures["Table_R"];
	material->m_AOMap = m_Textures["Table_AO"];
	m_Materials["Table"] = std::move(material);

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 1.0f;
	material->m_Metallic = 1.0f;
	material->m_DiffuseMap = m_Textures["Metal_D"];
	material->m_NormalMap = m_Textures["Metal_N"];
	material->m_RoughnessMap = m_Textures["Metal_R"];
	material->m_MetallicMap = m_Textures["Metal_M"];
	m_Materials["Metal"] = std::move(material);

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 1.0f;
	material->m_Metallic = 0.0f;
	material->m_DiffuseMap = m_Textures["Wood2_D"];
	material->m_NormalMap = m_Textures["Wood2_N"];
	material->m_RoughnessMap = m_Textures["Wood2_R"];
	m_Materials["Wood2"] = std::move(material);

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 0.6f;
	material->m_Metallic = 0.0f;
	material->m_DiffuseMap = m_Textures["Candle"];
	m_Materials["Candle"] = std::move(material);

	material = std::make_unique<Material>();
	material->m_Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material->m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Emissive = glm::vec3(1.0f, 1.0f, 1.0f);
	material->m_Roughness = 0.6f;
	material->m_Metallic = 0.0f;
	m_Materials["Test"] = std::move(material);
}

void Scene::CreateLights()
{
	std::unique_ptr<DirectionalLight> dirLight;
	std::unique_ptr<SpotLight> spotLight;
	std::unique_ptr<PointLight> pointLight;

	dirLight = std::make_unique<DirectionalLight>(m_LightUniformBuffer.get());
	dirLight->SetStrength(glm::vec3(1.0f, 1.0f, 1.0f));
	dirLight->SetPosition(-50.0f, 10.0f, 0.0f);
	dirLight->SetRotation(60.0f, 70.0f, 0.0f);
	dirLight->SetEnabledLightShaft(false);
	m_Lights.push_back(std::move(dirLight));
}

void Scene::CreateShadowMapArray()
{
	const float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };

	int shadowMapIndex = 0;
	int shadowCubeMapIndex = 0;
	for (const auto& light : m_Lights)
	{
		switch (light->GetLightType())
		{
		case LightType::Directional:
			light->SetShadowMapIndex(&m_ShadowMapArray, shadowMapIndex);
			shadowMapIndex += 3;
			break;
		case LightType::Spot:
			light->SetShadowMapIndex(&m_ShadowMapArray, shadowMapIndex);
			shadowMapIndex += 1;
			break;
		case LightType::Point:
			light->SetShadowMapIndex(&m_ShadowCubeMapArray, shadowCubeMapIndex);
			shadowCubeMapIndex += 1;
			break;
		}
	}

	glGenTextures(1, &m_ShadowMapArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMapArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16, SHADOWMAP_SIZE, SHADOWMAP_SIZE,
		shadowMapIndex, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glGenTextures(1, &m_ShadowCubeMapArray);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_ShadowCubeMapArray);
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT16, SHADOWMAP_SIZE, SHADOWMAP_SIZE,
		shadowCubeMapIndex * 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &m_ShadowMapClearFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapClearFBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::CreateObjects()
{
	std::unique_ptr<Actor> actor;

	actor = std::make_unique<Actor>();
	actor->m_Material = m_Materials["Wood"].get();
	actor->m_Mesh = m_StaticMeshes["Building"].get();
	actor->SetPosition(0.0f, 0.0f, 0.0f);
	actor->SetScale(0.8f, 0.8f, 0.8f);
	m_Actors[RenderLayer::Opaque].push_back(std::move(actor));

	actor = std::make_unique<Actor>();
	actor->m_Material = m_Materials["Table"].get();
	actor->m_Mesh = m_StaticMeshes["TableTop"].get();
	actor->SetPosition(6.0f, -14.0f, 12.0f);
	actor->SetScale(0.3f, 0.3f, 0.3f);
	m_Actors[RenderLayer::Opaque].push_back(std::move(actor));

	actor = std::make_unique<Actor>();
	actor->m_Material = m_Materials["Metal"].get();
	actor->m_Mesh = m_StaticMeshes["TableBottom"].get();
	actor->SetPosition(6.0f, -14.0f, 12.0f);
	actor->SetScale(0.3f, 0.3f, 0.3f);
	m_Actors[RenderLayer::Opaque].push_back(std::move(actor));

	actor = std::make_unique<Actor>();
	actor->m_Material = m_Materials["Wood2"].get();
	actor->m_Mesh = m_StaticMeshes["Chair"].get();
	actor->SetPosition(13.5f, -12.0f, 7.0f);
	actor->SetScale(1.3f, 1.3f, 1.3f);
	actor->SetRotation(0.0f, -75.0f, 0.0f);
	m_Actors[RenderLayer::Opaque].push_back(std::move(actor));
}

GLuint Scene::CreatePngTexture(const char* filePath, GLint wrapFlag, GLfloat minFlag, bool applyAnistropic)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(error == 0);
		return -1;
	}

	GLuint temp;

	// 텍스처 ID를 생성한다.
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);

	// 텍스처 상태를 정의한다.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFlag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFlag);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFlag);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFlag);

	if (applyAnistropic)
	{
		// 비등방형 필터링을 설정한다.
		GLfloat anisotropicCount;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropicCount);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicCount);
	}

	// 로드된 데이터를 사용하여 텍스처를 생성한다.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	// 밉맵을 생성한다.
	glGenerateMipmap(temp);

	return temp;
}

void Scene::UpdatePassConstants(float deltaTime)
{
	glm::mat4 view = m_Camera->GetView();
	glm::mat4 proj = m_Camera->GetProj();

	PassConstants passConstatns;

	passConstatns.m_View = view;
	passConstatns.m_InvView = glm::inverse(view);
	passConstatns.m_Proj = proj;
	passConstatns.m_InvProj = glm::inverse(proj);
	passConstatns.m_ViewProj = proj * view;
	passConstatns.m_InvViewProj = glm::inverse(passConstatns.m_ViewProj);
	passConstatns.m_Ambient = glm::vec4(0.02f, 0.02f, 0.02f, 0.0f);
	passConstatns.m_EyePos = m_Camera->GetPosition();
	passConstatns.m_DelatTime = deltaTime;
	passConstatns.m_ScreenDimension = Renderer::GetScreenDimension();
	passConstatns.m_InvScreenDimension = glm::vec2(1.0f / passConstatns.m_ScreenDimension.x, 1.0f / passConstatns.m_ScreenDimension.y);
	passConstatns.m_InvHalfScreenDimension = 
		glm::vec2(1.0f / (passConstatns.m_ScreenDimension.x / 2.0f), 1.0f / (passConstatns.m_ScreenDimension.y / 2.0f));
	passConstatns.m_Near = m_Camera->GetNear();
	passConstatns.m_Far = m_Camera->GetFar();

	m_PassUniformBuffer->Bind(passConstatns);
}

void Scene::Update(float deltaTime)
{
	UpdatePassConstants(deltaTime);

	for (int i = 0; i < RenderLayer::Count; ++i)
	{
		for (const auto& actor : m_Actors[i])
		{
			actor->Update(deltaTime);
		}
	}

	// ShadowMap Clear
	{
		glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapClearFBO);
		glDepthMask(GL_TRUE);
		glClearDepth(1.0f);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMapArray, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowCubeMapArray, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	for (const auto& light : m_Lights)
	{
		light->Update(deltaTime);
	}

	m_Camera->UpdateViewMatrix();
}

void Scene::Render(const RenderLayer renderLayer, Shader* shader) const
{
	for (const auto& actor : m_Actors[renderLayer])
	{
		actor->Bind(shader);
		actor->Render();
	}
}

void Scene::RenderLightVolume(Shader* shader, Light* light)
{
	switch (light->GetLightType())
	{
	case LightType::Directional:
		shader->SetUniform("g_World", light->GetLightVolumeTransform());
		m_StaticMeshes["Cube"]->Render();
		break;
	case LightType::Spot:
		shader->SetUniform("g_World", light->GetLightVolumeTransform());
		m_StaticMeshes["Cone"]->Render();
		break;
	case LightType::Point:
		shader->SetUniform("g_World", light->GetLightVolumeTransform());
		m_StaticMeshes["Sphere"]->Render();
		break;
	}
}

void Scene::GetLights(std::vector<Light*>& lights) const
{
	for (const auto& light : m_Lights)
	{
		lights.push_back(light.get());
	}
}

void Scene::BindShadowMapArray(Shader* shader) const
{
	shader->BindTexture("g_ShadowMapArray", 16, m_ShadowMapArray, GL_TEXTURE_2D_ARRAY);
	shader->BindTexture("g_ShadowCubeMapArray", 17, m_ShadowCubeMapArray, GL_TEXTURE_CUBE_MAP_ARRAY);
	shader->SetUniform("g_CascadeDists", DirectionalLight::cascadeDists[0], NUM_CASCADES + 1);
	shader->SetUniform("g_FarPlane", m_Camera->GetFar());
	shader->SetUniform("g_NearPlane", m_Camera->GetNear());
}