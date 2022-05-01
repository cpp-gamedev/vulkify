#pragma once
#include <ktl/expected.hpp>

namespace vf {
enum class Error {
	eUnknown,

	eInvalidArgument,
	eDuplicateInstance,
	eNoVulkanSupport,
	eGlfwFailure,
	eVulkanInitFailure,

	eInactiveInstance,
	eMemoryError,
};

template <typename T>
using Result = ktl::expected<T, Error>;
} // namespace vf
