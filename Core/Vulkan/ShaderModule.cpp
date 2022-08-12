#include "ShaderModule.hpp"
#include "Device.hpp"
#include <fstream>

namespace Vulkan {

ShaderModule::ShaderModule(const class Device& device, const std::string& filename) :
	ShaderModule(device, read_file(filename))
{
}

ShaderModule::ShaderModule(const class Device& device, const std::vector<char>& code) :
	device(device)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	Check(vkCreateShaderModule(device.Handle(), &createInfo, nullptr, &shaderModule),
		"create shader module");
}

ShaderModule::~ShaderModule()
{
	if (shaderModule != nullptr)
	{
		vkDestroyShaderModule(device.Handle(), shaderModule, nullptr);
		shaderModule = nullptr;
	}
}

VkPipelineShaderStageCreateInfo ShaderModule::create_shaderStage(VkShaderStageFlagBits stage) const
{
	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = stage;
	createInfo.module = shaderModule;
	createInfo.pName = "main";

	return createInfo;
}

std::vector<char> ShaderModule::read_file(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "failed to open file '" + filename + "'" << std::endl;

	}

	const auto fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

}
