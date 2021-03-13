#pragma once
//#define _ITERATOR_DEBUG_LEVEL 0
#include <string>
#include <filesystem>
#include <optional>
#include <array>
#include <functional>

#include <glad/glad.h>

#include "Typedefs.h"

#include "Utilities/Result.h"
#include "Utilities/RestrictTypes.h"
#include "Utilities/Vector.h"
#include "Utilities/TimeKeeper.h"
#include "Utilities/Json/JsonExtensions.h"

#include "System/SystemPtr.h"
#include "Message/Messages.h"
#include "Constants.h"
#include "Utilities/Matrix.h"
#include "Object/Object.h"
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/Component.h"
