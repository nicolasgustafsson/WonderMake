#include "ShaderResourceFactory.h"

#include "wondermake-base/SystemGlobal.h"

 // Kevin: Split the shader registering due to obj files becoming too big

using ShaderResourceFactoryFragment = ShaderResourceFactory<EShaderType::Fragment>;

WM_REGISTER_SYSTEM(ShaderResourceFactoryFragment);
