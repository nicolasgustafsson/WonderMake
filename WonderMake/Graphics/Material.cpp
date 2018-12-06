#include "stdafx.h"
#include "Material.h"
#include "Texture.h"
#include "../Utilities/Utility.h"

Material::Material(std::vector<Shader<EShaderType::Fragment>*> InFragmentShaders, std::vector<Texture*> InTextures)
	:FragmentShaders(InFragmentShaders), Textures(InTextures)
{

}

void Material::BindTextures()
{
	for (auto [i, Texture] : Utility::Enumerate(Textures))
	{
		Texture->Bind(i);
	}
}
