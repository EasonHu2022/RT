#include "Sampler.hpp"
#include "Device.hpp"

namespace Vulkan {

Sampler::Sampler(const class Device& _device, const SamplerConfig& config) :
	device(_device)
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = config.MagFilter;
	samplerInfo.minFilter = config.MinFilter;
	samplerInfo.addressModeU = config.AddressModeU;
	samplerInfo.addressModeV = config.AddressModeV;
	samplerInfo.addressModeW = config.AddressModeW;
	samplerInfo.anisotropyEnable = config.AnisotropyEnable;
	samplerInfo.maxAnisotropy = config.MaxAnisotropy;
	samplerInfo.borderColor = config.BorderColor;
	samplerInfo.unnormalizedCoordinates = config.UnnormalizedCoordinates;
	samplerInfo.compareEnable = config.CompareEnable;
	samplerInfo.compareOp = config.CompareOp;
	samplerInfo.mipmapMode = config.MipmapMode;
	samplerInfo.mipLodBias = config.MipLodBias;
	samplerInfo.minLod = config.MinLod;
	samplerInfo.maxLod = config.MaxLod;

	Check(vkCreateSampler(device.Handle(), &samplerInfo, nullptr, &sampler), "failed to create sampler");

}

Sampler::~Sampler()
{
	if (sampler != nullptr)
	{
		vkDestroySampler(device.Handle(), sampler, nullptr);
		sampler = nullptr;
	}
}

}
