#include "PhysicsWorld.h"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <iostream>

namespace dhm::world {

PhysicsWorld::PhysicsWorld() = default;

PhysicsWorld::~PhysicsWorld() {
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void PhysicsWorld::initialize() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    temp_allocator_ = std::make_unique<JPH::TempAllocatorImpl>(16 * 1024 * 1024); // 16 MB scratch buffer
    job_system_ = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1
    );

    constexpr JPH::uint MAX_BODIES = 1024;
    constexpr JPH::uint NUM_BODY_MUTEXES = 0;
    constexpr JPH::uint MAX_BODY_PAIRS = 1024;
    constexpr JPH::uint MAX_CONTACT_CONSTRAINTS = 1024;

    physics_system_.Init(
        MAX_BODIES,
        NUM_BODY_MUTEXES,
        MAX_BODY_PAIRS,
        MAX_CONTACT_CONSTRAINTS,
        bp_layer_interface_,
        obp_filter_,
        ol_filter_
    );

    create_ground_plane();
    physics_system_.OptimizeBroadPhase();
    std::cout << "[PhysicsWorld] Jolt Physics initialized with flat terrain." << std::endl;
}

void PhysicsWorld::create_ground_plane(float half_extent) {
    JPH::BodyInterface& body_interface = physics_system_.GetBodyInterface();
    
    // Flat static floor box
    JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(half_extent, 1.0f, half_extent));
    JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    
    JPH::BodyCreationSettings floor_settings(
        floor_shape_result.Get(),
        JPH::RVec3(0.0f, -1.0f, 0.0f),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        Layers::NON_MOVING
    );

    JPH::Body* floor = body_interface.CreateBody(floor_settings);
    body_interface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);
}

void PhysicsWorld::step(float delta_time_sec) {
    constexpr int COLLISION_STEPS = 1;
    physics_system_.Update(delta_time_sec, COLLISION_STEPS, temp_allocator_.get(), job_system_.get());
}

} // namespace dhm::world
