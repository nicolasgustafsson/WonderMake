#include "wondermake-ui/ShaderResourceSystem.h"

#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(ShaderResourceSystem);

Future<FileResourceRef<ShaderResource<EShaderType::Vertex>>> ShaderResourceSystem::GetShaderVertex(const FilePath& aPath)
{
	return Get<FileResourceSystem>().GetResource<ShaderResource<EShaderType::Vertex>>(aPath);
}

Future<FileResourceRef<ShaderResource<EShaderType::Fragment>>> ShaderResourceSystem::GetShaderFragment(const FilePath& aPath)
{
	return Get<FileResourceSystem>().GetResource<ShaderResource<EShaderType::Fragment>>(aPath);
}

Future<FileResourceRef<ShaderResource<EShaderType::Geometry>>> ShaderResourceSystem::GetShaderGeometry(const FilePath& aPath)
{
	return Get<FileResourceSystem>().GetResource<ShaderResource<EShaderType::Geometry>>(aPath);
}

std::shared_ptr<ShaderProgram> ShaderResourceSystem::CreateProgram(
	FileResourceRef<ShaderResource<EShaderType::Vertex>>	aShaderVertex,
	FileResourceRef<ShaderResource<EShaderType::Fragment>>	aShaderFragment,
	FileResourcePtr<ShaderResource<EShaderType::Geometry>>	aShaderGeometry)
{
	u32 programHandle = CreateProgramHandle(aShaderVertex, aShaderFragment, aShaderGeometry);

	if (!programHandle)
		return nullptr;

	ShaderProgram* program = new ShaderProgram(programHandle, std::move(aShaderVertex), std::move(aShaderFragment), std::move(aShaderGeometry));

	return std::shared_ptr<ShaderProgram>(program, [this](ShaderProgram* aProgram)
	{
		GetExecutor().Execute([this, aProgram]()
			{
				auto& openGL = Get<OpenGLFacade>();

				openGL.DeleteShaderProgram(aProgram->Handle());

				delete aProgram;
			});
	});
}

[[nodiscard]] Future<std::shared_ptr<ShaderProgram>> ShaderResourceSystem::CreateProgram(
	const FilePath& aShaderVertex,
	const FilePath& aShaderFragment)
{
	auto futureVertex	= GetShaderVertex(aShaderVertex);
	auto futureFragment	= GetShaderFragment(aShaderFragment);

	auto futureAll = WaitForAll(
		decltype(futureVertex)(futureVertex),
		decltype(futureFragment)(futureFragment));

	return std::move(futureAll)
		.ThenApply(GetExecutor(), FutureApplyResult([
			this,
			futureVertex = std::move(futureVertex),
			futureFragment = std::move(futureFragment)]()
		{
			auto program = CreateProgram(
				*futureVertex.GetResult(),
				*futureFragment.GetResult(),
				nullptr);

			return MakeCompletedFuture<std::shared_ptr<ShaderProgram>>(std::move(program));
		}));
}

[[nodiscard]] Future<std::shared_ptr<ShaderProgram>> ShaderResourceSystem::CreateProgram(
	const FilePath& aShaderVertex,
	const FilePath& aShaderFragment,
	const FilePath& aShaderGeometry)
{
	auto futureVertex	= GetShaderVertex(aShaderVertex);
	auto futureFragment	= GetShaderFragment(aShaderFragment);
	auto futureGeometry	= GetShaderGeometry(aShaderGeometry);

	auto futureAll = WaitForAll(
		decltype(futureVertex)(futureVertex),
		decltype(futureFragment)(futureFragment),
		decltype(futureGeometry)(futureGeometry));

	return std::move(futureAll)
		.ThenApply(GetExecutor(), FutureApplyResult([
			this,
			futureVertex = std::move(futureVertex),
			futureFragment = std::move(futureFragment),
			futureGeometry = std::move(futureGeometry)]()
		{
			auto program = CreateProgram(
				*futureVertex.GetResult(),
				*futureFragment.GetResult(),
				*futureGeometry.GetResult());

			return MakeCompletedFuture<std::shared_ptr<ShaderProgram>>(std::move(program));
		}));
}

bool ShaderResourceSystem::ActivateProgram(const std::shared_ptr<ShaderProgram>& aProgram)
{
	if (!aProgram)
		return false;

	auto& openGL = Get<OpenGLFacade>();

	if (aProgram->OutOfDate())
	{
		openGL.DeleteShaderProgram(aProgram->Handle());

		u32 programHandle = CreateProgramHandle(aProgram->ShaderVertex(), aProgram->ShaderFragment(), aProgram->ShaderGeometry());

		aProgram->UpdateHandle(programHandle);
	}

	if (!aProgram->Handle())
		return false;

	openGL.UseShaderProgram(aProgram->Handle());

	return true;
}

u32 ShaderResourceSystem::CreateProgramHandle(
	const FileResourceRef<ShaderResource<EShaderType::Vertex>>& aShaderVertex,
	const FileResourceRef<ShaderResource<EShaderType::Fragment>>& aShaderFragment,
	const FileResourcePtr<ShaderResource<EShaderType::Geometry>>& aShaderGeometry)
{
	auto& openGL = Get<OpenGLFacade>();

	u32 programHandle = openGL.CreateShaderProgram();

	openGL.AttachShaderToProgram(programHandle, aShaderVertex->Handle());
	openGL.AttachShaderToProgram(programHandle, aShaderFragment->Handle());
	if (aShaderGeometry)
		openGL.AttachShaderToProgram(programHandle, aShaderGeometry->Handle());

	openGL.LinkShaderProgram(programHandle);

	const i32 success = openGL.GetShaderProgramParameter(programHandle, GL_LINK_STATUS);

	if (success)
		return programHandle;

	const std::string errorMessage = openGL.GetShaderProgramInfoLog(programHandle);

	WmLogError(TagWonderMake << TagWmOpenGL << "Shader program linking failed; error: {" << errorMessage << "}.");

	openGL.DeleteShaderProgram(programHandle);

	return 0;
}
