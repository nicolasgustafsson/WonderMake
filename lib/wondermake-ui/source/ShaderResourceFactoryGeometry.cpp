#include "ShaderResourceFactory.h"

#include "wondermake-base/SystemGlobal.h"

 // Kevin: Split the shader registering due to obj files becoming too big

using ShaderResourceFactoryGeometry = ShaderResourceFactory<EShaderType::Geometry>;

WM_REGISTER_SYSTEM(ShaderResourceFactoryGeometry);
