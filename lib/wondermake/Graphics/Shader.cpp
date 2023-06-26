#include "Shader.h"

_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Fragment>>, Shader_Fragment);
_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Geometry>>, Shader_Geometry);
_WM_REGISTER_SYSTEM_IMPL(ResourceSystem<Shader<EShaderType::Vertex>>, Shader_Vertex);
