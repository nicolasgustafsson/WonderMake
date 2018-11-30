#pragma once

class Window;
class ImguiWrapper : NonCopyable
{
public:
	ImguiWrapper() = delete;
	ImguiWrapper(const Window& Window);

	void StartFrame();

	void EndFrame();
};

