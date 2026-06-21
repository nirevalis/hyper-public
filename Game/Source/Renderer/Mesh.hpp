#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <vector>
#include <Core/Math/Matrix4.hpp>
#include "Core/Math/Float3.hpp"
#include <Renderer/GPUTask.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

#include "Material.hpp"

namespace Hyper
{
	struct Vertex3D
	{
		Float3 Position;
		Float2 TexCoord;
		Float3 Normal;

		Vertex3D()
		{

		}

		Vertex3D(const Float3& position, const Float2& texCoord, const Float3& normal) :
			Position(position),
			TexCoord(texCoord),
			Normal(normal)
		{

		}
	};

	class HYPER_API MeshUploadAsyncTask : public GPUTask
	{
	public:
		nvrhi::IBuffer* VertexBuffer = nullptr;
		nvrhi::IBuffer* IndexBuffer = nullptr;
		nvrhi::rt::IAccelStruct* BLAS = nullptr;
		nvrhi::rt::AccelStructDesc BLASDesc;

		std::vector<Vertex3D> Vertices;
		std::vector<uint32> Indices;

		MeshUploadAsyncTask(nvrhi::BufferHandle vertexBuffer, nvrhi::BufferHandle indexBuffer, nvrhi::rt::AccelStructHandle blas, const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices, const nvrhi::rt::AccelStructDesc& blasDesc);
		~MeshUploadAsyncTask() = default;

		void Compute(nvrhi::CommandListHandle cmd) override;
		GPUTaskExecutionInfo GetExecutionInfo() override;
	};

	class HYPER_API Mesh
	{
	private:
		nvrhi::rt::GeometryDesc m_Geometry;
		nvrhi::rt::AccelStructHandle m_BLAS;
	public:
		nvrhi::BufferHandle VertexBuffer = nullptr;
		nvrhi::BufferHandle IndexBuffer = nullptr;
		Material Material;
		JPH::ConvexHullShapeSettings ConvexHullShape;

		Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices, const struct Material& material);
		~Mesh();

		void Update(const std::vector<Vertex3D>& vertices, const std::vector<uint32>& indices);

		nvrhi::rt::GeometryDesc& GetGeometry();
		nvrhi::rt::AccelStructHandle GetBLAS();
	};
}
