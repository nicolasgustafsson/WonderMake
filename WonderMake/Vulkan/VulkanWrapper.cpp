#include "pch.h"

#include "VulkanWrapper.h"
#include <GLFW/glfw3.h>
#include <Program/GlfwFacade.h>
#include "System/SystemPtr.h"
#include <GLFW/glfw3native.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	vk::DebugUtilsMessageSeverityFlagBitsEXT severity = (vk::DebugUtilsMessageSeverityFlagBitsEXT)messageSeverity;
	std::string severityTag;
	switch (severity)
	{
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
		severityTag = "[Verbose]";
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
		severityTag = "[Info]";
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
		severityTag = TagWarning;
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
		severityTag = TagError;
		break;
	}

	WmLog(severityTag, TagVulkan, pCallbackData->pMessage);

	return VK_FALSE;
}

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (Constants::ValidateVulkan) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanWrapper::Init()
{
	CreateInstance();
	SetupDebugMessenger();
	PickPhysicalDevice();
	CreateLogicalDevice();
	SetupSurface();
	CreateGraphicsPipeline();
}

void VulkanWrapper::Update()
{

}

void VulkanWrapper::Cleanup()
{

}

void VulkanWrapper::CreateInstance()
{
	vk::ApplicationInfo appInfo{};
	appInfo.sType = vk::StructureType::eApplicationInfo;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "WonderMake";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	vk::InstanceCreateInfo createInfo{};
	createInfo.sType = vk::StructureType::eInstanceCreateInfo;
	createInfo.pApplicationInfo = &appInfo;
	
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	std::vector<const char*> validationLayers;

	if constexpr (Constants::ValidateVulkan)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		validationLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.ppEnabledLayerNames = validationLayers.data();

	createInfo.enabledLayerCount = validationLayers.size();
	
	vk::Instance instance = vk::createInstance(createInfo).value;

	auto availableExtensions = vk::enumerateInstanceExtensionProperties().value;

	WmConditionalLog(Constants::DebugVulkan, TagVulkan, "available extensions:");

	for (const auto& extension : availableExtensions)
		WmConditionalLog(Constants::DebugVulkan, TagVulkan, '\t', extension.extensionName);

	myInstance = std::move(instance);

	myDynamicDispatchLoader = vk::DispatchLoaderDynamic(myInstance, vkGetInstanceProcAddr);
}

void VulkanWrapper::SetupDebugMessenger()
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.messageSeverity		
		= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
		| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
		| (Constants::DebugVulkan ? vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose : ((vk::DebugUtilsMessageSeverityFlagBitsEXT)0));

	createInfo.messageType 
		= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral 
		| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
		| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

	createInfo.pfnUserCallback = DebugCallback;

	myDebugMessenger = std::move(myInstance.createDebugUtilsMessengerEXTUnique(createInfo, nullptr, myDynamicDispatchLoader).value);
}

void VulkanWrapper::PickPhysicalDevice()
{
	std::vector<vk::PhysicalDevice> physicalDevices = myInstance.enumeratePhysicalDevices().value;

	if (physicalDevices.size() == 0)
		return;

	std::sort(physicalDevices.begin(), physicalDevices.end(), [&](const vk::PhysicalDevice& aLhs, const vk::PhysicalDevice& aRhs)
		{
			std::optional<u32> score = GetDeviceScore(aLhs);
			std::optional<u32> otherScore = GetDeviceScore(aRhs);

			if (!score)
				return false;

			if (!otherScore)
				return true;

			return (*score) < (*otherScore);
		});

	//Nicos: I /think/ this is the one with the highest score but needs to be doubled checked
	myPhysicalDevice = physicalDevices.back();

	WmLog(TagVulkan, "Picked physical device: ", myPhysicalDevice.getProperties().deviceName);

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = myPhysicalDevice.getQueueFamilyProperties();
	for (auto& queueFamilyProperty : queueFamilyProperties)
	{
		WmConditionalLog(Constants::DebugVulkan, TagVulkan, "Queue number: ", queueFamilyProperty.queueCount);
		WmConditionalLog(Constants::DebugVulkan, TagVulkan, "Queue flags: " + vk::to_string(queueFamilyProperty.queueFlags));
	}
}

void VulkanWrapper::CreateLogicalDevice()
{
	//[Nicos]: AFAICT creating multiple queues is a very small optimization for this engine for a lot of work, though this is subject to change

	f32 queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.setQueuePriorities(queuePriority);
	queueCreateInfo.queueFamilyIndex = *GetGraphicsQueueFamilyIndex();

	vk::PhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.geometryShader = true;

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vk::DeviceCreateInfo deviceCreateInfo({}, queueCreateInfo, {}, deviceExtensions, &deviceFeatures);
	
	myLogicalDevice = std::move(myPhysicalDevice.createDeviceUnique(deviceCreateInfo).value);

	myGraphicsQueueHandle = myLogicalDevice->getQueue(*GetGraphicsQueueFamilyIndex(), 0);
}

std::optional<u32> VulkanWrapper::GetGraphicsQueueFamilyIndex() const
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = myPhysicalDevice.getQueueFamilyProperties();
	
	for (auto [i, properties] : Utility::Enumerate(queueFamilyProperties))
	{
		if (properties.queueFlags & vk::QueueFlagBits::eGraphics)
			return i;
	}
	
	return std::nullopt;
}

std::optional<u32> VulkanWrapper::GetDeviceScore(const vk::PhysicalDevice& aPhysicalDevice) const
{
	vk::PhysicalDeviceProperties properties = aPhysicalDevice.getProperties();
	vk::PhysicalDeviceFeatures features = aPhysicalDevice.getFeatures();

	if (IsDeviceSuitable(aPhysicalDevice))
		return std::nullopt;

	u32 deviceScore{0};

	if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		deviceScore += 10000;
	
	deviceScore += properties.limits.maxImageDimension2D;

	return deviceScore;
}

bool VulkanWrapper::IsDeviceSuitable(const vk::PhysicalDevice& aPhysicalDevice) const
{
	vk::PhysicalDeviceProperties properties = aPhysicalDevice.getProperties();
	vk::PhysicalDeviceFeatures features = aPhysicalDevice.getFeatures();

	if (!features.geometryShader)
		return false;

	return true;
}

void VulkanWrapper::SetupSurface()
{
	mySurface = SystemPtr<GlfwFacade>()->CreateVulkanSurface(myInstance);

	vk::SurfaceCapabilitiesKHR surfaceCapabilities = myPhysicalDevice.getSurfaceCapabilitiesKHR(mySurface).value;

	auto formats = myPhysicalDevice.getSurfaceFormatsKHR(mySurface);

	//we must call this even if we support it, otherwise we get validation error. We can simply throw away the result for now 
	myPhysicalDevice.getSurfaceSupportKHR(*GetGraphicsQueueFamilyIndex(), mySurface);

	vk::SurfaceFormatKHR format;
	format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
	format.format = vk::Format::eB8G8R8A8Srgb;

	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eMailbox;
	auto queueFamilyIndex = *GetGraphicsQueueFamilyIndex();
	vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.surface = mySurface;
	swapchainCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
	swapchainCreateInfo.queueFamilyIndexCount = 1;
	swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
	swapchainCreateInfo.imageFormat = format.format;
	swapchainCreateInfo.imageColorSpace = format.colorSpace;
	swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = true;
	//swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; we will want to handle this later on when resizing

	mySwapchain = myLogicalDevice->createSwapchainKHR(swapchainCreateInfo).value;
	mySwapchainImages = myLogicalDevice->getSwapchainImagesKHR(mySwapchain).value;
	mySwapchainFormat = format.format;
	mySwapchainExtent = surfaceCapabilities.currentExtent;

	for (auto&& swapchainImage : mySwapchainImages)
	{
		vk::ImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.image = swapchainImage;
		imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
		imageViewCreateInfo.format = mySwapchainFormat;
		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		mySwapchainImageViews.push_back(myLogicalDevice->createImageView(imageViewCreateInfo).value);
	}
}

void VulkanWrapper::CreateGraphicsPipeline()
{
}
