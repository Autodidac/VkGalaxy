#ifndef DRAW_H
#define DRAW_H

#include "libs/render.hpp"
#include "libs/quickmath.hpp"

//----------------------------------------------------------------------------//

#define FRAMES_IN_FLIGHT 2

struct DrawState
{
	RenderInstance* instance;

	//drawing objects:
	VkFormat depthFormat;
	VkImage finalDepthImage;
	VkImageView finalDepthView;
	VkDeviceMemory finalDepthMemory;

	VkRenderPass finalRenderPass;

	uint32 framebufferCount;
	VkFramebuffer* framebuffers;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffers[FRAMES_IN_FLIGHT];

	VkSemaphore imageAvailableSemaphores[FRAMES_IN_FLIGHT];
	VkSemaphore renderFinishedSemaphores[FRAMES_IN_FLIGHT];
	VkFence inFlightFences[FRAMES_IN_FLIGHT];

	VkBuffer cameraBuffers[FRAMES_IN_FLIGHT];
	VkDeviceMemory cameraBuffersMemory[FRAMES_IN_FLIGHT];
	VkBuffer cameraStagingBuffer;
	VkDeviceMemory cameraStagingBufferMemory;

	//quad vertex buffers:
	VkBuffer quadVertexBuffer;
	VkDeviceMemory quadVertexBufferMemory;
	VkBuffer quadIndexBuffer;
	VkDeviceMemory quadIndexBufferMemory;

	//grid pipeline objects:
	VkDescriptorSetLayout gridPipelineDescriptorLayout;
	VkPipelineLayout gridPipelineLayout;
	VkPipeline gridPipeline;

	VkDescriptorPool gridDescriptorPool;
	VkDescriptorSet gridDescriptorSets[FRAMES_IN_FLIGHT];

	//galaxy particle pipeline objects:
	uint64 numGparticles;

	VkDescriptorSetLayout gparticleRenderPipelineDescriptorLayout;
	VkPipelineLayout gparticleRenderPipelineLayout;
	VkPipeline gparticleRenderPipeline;
	VkDescriptorPool gparticleRenderDescriptorPool;
	VkDescriptorSet gparticleRenderDescriptorSets[FRAMES_IN_FLIGHT];

	VkDescriptorSetLayout gparticleComputePipelineDescriptorLayout;
	VkPipelineLayout gparticleComputePipelineLayout;
	VkPipeline gparticleComputePipeline;
	VkDescriptorPool gparticleComputeDescriptorPool;
	VkDescriptorSet gparticleComputeDescriptorSet;

	VkDeviceSize gparticleBufferSize;
	VkBuffer gparticleBuffer;
	VkDeviceMemory gparticleBufferMemory;
};

//----------------------------------------------------------------------------//

struct Vertex
{
	qm::vec3 pos;
	qm::vec2 texCoord;
};

struct GalaxyParticle
{
	qm::vec3 pos;
	float scale;
};

//----------------------------------------------------------------------------//

struct DrawParams
{
	struct
	{
		qm::vec3 pos;
		qm::vec3 up;
		qm::vec3 target;

		float dist;

		float fov;
	} cam;

	uint32 numParticles;
	qm::mat4* particleTransforms;
};

//----------------------------------------------------------------------------//

bool draw_init(DrawState** state);
void draw_quit(DrawState* state);

void draw_render(DrawState* state, DrawParams* params);

#endif