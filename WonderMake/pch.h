#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#define _ITERATOR_DEBUG_LEVEL 0
#define VULKAN_HPP_NO_EXCEPTIONS //[Nicos]: this gives us fun result types instead of thrown exceptions :)
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <filesystem>
#include <optional>
#include <array>
#include <functional>
#include "System/SystemPtr.h"
#include "Utilities/Result.h"
#include "Utilities/Color.h"
#include "Utilities/RestrictTypes.h"
#include <glad/glad.h>
#include "Utilities/Vector.h"
#include "Typedefs.h"
#include "Message/Messages.h"
#include "Constants.h"
#include "Utilities/Matrix.h"
#include "Object/Object.h"
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Component.h"
#include "Utilities/TimeKeeper.h"
#include "Utilities/Json/JsonExtensions.h"
