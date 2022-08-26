#include "Application.hpp"
#include "BottomLevelAccelerationStructure.hpp"
#include "DeviceProcedures.hpp"
#include "RayTracingPipeline.hpp"
#include "ShaderBindingTable.hpp"
#include "TopLevelAccelerationStructure.hpp"
#include "../Assets/Model.hpp"
#include "../Assets/Scene.hpp"
#include "glm/Glm.hpp"
#include "../Vulkan/Buffer.hpp"
#include "../Vulkan/BufferUtil.hpp"
#include "../Vulkan/Image.hpp"
#include "../Vulkan/ImageMemoryBarrier.hpp"
#include "../Vulkan/ImageView.hpp"
#include "../Vulkan/PipelineLayout.hpp"
#include "../Vulkan/SingleTimeCommands.hpp"
#include "../Vulkan/SwapChain.hpp"
#include <chrono>
#include <iostream>
#include <numeric>


namespace Vulkan::RayTracing {

namespace
{
	template <class TAccelerationStructure>
	VkAccelerationStructureBuildSizesInfoKHR GetTotalRequirements(const std::vector<TAccelerationStructure>& accelerationStructures)
	{
		VkAccelerationStructureBuildSizesInfoKHR total{};

		for (const auto& accelerationStructure : accelerationStructures)
		{
			total.accelerationStructureSize += accelerationStructure.BuildSizes().accelerationStructureSize;
			total.buildScratchSize += accelerationStructure.BuildSizes().buildScratchSize;
			total.updateScratchSize += accelerationStructure.BuildSizes().updateScratchSize;
		}

		return total;
	}
}

Application::Application(const WindowConfig& windowConfig, const VkPresentModeKHR presentMode, const bool enableValidationLayers) :
	Vulkan::Application(windowConfig, presentMode, enableValidationLayers)
{
}

Application::~Application()
{
	Application::delete_swapChain();
	DeleteAccelerationStructures();

	rayTracingProperties_.reset();
	deviceProcedures_.reset();
}

void Application::set_physical_device(
	VkPhysicalDevice physicalDevice,
	std::vector<const char*>& requiredExtensions,
	VkPhysicalDeviceFeatures& deviceFeatures,
	void* nextDeviceFeatures)
{
	// Required extensions.
	requiredExtensions.insert(requiredExtensions.end(),
	{	
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
	});

	// Required device features.
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	bufferDeviceAddressFeatures.pNext = nextDeviceFeatures;
	bufferDeviceAddressFeatures.bufferDeviceAddress = true;

	VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
	indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	indexingFeatures.pNext = &bufferDeviceAddressFeatures;
	indexingFeatures.runtimeDescriptorArray = true;
	indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
	accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	accelerationStructureFeatures.pNext = &indexingFeatures;
	accelerationStructureFeatures.accelerationStructure = true;
	
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};
	rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	rayTracingFeatures.pNext = &accelerationStructureFeatures;
	rayTracingFeatures.rayTracingPipeline = true;

	Vulkan::Application::set_physical_device(physicalDevice, requiredExtensions, deviceFeatures, &rayTracingFeatures);
}

void Application::on_device_set()
{
	Vulkan::Application::on_device_set();

	deviceProcedures_.reset(new DeviceProcedures(get_device()));
	rayTracingProperties_.reset(new RayTracingProperties(get_device()));
}

void Application::CreateAccelerationStructures()
{
	const auto timer = std::chrono::high_resolution_clock::now();

	SingleTimeCommands::Submit(get_commandPool(), [this](VkCommandBuffer commandBuffer)
	{
		CreateBottomLevelStructures(commandBuffer);
		CreateTopLevelStructures(commandBuffer);
	});

	topScratchBuffer_.reset();
	topScratchBufferMemory_.reset();
	bottomScratchBuffer_.reset();
	bottomScratchBufferMemory_.reset();

	const auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();
	std::cout << "- built acceleration structures in " << elapsed << "s" << std::endl;
}

void Application::DeleteAccelerationStructures()
{
	topAs_.clear();
	instancesBuffer_.reset();
	instancesBufferMemory_.reset();
	topScratchBuffer_.reset();
	topScratchBufferMemory_.reset();
	topBuffer_.reset();
	topBufferMemory_.reset();

	bottomAs_.clear();
	bottomScratchBuffer_.reset();
	bottomScratchBufferMemory_.reset();
	bottomBuffer_.reset();
	bottomBufferMemory_.reset();
}

void Application::create_swapChain()
{
	Vulkan::Application::create_swapChain();

	CreateOutputImage();

	rayTracingPipeline_.reset(new RayTracingPipeline(*deviceProcedures_, get_swapChain(), topAs_[0], *accumulationImageView_, *outputImageView_, get_uniformBuffer(), get_scene()));

	const std::vector<ShaderBindingTable::Entry> rayGenPrograms = { {rayTracingPipeline_->RayGenShaderIndex(), {}} };
	const std::vector<ShaderBindingTable::Entry> missPrograms = { {rayTracingPipeline_->MissShaderIndex(), {}} };
	const std::vector<ShaderBindingTable::Entry> hitGroups = { {rayTracingPipeline_->TriangleHitGroupIndex(), {}}, {rayTracingPipeline_->ProceduralHitGroupIndex(), {}} };

	shaderBindingTable_.reset(new ShaderBindingTable(*deviceProcedures_, *rayTracingPipeline_, *rayTracingProperties_, rayGenPrograms, missPrograms, hitGroups));
}

void Application::delete_swapChain()
{
	shaderBindingTable_.reset();
	rayTracingPipeline_.reset();
	outputImageView_.reset();
	outputImage_.reset();
	outputImageMemory_.reset();
	accumulationImageView_.reset();
	accumulationImage_.reset();
	accumulationImageMemory_.reset();

	Vulkan::Application::delete_swapChain();
}

void Application::render(VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
	const auto extent = get_swapChain().get_extent();
	VkDescriptorSet descriptorSets[] = { rayTracingPipeline_->DescriptorSet(imageIndex) };
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	// Acquire destination images for rendering.
	ImageMemoryBarrier::Insert(commandBuffer, accumulationImage_->Handle(), subresourceRange, 0,
		VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	ImageMemoryBarrier::Insert(commandBuffer, outputImage_->Handle(), subresourceRange, 0,
		VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	// Bind ray tracing pipeline.
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rayTracingPipeline_->Handle());
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rayTracingPipeline_->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);
	// Describe the shader binding table.
	VkStridedDeviceAddressRegionKHR raygenShaderBindingTable = {};
	raygenShaderBindingTable.deviceAddress = shaderBindingTable_->RayGenDeviceAddress();
	raygenShaderBindingTable.stride = shaderBindingTable_->RayGenEntrySize();
	raygenShaderBindingTable.size = shaderBindingTable_->RayGenSize();
	VkStridedDeviceAddressRegionKHR missShaderBindingTable = {};
	missShaderBindingTable.deviceAddress = shaderBindingTable_->MissDeviceAddress();
	missShaderBindingTable.stride = shaderBindingTable_->MissEntrySize();
	missShaderBindingTable.size = shaderBindingTable_->MissSize();
	VkStridedDeviceAddressRegionKHR hitShaderBindingTable = {};
	hitShaderBindingTable.deviceAddress = shaderBindingTable_->HitGroupDeviceAddress();
	hitShaderBindingTable.stride = shaderBindingTable_->HitGroupEntrySize();
	hitShaderBindingTable.size = shaderBindingTable_->HitGroupSize();
	VkStridedDeviceAddressRegionKHR callableShaderBindingTable = {};
	// Execute ray tracing shaders.
	deviceProcedures_->vkCmdTraceRaysKHR(commandBuffer,
		&raygenShaderBindingTable, &missShaderBindingTable, &hitShaderBindingTable, &callableShaderBindingTable,
		extent.width, extent.height, 1);
	// Acquire output image and swap-chain image for copying.
	ImageMemoryBarrier::Insert(commandBuffer, outputImage_->Handle(), subresourceRange, 
		VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	ImageMemoryBarrier::Insert(commandBuffer, get_swapChain().Images()[imageIndex], subresourceRange, 0,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	// Copy output image into swap-chain image.
	VkImageCopy copyRegion;
	copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copyRegion.dstOffset = { 0, 0, 0 };
	copyRegion.extent = { extent.width, extent.height, 1 };
	vkCmdCopyImage(commandBuffer,
		outputImage_->Handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		get_swapChain().Images()[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copyRegion);
	ImageMemoryBarrier::Insert(commandBuffer, get_swapChain().Images()[imageIndex], subresourceRange, VK_ACCESS_TRANSFER_WRITE_BIT,
		0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void Application::CreateBottomLevelStructures(VkCommandBuffer commandBuffer)
{
	const auto& scene = get_scene();
	//const auto& debugUtils = get_device().DebugUtils();
	
	// Bottom level acceleration structure
	// Triangles via vertex buffers. Procedurals via AABBs.
	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;
	uint32_t aabbOffset = 0;

	for (const auto& model : scene.Models())
	{
		const auto vertexCount = static_cast<uint32_t>(model.NumberOfVertices());
		const auto indexCount = static_cast<uint32_t>(model.NumberOfIndices());
		BottomLevelGeometry geometries;
		
		model.Procedural()
			? geometries.AddGeometryAabb(scene, aabbOffset, 1, true)
			: geometries.AddGeometryTriangles(scene, vertexOffset, vertexCount, indexOffset, indexCount, true);

		bottomAs_.emplace_back(*deviceProcedures_, *rayTracingProperties_, geometries);

		vertexOffset += vertexCount * sizeof(Assets::Vertex);
		indexOffset += indexCount * sizeof(uint32_t);
		aabbOffset += sizeof(VkAabbPositionsKHR);
	}

	// Allocate the structures memory.
	const auto total = GetTotalRequirements(bottomAs_);

	bottomBuffer_.reset(new Buffer(get_device(), total.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
	bottomBufferMemory_.reset(new DeviceMemory(bottomBuffer_->allocate_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	bottomScratchBuffer_.reset(new Buffer(get_device(), total.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
	bottomScratchBufferMemory_.reset(new DeviceMemory(bottomScratchBuffer_->allocate_memory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	/*debugUtils.SetObjectName(bottomBuffer_->Handle(), "BLAS Buffer");
	debugUtils.SetObjectName(bottomBufferMemory_->Handle(), "BLAS Memory");
	debugUtils.SetObjectName(bottomScratchBuffer_->Handle(), "BLAS Scratch Buffer");
	debugUtils.SetObjectName(bottomScratchBufferMemory_->Handle(), "BLAS Scratch Memory");*/

	// Generate the structures.
	VkDeviceSize resultOffset = 0;
	VkDeviceSize scratchOffset = 0;

	for (size_t i = 0; i != bottomAs_.size(); ++i)
	{
		bottomAs_[i].Generate(commandBuffer, *bottomScratchBuffer_, scratchOffset, *bottomBuffer_, resultOffset);
		
		resultOffset += bottomAs_[i].BuildSizes().accelerationStructureSize;
		scratchOffset += bottomAs_[i].BuildSizes().buildScratchSize;

		//debugUtils.SetObjectName(bottomAs_[i].Handle(), ("BLAS #" + std::to_string(i)).c_str());
	}
}

void Application::CreateTopLevelStructures(VkCommandBuffer commandBuffer)
{
	const auto& scene = get_scene();
	//const auto& debugUtils = get_device().DebugUtils();

	// Top level acceleration structure
	std::vector<VkAccelerationStructureInstanceKHR> instances;

	// Hit group 0: triangles
	// Hit group 1: procedurals
	uint32_t instanceId = 0;

	for (const auto& model : scene.Models())
	{
		instances.push_back(TopLevelAccelerationStructure::CreateInstance(
			bottomAs_[instanceId], glm::mat4(1), instanceId, model.Procedural() ? 1 : 0));
		instanceId++;
	}

	// Create and copy instances buffer (do it in a separate one-time synchronous command buffer).
	BufferUtil::CreateDeviceBuffer(get_commandPool(), "TLAS Instances", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, instances, instancesBuffer_, instancesBufferMemory_);

	// Memory barrier for the bottom level acceleration structure builds.
	AccelerationStructure::MemoryBarrier(commandBuffer);
	
	topAs_.emplace_back(*deviceProcedures_, *rayTracingProperties_, instancesBuffer_->get_device_address(), static_cast<uint32_t>(instances.size()));

	// Allocate the structure memory.
	const auto total = GetTotalRequirements(topAs_);

	topBuffer_.reset(new Buffer(get_device(), total.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
	topBufferMemory_.reset(new DeviceMemory(topBuffer_->allocate_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	topScratchBuffer_.reset(new Buffer(get_device(), total.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
	topScratchBufferMemory_.reset(new DeviceMemory(topScratchBuffer_->allocate_memory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

	
	/*debugUtils.SetObjectName(topBuffer_->Handle(), "TLAS Buffer");
	debugUtils.SetObjectName(topBufferMemory_->Handle(), "TLAS Memory");
	debugUtils.SetObjectName(topScratchBuffer_->Handle(), "TLAS Scratch Buffer");
	debugUtils.SetObjectName(topScratchBufferMemory_->Handle(), "TLAS Scratch Memory");
	debugUtils.SetObjectName(instancesBuffer_->Handle(), "TLAS Instances Buffer");
	debugUtils.SetObjectName(instancesBufferMemory_->Handle(), "TLAS Instances Memory");*/

	// Generate the structures.
	topAs_[0].Generate(commandBuffer, *topScratchBuffer_, 0, *topBuffer_, 0);

	//debugUtils.SetObjectName(topAs_[0].Handle(), "TLAS");
}

void Application::CreateOutputImage()
{
	const auto extent = get_swapChain().get_extent();
	const auto format = get_swapChain().get_format();
	const auto tiling = VK_IMAGE_TILING_OPTIMAL;

	accumulationImage_.reset(new Image(get_device(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT));
	accumulationImageMemory_.reset(new DeviceMemory(accumulationImage_->allocate_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	accumulationImageView_.reset(new ImageView(get_device(), accumulationImage_->Handle(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT));

	outputImage_.reset(new Image(get_device(), extent, format, tiling, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
	outputImageMemory_.reset(new DeviceMemory(outputImage_->allocate_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	outputImageView_.reset(new ImageView(get_device(), outputImage_->Handle(), format, VK_IMAGE_ASPECT_COLOR_BIT));

	//const auto& debugUtils = get_device().DebugUtils();
	/*
	debugUtils.SetObjectName(accumulationImage_->Handle(), "Accumulation Image");
	debugUtils.SetObjectName(accumulationImageMemory_->Handle(), "Accumulation Image Memory");
	debugUtils.SetObjectName(accumulationImageView_->Handle(), "Accumulation ImageView");
	
	debugUtils.SetObjectName(outputImage_->Handle(), "Output Image");
	debugUtils.SetObjectName(outputImageMemory_->Handle(), "Output Image Memory");
	debugUtils.SetObjectName(outputImageView_->Handle(), "Output ImageView");*/

}

}
