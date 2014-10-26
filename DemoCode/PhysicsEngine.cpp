#include "stdafx.h"
#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() :
	mCollisionConfiguration(0),
	mDispatcher(0),
	mBroadphase(0),
	mSolver(0),
	mDynamicsWorld(0)
{}

PhysicsEngine::~PhysicsEngine()
{
	exitPhysics();
}

void PhysicsEngine::exitPhysics()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=mDynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		mDynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<mCollisionShapes.size();j++)
	{
		btCollisionShape* shape = mCollisionShapes[j];
		delete shape;
	}
	mCollisionShapes.clear();

	delete mDynamicsWorld;
	delete mSolver;
	delete mBroadphase;
	delete mDispatcher;
	delete mCollisionConfiguration;
}

void PhysicsEngine::initPhysics()
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mBroadphase = new btDbvtBroadphase();
	
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	// Create a dynamic simulation environment
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

	// Set the gravity for the simulation
	mDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void PhysicsEngine::update(float deltaTime)
{
	// Simulate dynamics
	mDynamicsWorld->stepSimulation(deltaTime);
}

btRigidBody* PhysicsEngine::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* collisionShape, Ogre::SceneNode* node)
{
	// Check validity of collision shape
	btAssert((!collisionShape || collisionShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	// Push the collision shape into the collision shapes array
	mCollisionShapes.push_back(collisionShape);

	// Rigidbody is dynamic if and only if mass is non zero, otherwise it's static
	bool isDynamic = (mass != 0.f);

	// Calculate moment of inertia
	btVector3 inertia(0, 0, 0);
	if(isDynamic)
		collisionShape->calculateLocalInertia(mass, inertia);

	// Create rigid body
	btMotionState* motionState = new MyMotionState(startTransform, node);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, inertia);
	btRigidBody* rigidBody = new btRigidBody(rbInfo);

	// Add rigid body to dynamic simulation
	mDynamicsWorld->addRigidBody(rigidBody);

	return rigidBody;
}

void PhysicsEngine::createTerrainData(float* ogreTerrainData, int terrainSize, float terrainScale)
{
	// Assume square dimensions
	float* heightfieldData = new float[terrainSize * terrainSize];
	
	// Initialise minimum and maximum heights to be the maximum and minimum values
	int minHeight = 0x7FFFFFFF;
	int maxHeight = 0;

	int heightfieldIndex = 0;
	int row = 0;

	// Parse Ogre terrain data to fill in heightfield data (re-order the data)
	// Find minimum and maximum height values
	for(int index = 0; index < terrainSize * terrainSize; index++, heightfieldIndex++)
	{
		row = index / terrainSize + 1;

		heightfieldData[heightfieldIndex] = ogreTerrainData[index % terrainSize + (terrainSize * terrainSize - row * terrainSize)];

		if (heightfieldData[heightfieldIndex] < minHeight)
		{
			minHeight = heightfieldData[heightfieldIndex];
		}
		else if (heightfieldData[heightfieldIndex] > maxHeight)
		{
			maxHeight = heightfieldData[heightfieldIndex];
		}
	}

	// Create heightfield shape based on heightfield data
	btHeightfieldTerrainShape *heightfieldShape = new btHeightfieldTerrainShape(terrainSize, terrainSize,
					  heightfieldData,
					  0,
					  minHeight, maxHeight,
					  1, PHY_FLOAT, false);

	btAssert(heightfieldShape && "null heightfield");

	// Adjust the heightfield collision shape to match terrain scale
	heightfieldShape->setLocalScaling(btVector3(terrainScale, 1.0f, terrainScale));
	// Set flag to use zigzag subdivision which is used by Ogre terrain
	heightfieldShape->setUseZigzagSubdivision(true);
	
	// Set origin - middle of heightfield + adjust height
	// Bullet sets the vertical centre to be right in the middle of the heightfield
	float adjustHeight = maxHeight - (maxHeight - minHeight)/2;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0, adjustHeight, 0));

	// Create rigidbody for terrain object
	float mass = 0.0f;
	btRigidBody* terrainBody = createRigidBody(mass, transform, heightfieldShape, NULL);

	// Disable debug drawing for the terrain
	terrainBody->setCollisionFlags(terrainBody->getCollisionFlags() | btRigidBody::CF_DISABLE_VISUALIZE_OBJECT);
}
