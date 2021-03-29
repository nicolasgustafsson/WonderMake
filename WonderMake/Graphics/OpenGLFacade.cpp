#include "pch.h"
#include "OpenGLFacade.h"

REGISTER_SYSTEM(OpenGLFacade);

void OpenGLFacade::Enable(const GLenum aSetting)
{
	//glEnable(aSetting);
}

void OpenGLFacade::SetViewportSize(const SVector2i aWindowSize)
{
	//glViewport(0, 0, aWindowSize.X, aWindowSize.Y);
}

void OpenGLFacade::SetBlendFunction(const GLenum aSourceFactor, const GLenum aDestinationFactor)
{
	//glBlendFunc(aSourceFactor, aDestinationFactor);
}

void OpenGLFacade::SetDepthFunction(const GLenum aDepthMode)
{
	//glDepthFunc(aDepthMode);
}

void OpenGLFacade::SetClearColor(const SColor aColor)
{
	//glClearColor(aColor.R, aColor.G, aColor.B, aColor.A);
}

void OpenGLFacade::SetClearDepth(const f32 aDepth)
{
	//glClearDepthf(aDepth);
}

void OpenGLFacade::Clear(const GLbitfield aMask)
{
	//glClear(aMask);
}

u32 OpenGLFacade::GenerateFramebuffer()
{
	return 0;
	//u32 frameBuffer;
	//glGenFramebuffers(1, &frameBuffer);
	//return frameBuffer;
}

void OpenGLFacade::DeleteFramebuffer(const u32 aFramebuffer)
{
	//glDeleteFramebuffers(1, &aFramebuffer);
}

void OpenGLFacade::BindFramebuffer(const GLenum aTarget, const u32 aFramebuffer)
{
	//glBindFramebuffer(aTarget, aFramebuffer);
}

GLenum OpenGLFacade::CheckFramebufferStatus(const GLenum aTarget)
{
	return 0;//glCheckFramebufferStatus(aTarget);
}

GLenum OpenGLFacade::CheckNamedFramebufferStatus(const GLenum aTarget, const u32 aFramebuffer)
{
	return 0;
	//return glCheckNamedFramebufferStatus(aFramebuffer, aTarget);
}

u32 OpenGLFacade::GenerateTexture()
{
	return 0;
	//u32 texture;
	//glGenTextures(1, &texture);
	//return texture;
}

void OpenGLFacade::DeleteTexture(const u32 aFramebuffer)
{
	return;
	glDeleteTextures(1, &aFramebuffer);
}

void OpenGLFacade::BindTexture(const GLenum aTarget, const u32 aTexture)
{
	return;
	glBindTexture(aTarget, aTexture);
}

void OpenGLFacade::UploadTextureData(const GLenum aTarget, const i32 aLevelOfDetail, const i32 aInternalFormat, const i32 aWidth, const i32 aHeight, const GLenum aPixelDataFormat, const GLenum aPixelDataType, const void* aPixelData)
{
	return;
	glTexImage2D(aTarget, aLevelOfDetail, aInternalFormat, aWidth, aHeight, 0, aPixelDataFormat, aPixelDataType, aPixelData);
}

void OpenGLFacade::SetTexureParameter(const GLenum aTarget, const GLenum aParameterName, const i32 aParameterValue)
{
	return;
	glTexParameteri(aTarget, aParameterName, aParameterValue);
}

void OpenGLFacade::SetActiveTextureSlot(const u32 aSlot)
{
	return;
	glActiveTexture(GL_TEXTURE0 + aSlot);
}

void OpenGLFacade::GenerateMipMapForCurrentTexture(const GLenum aTarget)
{
	return;
	glGenerateMipmap(aTarget);
}

void OpenGLFacade::SetFramebufferTexture(const GLenum aAttachment, const GLenum aTextureTarget, const u32 aTexture)
{
	return;
	glFramebufferTexture2D(GL_FRAMEBUFFER, aAttachment, aTextureTarget, aTexture, 0);
}

u32 OpenGLFacade::GenerateRenderbuffer()
{
	return 0;
	u32 aRenderBuffer;
	glGenRenderbuffers(1, &aRenderBuffer);
	return aRenderBuffer;
}

void OpenGLFacade::DeleteRenderbuffer(const u32 aRenderbuffer)
{
	return;
	glDeleteRenderbuffers(1, &aRenderbuffer);
}

void OpenGLFacade::BindRenderbuffer(const u32 aRenderBuffer)
{
	return;
	glBindRenderbuffer(GL_RENDERBUFFER, aRenderBuffer);
}

void OpenGLFacade::SetRenderbufferStorageParameters(const GLenum aInternalFormat, const i32 aWidth, const i32 aHeight)
{
	return;
	glRenderbufferStorage(GL_RENDERBUFFER, aInternalFormat, aWidth, aHeight);
}

void OpenGLFacade::AttachRenderbufferToFramebuffer(const GLenum aAttachment, const u32 aRenderbuffer)
{
	return;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, aAttachment, GL_RENDERBUFFER, aRenderbuffer);
}

u32 OpenGLFacade::GenerateBuffer()
{
	return 0;
	u32 aBuffer;
	glGenBuffers(1, &aBuffer);
	return aBuffer;
}

void OpenGLFacade::DeleteBuffer(const u32 aBuffer)
{
	return;
	glDeleteBuffers(1, &aBuffer);
}

void OpenGLFacade::BindBuffer(const GLenum aTarget, const u32 aBuffer)
{
	return;
	glBindBuffer(aTarget, aBuffer);
}

void OpenGLFacade::BindBufferToSlot(const GLenum aTarget, const u32 aIndex, const u32 aBuffer)
{
	return;
	glBindBufferBase(aTarget, aIndex, aBuffer);
}

void OpenGLFacade::AllocateBufferData(const GLenum aTarget, const i64 aSize, const void* aData, const GLenum aUsage)
{
	return;
	glBufferData(aTarget, aSize, aData, aUsage);
}

void OpenGLFacade::UpdateBufferData(const GLenum aTarget, const i64 aOffset, const i64 aSize, const void* aData)
{
	return;
	glBufferSubData(aTarget, aOffset, aSize, aData);
}

void OpenGLFacade::DefineVertexAttributeData(const u32 aIndex, const i32 aSize, const GLenum aType, const bool aNormalized, const i32 aStride, const void* aOffset)
{
	return;
	glVertexAttribPointer(aIndex, aSize, aType, aNormalized, aStride, aOffset);
}

void OpenGLFacade::EnableVertexAttributeArray(const u32 aIndex)
{
	return;
	glEnableVertexAttribArray(aIndex);
}

u32 OpenGLFacade::GenerateVertexArray()
{
	return 0;
	u32 aVertexArray;
	glGenVertexArrays(1, &aVertexArray);
	return aVertexArray;
}

void OpenGLFacade::DeleteVertexArray(const u32 aVertexArray)
{
	return;
	glDeleteVertexArrays(1, &aVertexArray);
}

void OpenGLFacade::BindVertexArray(const u32 aVertexArray)
{
	return;
	glBindVertexArray(aVertexArray);
}

void OpenGLFacade::DrawArrays(const GLenum aDrawMode, const i32 aFirstIndex, const i32 aAmount)
{
	return;
	glDrawArrays(aDrawMode, aFirstIndex, aAmount);
}

u32 OpenGLFacade::CreateShader(const GLenum aShaderType)
{
	return 0;
	return glCreateShader(aShaderType);
}

void OpenGLFacade::DeleteShader(const u32 aShader)
{
	return;
	glDeleteShader(aShader);
}

void OpenGLFacade::SetShaderSource(const u32 aShader, const char* aSource)
{
	return;
	glShaderSource(aShader, 1, &aSource, nullptr);
}

void OpenGLFacade::CompileShader(const u32 aShader)
{
	return;
	glCompileShader(aShader);
}

i32 OpenGLFacade::GetShaderParameter(const u32 aShader, const GLenum aParameterName)
{
	return 0;

	i32 parameter;
	glGetShaderiv(aShader, aParameterName, &parameter);
	return parameter;
}

std::string OpenGLFacade::GetShaderInfoLog(const u32 aShader)
{
	return "no";
	char errorMessage[512];
	glGetShaderInfoLog(aShader, 512, nullptr, errorMessage);

	return std::string(errorMessage);
}

u32 OpenGLFacade::CreateShaderProgram()
{
	return 0;
	return glCreateProgram();
}

void OpenGLFacade::DeleteShaderProgram(const u32 aShaderProgram)
{
	return;
	glDeleteProgram(aShaderProgram);
}

void OpenGLFacade::AttachShaderToProgram(const u32 aShaderProgram, const u32 aShader)
{
	return;
	glAttachShader(aShaderProgram, aShader);
}

void OpenGLFacade::LinkShaderProgram(const u32 aShaderProgram)
{
	return;
	glLinkProgram(aShaderProgram);
}

void OpenGLFacade::UseShaderProgram(const u32 aShaderProgram)
{
	return;
	glUseProgram(aShaderProgram);
}

i32 OpenGLFacade::GetShaderProgramParameter(const u32 aShaderProgram, const GLenum aParameterName)
{
	return 0;

	i32 parameter;
	glGetProgramiv(aShaderProgram, aParameterName, &parameter);

	return parameter;
}

std::string OpenGLFacade::GetShaderProgramInfoLog(const u32 aShaderProgram)
{
	return "nrnierst";
	char errorMessage[512];
	glGetProgramInfoLog(aShaderProgram, 512, nullptr, errorMessage);

	return std::string(errorMessage);
}

i32 OpenGLFacade::GetUniformVariableLocation(const u32 aShaderProgram, const char* aVariableName)
{
	return 0;

	return glGetUniformLocation(aShaderProgram, aVariableName);
}
