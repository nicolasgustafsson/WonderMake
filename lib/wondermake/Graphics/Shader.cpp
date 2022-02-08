#include "pch.h"
#include "Shader.h"
#include "Resources/AssetDatabase.h"

_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Fragment>>, Shader_Fragment);
_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Geometry>>, Shader_Geometry);
_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Vertex>>, Shader_Vertex);

_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Fragment>>, Shader_FragmentAsset);
_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Geometry>>, Shader_GeometryAsset);
_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Vertex>>, Shader_VertexAsset);
