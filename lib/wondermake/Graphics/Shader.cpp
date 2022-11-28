#include "pch.h"
#include "Shader.h"
#include "Resources/AssetDatabase.h"

_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Fragment>>, Shader_Fragment);
_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Geometry>>, Shader_Geometry);
_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Vertex>>, Shader_Vertex);

_WM_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Fragment>>, Shader_FragmentAsset);
_WM_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Geometry>>, Shader_GeometryAsset);
_WM_REGISTER_SYSTEM_IMPL(AssetDatabase<Shader<EShaderType::Vertex>>, Shader_VertexAsset);
