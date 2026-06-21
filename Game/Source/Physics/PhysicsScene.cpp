#include "PhysicsScene.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "JoltMath.hpp"
#include <unordered_set>

namespace Hyper
{
    namespace Internal
    {
        constexpr float POSITION_PRECISION = 1e-3f;

        static inline int64_t Quantize(float value)
        {
            return static_cast<int64_t>(std::round(value / POSITION_PRECISION));
        }

        static inline size_t HashPosition(const JPH::Vec3 &pos)
        {
            int64_t x = Quantize(pos.GetX());
            int64_t y = Quantize(pos.GetY());
            int64_t z = Quantize(pos.GetZ());

            size_t hx = std::hash<int64_t>{}(x);
            size_t hy = std::hash<int64_t>{}(y);
            size_t hz = std::hash<int64_t>{}(z);

            return hx ^ (hy << 1) ^ (hz << 2);
        }

    }

    PhysicsScene::PhysicsScene() : m_JobSystem(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1)
    {
        System.Init(
            /* max bodies */ 10240,
            /* num body mutexes */ 0,
            /* max body pairs */ 10240,
            /* max contact constraints */ 10240,
            m_BLayerInterface,
            m_ObjectVsBroadPhaseLayerFilter,
            m_ObjectVsObjectLayerFilter
        );
    }

    PhysicsScene::~PhysicsScene()
    {

    }

    void PhysicsScene::ComputePoints(const std::vector<Vertex3D>& vertices, const std::vector<uint32> indices, JPH::Array<JPH::Vec3>& target)
    {
        std::unordered_set<size_t> seenHashes;

        for (size_t i = 0; i < indices.size(); ++i)
        {
            uint32_t index = indices[i];
            if (index >= vertices.size()) continue;

            JPH::Vec3 pos = JoltMath::FromFloat3(vertices[index].Position);
            size_t hash = Internal::HashPosition(pos);

            if (seenHashes.insert(hash).second)
            {
                target.push_back(pos);
            }
        }
    }

    JPH::Ref<JPH::Shape> PhysicsScene::CreateConvexHull(const std::vector<Mesh>& meshes)
    {
        JPH::ConvexHullShapeSettings settings;

        for (const Mesh& mesh : meshes)
        {
            for (const JPH::Vec3& point : mesh.ConvexHullShape.mPoints)
            {
                settings.mPoints.push_back(point);
            }
        }

        return settings.Create().Get();
    }

    void PhysicsScene::Simulate(double deltaTime)
    {
        System.Update(deltaTime, 1, &TempAllocator, &m_JobSystem);
    }

    JPH::BodyInterface &PhysicsScene::GetBodyInterface()
    {
        return System.GetBodyInterface();
    }
}
