#pragma once
#include "Shader.h"

class Texture;

class Material
{
public:
	Material(std::vector<Shader<EShaderType::Fragment>*> InFragmentShaders, std::vector<Texture*> InTextures);

	void BindTextures();

	const std::vector<Shader<EShaderType::Fragment>*>& GetFragmentShaders() const { return FragmentShaders; }

private:
	std::vector<Shader<EShaderType::Fragment>*> FragmentShaders;
	std::vector<Texture*> Textures;
};

