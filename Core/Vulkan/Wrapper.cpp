#include "Wrapper.hpp"
#include <iomanip>
#include <sstream>
#include <type_traits>
namespace Vulkan {


    void Check(const VkResult result, const char* const operation)
    {
	    if (result != VK_SUCCESS)
	    {
		
            std::cerr << std::string("failed to ") + operation + " (" + ToString(result) + ")" << std::endl;
	    }
    }

    const char* ToString(const VkResult result)
    {
        switch (result)
        {
    #define CASE(r) case VK_ ##r: return #r
            CASE(SUCCESS);
            CASE(NOT_READY);
            CASE(TIMEOUT);
            CASE(EVENT_SET);
            CASE(EVENT_RESET);
            CASE(INCOMPLETE);
            CASE(ERROR_OUT_OF_HOST_MEMORY);
            CASE(ERROR_OUT_OF_DEVICE_MEMORY);
            CASE(ERROR_INITIALIZATION_FAILED);
            CASE(ERROR_DEVICE_LOST);
            CASE(ERROR_MEMORY_MAP_FAILED);
            CASE(ERROR_LAYER_NOT_PRESENT);
            CASE(ERROR_EXTENSION_NOT_PRESENT);
            CASE(ERROR_FEATURE_NOT_PRESENT);
            CASE(ERROR_INCOMPATIBLE_DRIVER);
            CASE(ERROR_TOO_MANY_OBJECTS);
            CASE(ERROR_FORMAT_NOT_SUPPORTED);
            CASE(ERROR_FRAGMENTED_POOL);
            CASE(ERROR_UNKNOWN);
            CASE(ERROR_OUT_OF_POOL_MEMORY);
            CASE(ERROR_INVALID_EXTERNAL_HANDLE);
            CASE(ERROR_FRAGMENTATION);
            CASE(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
            CASE(ERROR_SURFACE_LOST_KHR);
            CASE(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE(SUBOPTIMAL_KHR);
            CASE(ERROR_OUT_OF_DATE_KHR);
            CASE(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE(ERROR_VALIDATION_FAILED_EXT);
            CASE(ERROR_INVALID_SHADER_NV);
            CASE(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
            CASE(ERROR_NOT_PERMITTED_EXT);
            CASE(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
            CASE(THREAD_IDLE_KHR);
            CASE(THREAD_DONE_KHR);
            CASE(OPERATION_DEFERRED_KHR);
            CASE(OPERATION_NOT_DEFERRED_KHR);
            CASE(PIPELINE_COMPILE_REQUIRED_EXT);
    #undef CASE
        default:
            return "UNKNOWN_ERROR";
        }
    }

    std::string to_string(VkDebugUtilsMessageSeverityFlagBitsEXT aSeverity)
    {
        // See
        // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDebugUtilsMessageSeverityFlagBitsEXT.html
        switch (aSeverity)
        {
            // This appears fairly frequently in the output, so make the part 
            // that's printed a bit shorter.
#			define CASE_(x) case VK_DEBUG_UTILS_MESSAGE_##x##_BIT_EXT: return #x
            CASE_(SEVERITY_VERBOSE);
            CASE_(SEVERITY_INFO);
            CASE_(SEVERITY_WARNING);
            CASE_(SEVERITY_ERROR);
#			undef CASE_

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
        }

        // Handle other values gracefully.
        std::ostringstream oss;
        oss << "VkDebugUtilsMessageSeverityFlagBitsEXT(" << std::underlying_type_t<VkDebugUtilsMessageSeverityFlagBitsEXT>(aSeverity) << ")";
        return oss.str();
    }


    std::string message_type_flags(VkDebugUtilsMessageTypeFlagsEXT aFlags)
    {
        std::ostringstream oss;

        bool separator = false;

        // This appears fairly frequently in the output, so make the part 
        // that's printed a bit shorter.
#		define APPEND_(x) if( VK_DEBUG_UTILS_MESSAGE_TYPE_##x##_BIT_EXT & aFlags ) { \
			if( separator ) oss << ", "; \
			oss << #x; \
			aFlags &= ~VkDebugUtilsMessageTypeFlagsEXT(VK_DEBUG_UTILS_MESSAGE_TYPE_##x##_BIT_EXT); \
			separator = true; \
		} /*ENDM*/

        APPEND_(GENERAL);
        APPEND_(VALIDATION);
        APPEND_(PERFORMANCE);

#		undef APPEND_

        if (aFlags)
        {
            if (separator) oss << " | ";
            oss << "VkDebugUtilsMessageTypeFlagsEXT(" << std::hex << aFlags << ")";
        }

        return oss.str();
    }
}
