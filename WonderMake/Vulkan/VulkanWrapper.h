#pragma once
#include <Vulkan/vulkan.hpp>

class VulkanWrapper
{
public:
	void Init();

	void Update();

	void Cleanup();
private:
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void SetupSurface();
	void CreateGraphicsPipeline();

	std::optional<u32> GetGraphicsQueueFamilyIndex() const;

	std::optional<u32> GetDeviceScore(const vk::PhysicalDevice& aPhysicalDevice) const;
	bool IsDeviceSuitable(const vk::PhysicalDevice& aPhysicalDevice) const;

	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> myDebugMessenger;
	
	vk::Instance myInstance;

	vk::PhysicalDevice myPhysicalDevice;
	vk::UniqueDevice myLogicalDevice;

	vk::Queue myGraphicsQueueHandle;

	vk::SurfaceKHR mySurface;

	vk::DispatchLoaderDynamic myDynamicDispatchLoader;

	vk::SwapchainKHR mySwapchain;
	std::vector<vk::Image> mySwapchainImages;
	vk::Format mySwapchainFormat;
	vk::Extent2D mySwapchainExtent;
	std::vector<vk::ImageView> mySwapchainImageViews;

};

