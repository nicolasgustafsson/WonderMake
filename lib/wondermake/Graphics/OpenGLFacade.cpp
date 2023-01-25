#include "OpenGLFacade.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(OpenGLFacade);

void OpenGLFacade::Enable(const GLenum aSetting)
{
	glEnable(aSetting);
}

void OpenGLFacade::SetViewport(const SVector2i aPosition, const SVector2i aSize)
{
	glViewport(aPosition.X, aPosition.Y, aSize.X, aSize.Y);
}

void OpenGLFacade::SetViewportSize(const SVector2i aWindowSize)
{
	glViewport(0, 0, aWindowSize.X, aWindowSize.Y);
}

void OpenGLFacade::SetBlendFunction(const GLenum aSourceFactor, const GLenum aDestinationFactor)
{
	glBlendFunc(aSourceFactor, aDestinationFactor);
}

void OpenGLFacade::SetDepthFunction(const GLenum aDepthMode)
{
	glDepthFunc(aDepthMode);
}

void OpenGLFacade::SetClearColor(const SColor aColor)
{
	glClearColor(aColor.R, aColor.G, aColor.B, aColor.A);
}

void OpenGLFacade::SetClearDepth(const f32 aDepth)
{
	glClearDepthf(aDepth);
}

void OpenGLFacade::Clear(const GLbitfield aMask)
{
	glClear(aMask);
}

u32 OpenGLFacade::GenerateFramebuffer()
{
	u32 frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	return frameBuffer;
}

void OpenGLFacade::DeleteFramebuffer(const u32 aFramebuffer)
{
	glDeleteFramebuffers(1, &aFramebuffer);
}

void OpenGLFacade::BindFramebuffer(const GLenum aTarget, const u32 aFramebuffer)
{
	glBindFramebuffer(aTarget, aFramebuffer);
}

GLenum OpenGLFacade::CheckFramebufferStatus(const GLenum aTarget)
{
	return glCheckFramebufferStatus(aTarget);
}

GLenum OpenGLFacade::CheckNamedFramebufferStatus(const GLenum aTarget, const u32 aFramebuffer)
{
	return glCheckNamedFramebufferStatus(aFramebuffer, aTarget);
}

u32 OpenGLFacade::GenerateTexture()
{
	u32 texture;
	glGenTextures(1, &texture);
	return texture;
}

void OpenGLFacade::DeleteTexture(const u32 aFramebuffer)
{
	glDeleteTextures(1, &aFramebuffer);
}

void OpenGLFacade::BindTexture(const GLenum aTarget, const u32 aTexture)
{
	glBindTexture(aTarget, aTexture);
}

void OpenGLFacade::UploadTextureData(const GLenum aTarget, const i32 aLevelOfDetail, const i32 aInternalFormat, const i32 aWidth, const i32 aHeight, const GLenum aPixelDataFormat, const GLenum aPixelDataType, const void* aPixelData)
{
	glTexImage2D(aTarget, aLevelOfDetail, aInternalFormat, aWidth, aHeight, 0, aPixelDataFormat, aPixelDataType, aPixelData);
}

void OpenGLFacade::SetTexureParameter(const GLenum aTarget, const GLenum aParameterName, const i32 aParameterValue)
{
	glTexParameteri(aTarget, aParameterName, aParameterValue);
}

void OpenGLFacade::SetActiveTextureSlot(const u32 aSlot)
{
	glActiveTexture(GL_TEXTURE0 + aSlot);
}

void OpenGLFacade::GenerateMipMapForCurrentTexture(const GLenum aTarget)
{
	glGenerateMipmap(aTarget);
}

void OpenGLFacade::SetFramebufferTexture(const GLenum aAttachment, const GLenum aTextureTarget, const u32 aTexture)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, aAttachment, aTextureTarget, aTexture, 0);
}

u32 OpenGLFacade::GenerateRenderbuffer()
{
	u32 aRenderBuffer;
	glGenRenderbuffers(1, &aRenderBuffer);
	return aRenderBuffer;
}

void OpenGLFacade::DeleteRenderbuffer(const u32 aRenderbuffer)
{
	glDeleteRenderbuffers(1, &aRenderbuffer);
}

void OpenGLFacade::BindRenderbuffer(const u32 aRenderBuffer)
{
	glBindRenderbuffer(GL_RENDERBUFFER, aRenderBuffer);
}

void OpenGLFacade::SetRenderbufferStorageParameters(const GLenum aInternalFormat, const i32 aWidth, const i32 aHeight)
{
	glRenderbufferStorage(GL_RENDERBUFFER, aInternalFormat, aWidth, aHeight);
}

void OpenGLFacade::AttachRenderbufferToFramebuffer(const GLenum aAttachment, const u32 aRenderbuffer)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, aAttachment, GL_RENDERBUFFER, aRenderbuffer);
}

u32 OpenGLFacade::GenerateBuffer()
{
	u32 aBuffer;
	glGenBuffers(1, &aBuffer);
	return aBuffer;
}

void OpenGLFacade::DeleteBuffer(const u32 aBuffer)
{
	glDeleteBuffers(1, &aBuffer);
}

void OpenGLFacade::BindBuffer(const GLenum aTarget, const u32 aBuffer)
{
	glBindBuffer(aTarget, aBuffer);
}

void OpenGLFacade::BindBufferToSlot(const GLenum aTarget, const u32 aIndex, const u32 aBuffer)
{
	glBindBufferBase(aTarget, aIndex, aBuffer);
}

void OpenGLFacade::AllocateBufferData(const GLenum aTarget, const i64 aSize, const void* aData, const GLenum aUsage)
{
	glBufferData(aTarget, static_cast<GLsizeiptr>(aSize), aData, aUsage);
}

void OpenGLFacade::UpdateBufferData(const GLenum aTarget, const i64 aOffset, const i64 aSize, const void* aData)
{
	glBufferSubData(aTarget, static_cast<GLintptr>(aOffset), static_cast<GLsizeiptr>(aSize), aData);
}

void OpenGLFacade::DefineVertexAttributeData(const u32 aIndex, const i32 aSize, const GLenum aType, const bool aNormalized, const i32 aStride, const void* aOffset)
{
	glVertexAttribPointer(aIndex, aSize, aType, aNormalized, aStride, aOffset);
}

void OpenGLFacade::EnableVertexAttributeArray(const u32 aIndex)
{
	glEnableVertexAttribArray(aIndex);
}

u32 OpenGLFacade::GenerateVertexArray()
{
	u32 aVertexArray;
	glGenVertexArrays(1, &aVertexArray);
	return aVertexArray;
}

void OpenGLFacade::DeleteVertexArray(const u32 aVertexArray)
{
	glDeleteVertexArrays(1, &aVertexArray);
}

void OpenGLFacade::BindVertexArray(const u32 aVertexArray)
{
	glBindVertexArray(aVertexArray);
}

void OpenGLFacade::DrawArrays(const GLenum aDrawMode, const i32 aFirstIndex, const i32 aAmount)
{
	glDrawArrays(aDrawMode, aFirstIndex, aAmount);
}

u32 OpenGLFacade::CreateShader(const GLenum aShaderType)
{
	return glCreateShader(aShaderType);
}

void OpenGLFacade::DeleteShader(const u32 aShader)
{
	glDeleteShader(aShader);
}

void OpenGLFacade::SetShaderSource(const u32 aShader, const char* aSource)
{
	glShaderSource(aShader, 1, &aSource, nullptr);
}

void OpenGLFacade::CompileShader(const u32 aShader)
{
	glCompileShader(aShader);
}

i32 OpenGLFacade::GetShaderParameter(const u32 aShader, const GLenum aParameterName)
{
	i32 parameter;
	glGetShaderiv(aShader, aParameterName, &parameter);
	return parameter;
}

std::string OpenGLFacade::GetShaderInfoLog(const u32 aShader)
{
	char errorMessage[512];
	glGetShaderInfoLog(aShader, 512, nullptr, errorMessage);

	return std::string(errorMessage);
}

u32 OpenGLFacade::CreateShaderProgram()
{
	return glCreateProgram();
}

void OpenGLFacade::DeleteShaderProgram(const u32 aShaderProgram)
{
	glDeleteProgram(aShaderProgram);
}

void OpenGLFacade::AttachShaderToProgram(const u32 aShaderProgram, const u32 aShader)
{
	glAttachShader(aShaderProgram, aShader);
}

void OpenGLFacade::LinkShaderProgram(const u32 aShaderProgram)
{
	glLinkProgram(aShaderProgram);
}

void OpenGLFacade::UseShaderProgram(const u32 aShaderProgram)
{
	glUseProgram(aShaderProgram);
}

i32 OpenGLFacade::GetShaderProgramParameter(const u32 aShaderProgram, const GLenum aParameterName)
{
	i32 parameter;
	glGetProgramiv(aShaderProgram, aParameterName, &parameter);

	return parameter;
}

std::string OpenGLFacade::GetShaderProgramInfoLog(const u32 aShaderProgram)
{
	char errorMessage[512];
	glGetProgramInfoLog(aShaderProgram, 512, nullptr, errorMessage);

	return std::string(errorMessage);
}

i32 OpenGLFacade::GetUniformVariableLocation(const u32 aShaderProgram, const char* aVariableName)
{
	return glGetUniformLocation(aShaderProgram, aVariableName);
}
