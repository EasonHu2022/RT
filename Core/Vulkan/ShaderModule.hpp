#pragma once

#include "Wrapper.hpp"
#include <string>
#include <vector>

namespace Vulkan
{
	class Device;

	class ShaderModule final
	{
	public:

		VULKAN_OBJECT(ShaderModule)

		ShaderModule(const Device& device, const std::string& filename);
		ShaderModule(const Device& device, const std::vector<char>& code);
		~ShaderModule();

		const class Device& get_device() const { return device; }

		VkPipelineShaderStageCreateInfo create_shaderStage(VkShaderStageFlagBits stage) const;

	private:

		static std::vector<char> read_file(const std::string& filename);

		const class Device& device;

		VULKAN_HANDLES(VkShaderModule, shaderModule)
	};

}
