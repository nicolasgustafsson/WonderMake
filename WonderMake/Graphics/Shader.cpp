#include "pch.h"
#include "Shader.h"

_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Fragment>>, Shader_Fragment);
_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Geometry>>, Shader_Geometry);
_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Vertex>>, Shader_Vertex);
