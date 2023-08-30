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
	static constexpr u32 errorHandle = 0;

	auto& configSys = this->Get<ConfigurationSystem>();

	auto parsedShader = ShaderParser::ParseShader(aPath, configSys.Get<FilePath>(ConfigurationUi::ShaderSearchPath, FilePath()));

	const auto& shaderCode = parsedShader.AllCode();
	for (auto& file : std::ranges::subrange(++parsedShader.RelatedFiles().begin(), parsedShader.RelatedFiles().end()))
		aMakeOp.AddRelatedFile(file);

	if (!shaderCode)
	{
		WmLogError(TagWonderMake << TagWmOpenGL << "Shader preprocessing failed: " << aPath << '.');

		return MakeCompletedFuture<SharedReference<ShaderResource<ShaderType>>>(aMakeOp(std::move(parsedShader), errorHandle, "Failed to parse shader file.", EShaderStatus::Err));
	}

	auto& openGL = this->Get<OpenGLFacade>();

	u32 shaderHandle = 0;

	if constexpr (ShaderType == EShaderType::Vertex)
		shaderHandle = openGL.CreateShader(GL_VERTEX_SHADER);
	else if constexpr (ShaderType == EShaderType::Fragment)
		shaderHandle = openGL.CreateShader(GL_FRAGMENT_SHADER);
	else if constexpr (ShaderType == EShaderType::Geometry)
		shaderHandle = openGL.CreateShader(GL_GEOMETRY_SHADER);

	const char* raw = shaderCode->c_str();

	openGL.SetShaderSource(shaderHandle, raw);

	openGL.CompileShader(shaderHandle);

	const i32 compileWasSuccessful = openGL.GetShaderParameter(shaderHandle, GL_COMPILE_STATUS);

	if (!compileWasSuccessful)
	{
		 // Kevin: The output from GetShaderInfoLog is not standardized, so this is tailored for the messages I get on my machine
		 // If the message does not follow the expected format it should just display the raw message.
		const auto parseErrorMessage = [&aPath, &parsedShader](const std::string& aErrorMessage) -> std::string
		{
			auto lineItBegin = aErrorMessage.begin();
			std::string err;

			while (lineItBegin != aErrorMessage.end())
			{
				auto lineItEnd = std::find(lineItBegin, aErrorMessage.end(), '\n');

				auto lineView = std::string_view(lineItBegin, lineItEnd);
				
				auto errorLineBegin	= std::ranges::find(lineView, '(');
				
				auto errorLineEnd	= std::find(errorLineBegin, lineView.end(), ')');

				auto errorLineText	= std::find(errorLineEnd, lineView.end(), ':');

				if (errorLineBegin == lineView.end() || errorLineEnd == lineView.end() || errorLineText == lineView.end())
					return aErrorMessage;
				
				std::string_view lineNumberView(errorLineBegin + 1, errorLineEnd);
				size_t line = 0;

				if (std::from_chars(lineNumberView.data(), lineNumberView.data() + lineNumberView.size(), line).ec != std::errc{})
					return aErrorMessage;

				auto lineInfo = parsedShader.GetShaderLineInfo(line);

				std::stringstream ss;

				WmLogStream(ss, lineInfo.Path);

				err += std::move(ss).str();

				err += '(' + std::to_string(lineInfo.Line) + "):" + std::string(errorLineText + 1, lineView.end()) + '\n';

				if (lineItEnd == aErrorMessage.end())
					break;

				lineItBegin = lineItEnd + 1;
			}

			if (err.back() == '\n')
				err.pop_back();

			return err;
		};

		std::string errorMessage = parseErrorMessage(openGL.GetShaderInfoLog(shaderHandle));

		WmLogError(TagWonderMake << TagWmOpenGL << "Shader compilation failed: {\n" << errorMessage << "\n}.");

		openGL.DeleteShader(shaderHandle);

		return MakeCompletedFuture<SharedReference<ShaderResource<ShaderType>>>(aMakeOp(std::move(parsedShader), errorHandle, std::move(errorMessage), EShaderStatus::Err));
	}

	return MakeCompletedFuture<SharedReference<ShaderResource<ShaderType>>>(aMakeOp(std::move(parsedShader), shaderHandle, "Compiled successfully", EShaderStatus::Ok));
}

template<EShaderType ShaderType>
void ShaderResourceFactory<ShaderType>::DestroyResource(ShaderResource<ShaderType>& aResource)
{
	if (aResource.Handle())
		this->Get<OpenGLFacade>().DeleteShader(aResource.Handle());
}
