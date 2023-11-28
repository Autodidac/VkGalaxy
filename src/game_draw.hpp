#ifndef GAMEDRAW_H
#define GAMEDRAW_H

#include "render.hpp"
#include "quickmath.hpp"

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

	//terrain pipeline objects:
	VkDescriptorSetLayout terrainPipelineDescriptorLayout;
	VkPipelineLayout terrainPipelineLayout;
	VkPipeline terrainPipeline;

	VkBuffer terrainVertexBuffer;
	VkDeviceMemory terrainVertexBufferMemory;
	VkBuffer terrainIndexBuffer;
	VkDeviceMemory terrainIndexBufferMemory;

	VkImage terrainTextureAtlas;
	VkImageView terrainTextureAtlasView;
	VkDeviceMemory terrainTextureAtlasMemory;
	VkSampler terrainTextureAtlasSampler;

	VkDescriptorPool terrainDescriptorPool;
	VkDescriptorSet terrainDescriptorSets[FRAMES_IN_FLIGHT];
};

//----------------------------------------------------------------------------//

struct TerrainVertex
{
	qm::vec3 pos;
	qm::vec2 texCoord;
};

struct TerrainMesh
{
	uint64 id;
	qm::vec3 pos;

	uint64 vertexCount;
	TerrainVertex* mesh;
};

//----------------------------------------------------------------------------//

struct Camera
{
	qm::vec3 pos;
	qm::vec3 front;
	qm::vec3 up;

	float fov;
	float nearPlane, farPlane;

	qm::vec3 orient; //TODO: find a better place for this
};

//----------------------------------------------------------------------------//

bool gamedraw_init(DrawState** state);
void gamedraw_quit(DrawState* state);

void gamedraw_draw(DrawState* state, Camera* cam);

#endif