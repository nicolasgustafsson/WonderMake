#include "ShaderResourceFactory.h"

#include "ConfigurationUi.h"
#include "ShaderParser.h"

#include "wondermake-ui/OpenGLFacade.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/SystemGlobal.h"

using ShaderResourceFactoryVertex	= ShaderResourceFactory<EShaderType::Vertex>;
using ShaderResourceFactoryFragment	= ShaderResourceFactory<EShaderType::Fragment>;
using ShaderResourceFactoryGeometry	= ShaderResourceFactory<EShaderType::Geometry>;

WM_REGISTER_SYSTEM(ShaderResourceFactoryVertex);
WM_REGISTER_SYSTEM(ShaderResourceFactoryFragment);
WM_REGISTER_SYSTEM(ShaderResourceFactoryGeometry);

template<EShaderType ShaderType>
Future<SharedReference<ShaderResource<ShaderType>>> ShaderResourceFactory<ShaderType>::CreateResourceStrategy(FileResourceId /*aId*/, FilePath aPath, MakeResourceOp aMakeOp)
{
	auto& configSys = this->Get<ConfigurationSystem>();

	auto [shaderString, files] = ShaderParser::ParseShader(aPath, configSys.Get<FilePath>(ConfigurationUi::ShaderSearchPath, FilePath()));

	for (auto& file : files)
		aMakeOp.AddRelatedFile(std::move(file));

	if (!shaderString)
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Shader preprocessing failed: " << aPath << '.');

		return MakeCompletedFuture<SharedReference<ShaderResource<ShaderType>>>(aMakeOp(0));
	}

	auto& openGL = this->Get<OpenGLFacade>();

	u32 shaderHandle = 0;

	if constexpr (ShaderType == EShaderType::Vertex)
		shaderHandle = openGL.CreateShader(GL_VERTEX_SHADER);
	else if constexpr (ShaderType == EShaderType::Fragment)
		shaderHandle = openGL.CreateShader(GL_FRAGMENT_SHADER);
	else if constexpr (ShaderType == EShaderType::Geometry)
		shaderHandle = openGL.CreateShader(GL_GEOMETRY_SHADER);

	const char* raw = shaderString->c_str();

	openGL.SetShaderSource(shaderHandle, raw);

	openGL.CompileShader(shaderHandle);

	const i32 compileWasSuccessful = openGL.GetShaderParameter(shaderHandle, GL_COMPILE_STATUS);

	if (!compileWasSuccessful)
	{
		const std::string errorMessage = openGL.GetShaderInfoLog(shaderHandle);

		WmLogError(TagWonderMake << TagWmOpenGL << "Shader compilation failed: {" << errorMessage << "}.");
	}

	return MakeCompletedFuture<SharedReference<ShaderResource<ShaderType>>>(aMakeOp(shaderHandle));
}

template<EShaderType ShaderType>
void ShaderResourceFactory<ShaderType>::DestroyResource(ShaderResource<ShaderType>& aResource)
{
	if (aResource.Handle())
		this->Get<OpenGLFacade>().DeleteShader(aResource.Handle());
}
