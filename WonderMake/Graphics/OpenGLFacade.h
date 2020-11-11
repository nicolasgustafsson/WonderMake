#pragma once
#include "System/System.h"


//[Nicos]: Note that this is not a complete facade for openGL; you may need to create your own wrapped functions.
class OpenGLFacade : public System
{
public:
	void Enable(const GLenum aSetting);

	void SetViewportSize(const SVector2i aWindowSize);

	void SetBlendFunction(const GLenum aSourceFactor, const GLenum aDestinationFactor);

	void SetClearColor(const SColor aColor);
	void SetClearDepth(const f32 aDepth);
	void Clear(const GLbitfield aMask);
	
	[[nodiscard]] u32 GenerateFramebuffer();
	void DeleteFramebuffer(const u32 aFramebuffer);
	void BindFramebuffer(const GLenum aTarget, const u32 aFramebuffer);
	[[nodiscard]] GLenum CheckFramebufferStatus(const GLenum aTarget);
	[[nodiscard]] GLenum CheckNamedFramebufferStatus(const GLenum aTarget, const u32 aFramebuffer);

	[[nodiscard]] u32 GenerateTexture();
	void DeleteTexture(const u32 aFramebuffer);
	void BindTexture(const GLenum aTarget, const u32 aTexture);
	void UploadTextureData(const GLenum aTarget, const i32 aLevelOfDetail, const i32 aInternalFormat, const i32 aWidth, const i32 aHeight, const GLenum aPixelDataFormat, const GLenum aPixelDataType, const void* aPixelData);
	void SetTexureParameter(const GLenum aTarget, const GLenum aParameterName, const i32 aParameterValue);
	void SetActiveTextureSlot(const u32 aSlot);
	void GenerateMipMapForCurrentTexture(const GLenum aTarget);

	void SetFramebufferTexture(const GLenum aAttachment, const GLenum aTextureTarget, const u32 aTexture);

	[[nodiscard]] u32 GenerateRenderbuffer();
	void DeleteRenderbuffer(const u32 aRenderbuffer);
	void BindRenderbuffer(const u32 aRenderBuffer);
	void SetRenderbufferStorageParameters(const GLenum aInternalFormat, const i32 aWidth, const i32 aHeight);

	void AttachRenderbufferToFramebuffer(const GLenum aAttachment, const u32 aRenderbuffer);

	[[nodiscard]] u32 GenerateBuffer();
	void DeleteBuffer(const u32 aBuffer);
	void BindBuffer(const GLenum aTarget, const u32 aBuffer);
	void BindBufferToSlot(const GLenum aTarget, const u32 aIndex, const u32 aBuffer);
	void AllocateBufferData(const GLenum aTarget, const i64 aSize, const void* aData, const GLenum aUsage);
	void UpdateBufferData(const GLenum aTarget, const i64 aOffset, const i64 aSize, const void* aData);
	
	void DefineVertexAttributeData(const u32 aIndex, const i32 aSize, const GLenum aType, const bool aNormalized, const i32 aStride, const void* aOffset);
	void EnableVertexAttributeArray(const u32 aIndex);

	[[nodiscard]] u32 GenerateVertexArray();
	void DeleteVertexArray(const u32 aVertexArray);
	void BindVertexArray(const u32 aVertexArray);

	void DrawArrays(const GLenum aDrawMode, const i32 aFirstIndex, const i32 aAmount);

	[[nodiscard]] u32 CreateShader(const GLenum aShaderType);
	void DeleteShader(const u32 aShader);
	void SetShaderSource(const u32 aShader, const char* aSource);
	void CompileShader(const u32 aShader);
	[[nodiscard]] i32 GetShaderParameter(const u32 aShader, const GLenum aParameterName);
	[[nodiscard]] std::string GetShaderInfoLog(const u32 aShader);

	[[nodiscard]] u32 CreateShaderProgram();
	void DeleteShaderProgram(const u32 aShaderProgram);
	void AttachShaderToProgram(const u32 aShaderProgram, const u32 aShader);
	void LinkShaderProgram(const u32 aShaderProgram);
	void UseShaderProgram(const u32 aShaderProgram);
	[[nodiscard]] i32 GetShaderProgramParameter(const u32 aShaderProgram, const GLenum aParameterName);
	[[nodiscard]] std::string GetShaderProgramInfoLog(const u32 aShaderProgram);
	[[nodiscard]] i32 GetUniformVariableLocation(const u32 aShaderProgram, const char* aVariableName);

	template<typename TVariableType>
	void SetUniformVariable(const u32 aLocation, TVariableType aProperty);

	template<typename TCallback>
	void SetDebugMessageCallback(TCallback aCallback)
	{
		glDebugMessageCallback(aCallback, nullptr);
	}

private:

};

template<typename TVariableType>
void OpenGLFacade::SetUniformVariable(const u32 aLocation, TVariableType aProperty)
{
	if constexpr (std::is_same_v<TVariableType, i32>)
		glUniform1i(aLocation, aProperty);
	else if constexpr (std::is_same_v<TVariableType, f32>)
		glUniform1f(aLocation, aProperty);
	else if constexpr (std::is_same_v<TVariableType, bool>)
		glUniform1i(aLocation, aProperty);
	else if constexpr (std::is_same_v<TVariableType, f64>)
		glUniform1d(aLocation, aProperty);
	else if constexpr (std::is_same_v<TVariableType, SVector2f>)
		glUniform2f(aLocation, aProperty.X, aProperty.Y);
	else if constexpr (std::is_same_v<TVariableType, SVector2i>)
		glUniform2i(aLocation, aProperty.X, aProperty.Y);
	else if constexpr (std::is_same_v<TVariableType, SVector3f>)
		glUniform3f(aLocation, aProperty.X, aProperty.Y, aProperty.Z);
	else if constexpr (std::is_same_v<TVariableType, SVector4f>)
		glUniform4f(aLocation, aProperty.X, aProperty.Y, aProperty.Z, aProperty.W);
	else if constexpr (std::is_same_v<TVariableType, SColor>)
		glUniform4f(aLocation, aProperty.R, aProperty.G, aProperty.B, aProperty.A);
}

