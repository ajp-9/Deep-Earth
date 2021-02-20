#pragma once

#include "../Node/Raw/Node.hpp"
#include "../Light/Light.hpp"
#include "../../../../Engine/OpenGL/VertexArray.hpp"
#include "../../../../Engine/Renderer/Shader/Shader3D.hpp"
#include "../Node/NodeManager.hpp"

#define CHUNK_SIZE 32
#define CHUNK_VOLUME 32768 // 32*32*32

class ChunkDatabase;

class Chunk
{
public:

	Chunk(NodeManager& nodeManager, glm::ivec3 position, ChunkDatabase& chunkDatabase);
	Chunk(NodeManager& nodeManager, std::vector<Node>& m_Nodes, glm::ivec3& position, ChunkDatabase& chunkDatabase);
	~Chunk();

	void render(engine::Shader3D& shader);
	void tick();

	// For player placing blocks
	void addNode(Node& node, glm::ivec3 nodePosition);
	// Sets node to air
	void removeNode(glm::ivec3 nodePosition);

	const Node getNode(glm::ivec3 nodePosition) const;
	void setNode(Node node, glm::ivec3 nodePosition);

	inline const glm::ivec3& getPosition() const { return m_Position; }

	void getNeighboringChunks();

	// Run after ALL chunks have been loaded.
	void buildMesh(bool buildNeighbors);

private:

	glm::ivec3 m_Position;

	std::unique_ptr<engine::gl::VertexArray> m_VertexArray;

	glm::mat4 m_TransformationMatrix;

	ChunkDatabase& m_ChunkDatabase;
	NodeManager& m_NodeManager;

	std::weak_ptr<Chunk> m_ChunkRef_front;
	std::weak_ptr<Chunk> m_ChunkRef_frontRight;
	std::weak_ptr<Chunk> m_ChunkRef_back;
	std::weak_ptr<Chunk> m_ChunkRef_frontLeft;
	std::weak_ptr<Chunk> m_ChunkRef_top;
	std::weak_ptr<Chunk> m_ChunkRef_bottom;

	std::vector<Node> m_Nodes = std::vector<Node>(CHUNK_VOLUME);
	std::vector<Light> m_LightMap = std::vector<Light>(CHUNK_VOLUME);
};