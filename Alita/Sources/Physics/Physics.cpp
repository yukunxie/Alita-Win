//
// Created by realxie on 2019-10-29.
//

#include "Physics.h"
#include "cooking/PxCooking.h"

using namespace physx;

NS_RX_BEGIN

physx::PxDefaultAllocator		 gAllocator;
class RxErrorCallback : public physx::PxErrorCallback
{
public:
	RxErrorCallback()
		:physx::PxErrorCallback()
	{
	}

	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		LOGE("RxErrorCallback file=%s, line=%d, msg=%s", file, line, message);
	}
};

RxErrorCallback	 gErrorCallback;

Physics* Physics::Instance_ = nullptr;

Physics::Physics()
{
	pxFoundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	pxPhysics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation_, physx::PxTolerancesScale(), true);
	physx::PxSceneDesc sceneDesc(pxPhysics_->getTolerancesScale());
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	//sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);
	pxScene_ = pxPhysics_->createScene(sceneDesc);
	PX_ASSERT(pxScene_ != nullptr);
	pxScene_->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0);
	pxScene_->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	pxDefaultMaterial_ = pxPhysics_->createMaterial(0.5, 0.5, 0.5);

	physx::PxTolerancesScale scale;
	physx::PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.
	pxCooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation_, params);
}

void Physics::AddPrimitive(const Transform& transform, const std::vector<TVector3>& vertices, const std::vector<uint32>& indices)
{
	std::vector< physx::PxVec3>verts;
	verts.reserve(vertices.size());
	for (const auto vert : vertices)
	{
		verts.emplace_back(vert.x, vert.y, vert.z);
	}
	std::vector< physx::PxU32> indices32;
	indices32.reserve(indices.size());
	for (auto idx : indices)
	{
		indices32.push_back(idx);
	}
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = verts.size();
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = verts.data();

	meshDesc.triangles.count = indices.size() / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices32.data();

	physx::PxDefaultMemoryOutputStream ostream;
	pxCooking_->cookTriangleMesh(meshDesc, ostream);

	physx::PxDefaultMemoryInputData istream(ostream.getData(), ostream.getSize());
	physx::PxTriangleMesh* mesh = pxPhysics_->createTriangleMesh(istream);

	physx::PxShape* shape = pxPhysics_->createShape(physx::PxTriangleMeshGeometry(mesh), *pxDefaultMaterial_, true);
	physx::PxQuat quatX(transform.Rotation().x, physx::PxVec3(1, 0, 0));
	physx::PxQuat quatY(transform.Rotation().y, physx::PxVec3(0, 1, 0));
	physx::PxQuat quatZ(transform.Rotation().z, physx::PxVec3(0, 0, 1));
	physx::PxQuat quat = quatX * quatY * quatZ;
	physx::PxTransform pxTransform(transform.Position().x, transform.Position().y, transform.Position().z, quat);
	physx::PxRigidStatic* actor = PxCreateStatic(*pxPhysics_, pxTransform, *shape);

	pxScene_->addActor(*actor);

	Tick(0.5f);
}

void Physics::Tick(float dt)
{

	PxRaycastHit hit; hit.shape = NULL;
	PxRaycastBuffer hit1;
	auto ret = pxScene_->raycast({ 0, 5, 0 }, { 0, -1, 0 }, PX_MAX_F32, hit1, PxHitFlag::ePOSITION);

	hit = hit1.block;

	//if (hit.shape)
	//{
	//	const char* shapeName = hit.shape->getName();
	//	if (shapeName)
	//		shdfnd::printFormatted("Picked shape name: %s\n", shapeName);

	//	PxRigidActor* actor = hit.actor;
	//	PX_ASSERT(actor);
	//	mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxRigidDynamic>());
	//	if (!mSelectedActor)
	//		mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxArticulationLink>());

	//	//ML::this is very useful to debug some collision problem
	//	PxTransform t = actor->getGlobalPose();
	//	PX_UNUSED(t);
	//	shdfnd::printFormatted("id = %i\n PxTransform transform(PxVec3(%f, %f, %f), PxQuat(%f, %f, %f, %f))\n", reinterpret_cast<size_t>(actor->userData), t.p.x, t.p.y, t.p.z, t.q.x, t.q.y, t.q.z, t.q.w);
	//}
	//else
	//	mSelectedActor = 0;

	//if (mSelectedActor)
	//{
	//	shdfnd::printFormatted("Actor '%s' picked! (userData: %p)\n", mSelectedActor->getName(), mSelectedActor->userData);

	//	//if its a dynamic rigid body, joint it for dragging purposes:
	//	grabActor(hit.position, rayOrig);
	//}

	/*PxRaycastHit hit;
	PxShape* closestShape;
	pxScene_->raycast({0, 5, 0}, { 0, -5, 0 }, 100, );*/

	/*physx::Ray ray;
	ray.dir -= ray.orig;
	float length = ray.dir.magnitude();
	ray.dir.normalize();*/
}

NS_RX_END