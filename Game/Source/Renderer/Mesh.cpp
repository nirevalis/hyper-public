#include "Mesh.hpp"

#include <Jolt/Geometry/ConvexHullBuilder.h>
#include <nvrhi/utils.h>
#include <Physics/JoltMath.hpp>

#include "Engine/GameEngine.hpp"
#include "Threading/JobManager.hpp"
#include <unordered_set>
#include <Physics/PhysicsScene.hpp>

namespace Hyper
{
	Mesh::Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices, const struct Material& material)
		: Material(material)
	{
		Update(vertices, indices);
	}

	Mesh::~Mesh()
	{

	}

	nvrhi::rt::GeometryDesc& Mesh::GetGeometry()
	{
		return m_Geometry;
	}

	nvrhi::rt::AccelStructHandle Mesh::GetBLAS()
	{
		return m_BLAS;
	}

	void Mesh::Update(const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices)
	{
		{
			nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

			nvrhi::BufferDesc bufferDesc;
			bufferDesc.byteSize = sizeof(Vertex3D) * vertices.size();
			bufferDesc.isVertexBuffer = true;
			bufferDesc.initialState = nvrhi::ResourceStates::ShaderResource;
			bufferDesc.keepInitialState = true;
			bufferDesc.isAccelStructBuildInput = true;
			bufferDesc.canHaveRawViews = true;

			if (VertexBuffer == nullptr || VertexBuffer->getDesc().byteSize != vertices.size() * sizeof(Vertex3D))
				VertexBuffer = device->createBuffer(bufferDesc);

			bufferDesc.isVertexBuffer = false;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = sizeof(uint32) * indices.size();

			if (IndexBuffer == nullptr || IndexBuffer->getDesc().byteSize != indices.size() * sizeof(uint32))
				IndexBuffer = device->createBuffer(bufferDesc);

			auto& triangles = m_Geometry.geometryData.triangles;
			triangles.indexBuffer = IndexBuffer;
			triangles.vertexBuffer = VertexBuffer;

			triangles.indexFormat = nvrhi::Format::R32_UINT;
			triangles.indexCount = indices.size();

			triangles.vertexFormat = nvrhi::Format::RGB32_FLOAT;
			triangles.vertexStride = sizeof(Vertex3D);
			triangles.vertexCount = vertices.size();

			m_Geometry.geometryType = nvrhi::rt::GeometryType::Triangles;
			m_Geometry.flags = nvrhi::rt::GeometryFlags::Opaque;

			auto blasDesc = nvrhi::rt::AccelStructDesc()
				.setDebugName("BLAS")
				.setIsTopLevel(false)
				.addBottomLevelGeometry(m_Geometry);

			m_BLAS = device->createAccelStruct(blasDesc);

			MeshUploadAsyncTask* task;
			JobManager::Run(
				TaskImportance::Side,
				TaskGraph()
				.AddTask(new MeshUploadAsyncTask(
					VertexBuffer,
					IndexBuffer,
					m_BLAS,
					vertices,
					indices,
					blasDesc
				), ( void** )&task)
				.AddOnFinished([task] ()
					{
						INFO("[ASYNC] Uploaded mesh with {} vertices and {} indices in {}", task->Vertices.size(), task->Indices.size(), task->GetTaskDuration());
						delete task;
					})
			);
		}
		ConvexCreating:
		JPH::Array<JPH::Vec3> points;
		PhysicsScene::ComputePoints(vertices, indices, points);

		ConvexHullShape = JPH::ConvexHullShapeSettings(points);
	}


	MeshUploadAsyncTask::MeshUploadAsyncTask(nvrhi::BufferHandle vertexBuffer, nvrhi::BufferHandle indexBuffer, nvrhi::rt::AccelStructHandle blas, const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices, const nvrhi::rt::AccelStructDesc& blasDesc)
		: VertexBuffer(vertexBuffer),
		IndexBuffer(indexBuffer),
		BLAS(blas),
		Vertices(vertices),
		Indices(indices),
		BLASDesc(blasDesc)
	{
	}

	void MeshUploadAsyncTask::Compute(nvrhi::CommandListHandle cmd)
	{
		cmd->writeBuffer(VertexBuffer, Vertices.data(), Vertices.size() * sizeof(Vertex3D));
		cmd->writeBuffer(IndexBuffer, Indices.data(), Indices.size() * sizeof(uint32));
		cmd->buildBottomLevelAccelStruct(BLAS, BLASDesc.bottomLevelGeometries.data(), BLASDesc.bottomLevelGeometries.size());
	}

	GPUTask::GPUTaskExecutionInfo MeshUploadAsyncTask::GetExecutionInfo()
	{
		GPUTaskExecutionInfo info = { };
		info.QueueType = nvrhi::CommandQueue::Graphics;

		return info;
	}

}
