#include "Chunk.hpp"

#include <iostream>
#include "../../../../Engine/Util/Math.hpp"

#define PROFILE
#include "../../../../Engine/Util/Profile.hpp"\

#include "ChunkDatabase.hpp"

Chunk::Chunk(NodeManager& nodeManager, glm::ivec3 position, ChunkDatabase& chunkDatabase)
	: m_Position(position), m_TransformationMatrix(engine::math::TransMatrix::createTransformationMatrix(position * CHUNK_SIZE)), m_ChunkDatabase(chunkDatabase), m_NodeManager(nodeManager)
{
	for (int x = 0; x < CHUNK_VOLUME; x++)
		m_Nodes.at(x) = nodeManager.getNode(node::grass);
}

Chunk::Chunk(NodeManager& nodeManager, std::vector<Node>& m_Nodes, glm::ivec3& position, ChunkDatabase& chunkDatabase)
	: m_Position(position), m_TransformationMatrix(engine::math::TransMatrix::createTransformationMatrix(position * CHUNK_SIZE)), m_ChunkDatabase(chunkDatabase), m_NodeManager(nodeManager)
{}

Chunk::~Chunk()
{
	if (!m_ChunkRef_front.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y - 1, m_Position.z), false);
	if (!m_ChunkRef_frontRight.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x + 1, m_Position.y, m_Position.z), false);
	if (!m_ChunkRef_back.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y + 1, m_Position.z), false);
	if (!m_ChunkRef_frontLeft.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x - 1, m_Position.y, m_Position.z), false);
	if (!m_ChunkRef_top.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z + 1), false);
	if (!m_ChunkRef_bottom.expired())
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z - 1), false);
}

void Chunk::addNode(Node& node, glm::ivec3 position)
{
	setNode(node, position);

	// front
	if (!position.y)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y - 1, m_Position.z), false);
	}

	// front right
	if (position.x == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x + 1, m_Position.y, m_Position.z), false);
	}

	// back
	if (position.y == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y + 1, m_Position.z), false);
	}

	// front left
	if (!position.x)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x - 1, m_Position.y, m_Position.z), false);
	}

	// top
	if (position.z == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z + 1), false);
	}

	// bottom
	if (!position.z)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z - 1), false);
	}
}

void Chunk::removeNode(glm::ivec3 position)
{
	Node node = Node(node::air);
	setNode(node, position);

	// front
	if (!position.y)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y - 1, m_Position.z), false);
	}

	// front right
	if (position.x == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x + 1, m_Position.y, m_Position.z), false);
	}

	// back
	if (position.y == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y + 1, m_Position.z), false);
	}

	// front left
	if (!position.x)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x - 1, m_Position.y, m_Position.z), false);
	}

	// top
	if (position.z == CHUNK_SIZE - 1)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z + 1), false);
	}

	// bottom
	if (!position.z)
	{
		m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z - 1), false);
	}
}

void Chunk::render(engine::Shader3D& shader)
{
	if (m_VertexArray != nullptr)
	{
		shader.setModelMatrix(m_TransformationMatrix);
		m_VertexArray->render();
	}
}

void Chunk::tick()
{
}

inline Node& Chunk::getNode(glm::ivec3 position)
{
	if (position.x >= CHUNK_SIZE || position.x < 0
	||  position.y >= CHUNK_SIZE || position.y < 0
	||  position.z >= CHUNK_SIZE || position.z < 0)
	{
		Node node = Node(node::air);
		return node;
	}
	else
	{
		uint flat = engine::math::DimensionalAndFlat::getFlatFrom3D(position, CHUNK_SIZE);
		return m_Nodes.at(flat);
	}
}

void Chunk::setNode(Node& node, glm::ivec3 position)
{
	uint flat = engine::math::DimensionalAndFlat::getFlatFrom3D(position, CHUNK_SIZE);

	if (flat >= CHUNK_VOLUME || flat < 0)
	{
		std::cout << "ERROR::<Chunk::setNode>: Out of bounds exception!\n";
	}
	else
	{
		m_Nodes.at(flat) = node;
	}
}

void Chunk::getNeighboringChunks()
{
	if (m_ChunkRef_front.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x, m_Position.y - 1, m_Position.z), m_ChunkRef_front);

	if (m_ChunkRef_frontRight.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x + 1, m_Position.y, m_Position.z), m_ChunkRef_frontRight);

	if (m_ChunkRef_back.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x, m_Position.y + 1, m_Position.z), m_ChunkRef_back);

	if (m_ChunkRef_frontLeft.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x - 1, m_Position.y, m_Position.z), m_ChunkRef_frontLeft);

	if (m_ChunkRef_top.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x, m_Position.y, m_Position.z + 1), m_ChunkRef_top);

	if (m_ChunkRef_bottom.expired())
		m_ChunkDatabase.getChunkWCallback(glm::ivec3(m_Position.x, m_Position.y, m_Position.z - 1), m_ChunkRef_bottom);
}

void Chunk::buildMesh(bool buildNeighbors)
{
	getNeighboringChunks();

	std::vector<engine::NodeVertex> vertices;
	std::vector<uint> indices;

	// Lock is slow for each block
	std::shared_ptr<Chunk> tempChunkRef_front = m_ChunkRef_front.lock();
	std::shared_ptr<Chunk> tempChunkRef_frontRight = m_ChunkRef_frontRight.lock();
	std::shared_ptr<Chunk> tempChunkRef_back = m_ChunkRef_back.lock();
	std::shared_ptr<Chunk> tempChunkRef_frontLeft = m_ChunkRef_frontLeft.lock();
	std::shared_ptr<Chunk> tempChunkRef_top = m_ChunkRef_top.lock();
	std::shared_ptr<Chunk> tempChunkRef_bottom = m_ChunkRef_bottom.lock();

	uint indx = -1;
	uint greatestIndex = -1;
	for (auto& b : m_Nodes)
	{
		++indx;
		if (b.m_ID == node::air)
			continue;

		glm::ivec3 bPosition = engine::math::DimensionalAndFlat::get3DFromFlat(indx, CHUNK_SIZE);

		// Does node occlude the side.
		bool doesFrontOcclude, doesFrontRightOcclude, doesBackOcclude, doesFrontLeftOcclude, doesTopOcclude, doesBottomOcclude;

		// front
		if (bPosition.y)
		{
			doesFrontOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x, bPosition.y - 1, bPosition.z)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_front.expired())
				doesFrontOcclude = m_NodeManager.getNodeType(tempChunkRef_front->getNode(glm::ivec3(bPosition.x, CHUNK_SIZE - 1, bPosition.z)).m_ID)->doesOcclude;
			else
				doesFrontOcclude = true;
		}

		// front right
		if (bPosition.x != CHUNK_SIZE - 1)
		{
			doesFrontRightOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x + 1, bPosition.y, bPosition.z)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_frontRight.expired())
				doesFrontRightOcclude = m_NodeManager.getNodeType(tempChunkRef_frontRight->getNode(glm::ivec3(0, bPosition.y, bPosition.z)).m_ID)->doesOcclude;
			else
				doesFrontRightOcclude = true;
		}

		// back
		if (bPosition.y != CHUNK_SIZE - 1)
		{
			doesBackOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x, bPosition.y + 1, bPosition.z)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_back.expired())
				doesBackOcclude = m_NodeManager.getNodeType(tempChunkRef_back->getNode(glm::ivec3(bPosition.x, 0, bPosition.z)).m_ID)->doesOcclude;
			else
				doesBackOcclude = true;
		}

		// front left
		if (bPosition.x)
		{
			doesFrontLeftOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x - 1, bPosition.y, bPosition.z)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_frontLeft.expired())
				doesFrontLeftOcclude = m_NodeManager.getNodeType(tempChunkRef_frontLeft->getNode(glm::ivec3(CHUNK_SIZE - 1, bPosition.y, bPosition.z)).m_ID)->doesOcclude;
			else
				doesFrontLeftOcclude = true;
		}

		// top
		if (bPosition.z != CHUNK_SIZE - 1)
		{
			doesTopOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x, bPosition.y, bPosition.z + 1)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_top.expired())
				doesTopOcclude = m_NodeManager.getNodeType(tempChunkRef_top->getNode(glm::ivec3(bPosition.x, bPosition.y, 0)).m_ID)->doesOcclude;
			else
				doesTopOcclude = false;
		}

		// bottom
		if (bPosition.z)
		{
			doesBottomOcclude = m_NodeManager.getNodeType(getNode(glm::ivec3(bPosition.x, bPosition.y, bPosition.z - 1)).m_ID)->doesOcclude;
		}
		else
		{
			if (!m_ChunkRef_bottom.expired())
				doesBottomOcclude = m_NodeManager.getNodeType(tempChunkRef_bottom->getNode(glm::ivec3(bPosition.x, bPosition.y, CHUNK_SIZE - 1)).m_ID)->doesOcclude;
			else
				doesBottomOcclude = true;
		}

		std::pair<std::vector<engine::NodeVertex>, std::vector<uint>> bVerticesWIndices = m_NodeManager.getNodeType(b.m_ID)->getVerticesWIndices(b.m_ID, bPosition, greatestIndex,
			doesFrontOcclude, doesFrontRightOcclude,
			doesBackOcclude, doesFrontLeftOcclude,
			doesTopOcclude, doesBottomOcclude);

		if (!bVerticesWIndices.first.size() || !bVerticesWIndices.second.size())
			continue;

		vertices.insert(vertices.end(), bVerticesWIndices.first.begin(), bVerticesWIndices.first.end());
		indices.insert(indices.end(), bVerticesWIndices.second.begin(), bVerticesWIndices.second.end());

		greatestIndex = *std::max_element(bVerticesWIndices.second.begin(), bVerticesWIndices.second.end()); // optimize this
	}

	if(vertices.size() && indices.size())
		m_VertexArray = std::make_unique<engine::gl::VertexArray>(vertices, indices);

	/*if (buildNeighbors)
	{
		if (!m_ChunkRef_front.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y - 1, m_Position.z), false);
		if (!m_ChunkRef_frontRight.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x + 1, m_Position.y, m_Position.z), false);
		if (!m_ChunkRef_back.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y + 1, m_Position.z), false);
		if (!m_ChunkRef_frontLeft.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x - 1, m_Position.y, m_Position.z), false);
		if (!m_ChunkRef_top.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z + 1), false);
		if (!m_ChunkRef_bottom.expired())
			m_ChunkDatabase.addChunkMeshToQueue(glm::ivec3(m_Position.x, m_Position.y, m_Position.z - 1), false);
	}*/
}

//std::cout << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << "\n";
