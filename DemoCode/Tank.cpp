#include "stdafx.h"
#include "Tank.h"



Tank::Tank(const int id)
{
	mMove = 0;
	mBodyRotate = 0;
	mTurretRotate = 0;
	mBarrelRotate = 0;
	mBarrelPitch = 0;
	mHeightOffset = 18;
	mTankBodyMoveFactor = 1.5;
	mTankBodyRotFactor = 1;
	mTankTurretRotFactor = 1;
	mTankBarrelPitchFactor = 1;
	mId = id;
	//tank_state = TANK_STATE_AI;
	tank_state = TANK_STATE_USER; // testing
	ai_state = AI_STATE_ROAMING;
	wander_turning180 = false;
	wander_rotateCounter = 0;
	wander_delayAfterTurning = false;
	mProjectileInitVelocity = 25;
	mTankHealth = 1; // full hp
}


Tank::~Tank(void)
{
}

bool Tank::keyRealesed(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
		case OIS::KC_I:
			mMove += mTankBodyMoveFactor;
			break;

		case OIS::KC_K:
			mMove -= mTankBodyMoveFactor;
			break;

		case OIS::KC_J:
			mBodyRotate -= mTankBodyRotFactor;
			break;

		case OIS::KC_L:
			mBodyRotate += mTankBodyRotFactor;
			break;

		case OIS::KC_LEFT:
			mTurretRotate -= mTankTurretRotFactor;
			break;
 
		case OIS::KC_RIGHT:
			mTurretRotate += mTankTurretRotFactor;
			break;

		case OIS::KC_UP:
			mBarrelRotate -= mTankBarrelPitchFactor;
			break;
 
		case OIS::KC_DOWN:
			mBarrelRotate += mTankBarrelPitchFactor;
			break;
		default:
			break;
	}
	return true;
}

bool Tank::keyPressed(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
		case OIS::KC_I:
			mMove -= mTankBodyMoveFactor;
			break;

		case OIS::KC_K:
			mMove += mTankBodyMoveFactor;
			break;

		case OIS::KC_J:
			mBodyRotate += mTankBodyRotFactor;
			break;

		case OIS::KC_L:
			mBodyRotate -= mTankBodyRotFactor;
			break;

		case OIS::KC_LEFT:
			mTurretRotate += mTankTurretRotFactor;
			break;
 
		case OIS::KC_RIGHT:
			mTurretRotate -= mTankTurretRotFactor;
			break;

		case OIS::KC_UP:
			mBarrelRotate += mTankBarrelPitchFactor;
			break;
 
		case OIS::KC_DOWN:
			mBarrelRotate -= mTankBarrelPitchFactor;
			break;
		case OIS::KC_RSHIFT:
			shootProjectile();
			break;
		default:
			break;
	}

	return true;
}


bool Tank::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

	if (tank_state == TANK_STATE_AI) {
		if (ai_state == AI_STATE_ROAMING) {
			tankWander();
		}
	}

	// Move and rotate the tank
		mTankBodyNode->translate(mMove, 0, 0, Ogre::Node::TransformSpace::TS_LOCAL);
		mTankBodyNode->yaw(Ogre::Degree(mBodyRotate));

		// Get tank's current position
		Ogre::Vector3 tankPosition = mTankBodyNode->getPosition();
		// Move it above the ground
		tankPosition.y = mTerrain->getHeightAtWorldPosition(tankPosition) + mHeightOffset;
		mTankBodyNode->setPosition(tankPosition);

		// Get current tank orientation
		Ogre::Quaternion tankOrientation = mTankBodyNode->getOrientation();

		// Get point on ground where the tank is
		tankPosition.y = mTerrain->getHeightAtWorldPosition(tankPosition);

		// Get a vector pointing in the local x direction
		Ogre::Vector3 v1 = tankPosition + tankOrientation.xAxis();
		v1.y = mTerrain->getHeightAtWorldPosition(v1);
		v1 -= tankPosition;

		// Get a vector pointing in the local -z direction
		Ogre::Vector3 v2 = tankPosition - tankOrientation.zAxis();
		v2.y = mTerrain->getHeightAtWorldPosition(v2);
		v2 -= tankPosition;
	
		// Find the normal vector
		Ogre::Vector3 normal = v1.crossProduct(v2);
		normal.normalise();

		// Rotate the tank turret
		mTankTurretNode->yaw(Ogre::Degree(mTurretRotate));

		// Calculate the tank barrel's current pitch
		mBarrelPitch += mBarrelRotate;

		// Clamp tank barrel rotation between 0 and 30 degrees
		if(mBarrelPitch > 30)
			mBarrelPitch = 30;
		else if(mBarrelPitch < 0)
			mBarrelPitch = 0;
		else
			mTankBarrelNode->roll(Ogre::Degree(-mBarrelRotate));


/*
	// Get the tank's facing direction
	Ogre::Vector3 facing = tankOrientation.xAxis();

	// Find the local -z direction
	Ogre::Vector3 right = facing.crossProduct(normal);
	right.normalise();

	// Find the local x direction
	Ogre::Vector3 forward = normal.crossProduct(right);
	forward.normalise();

	// Orientate the tank
	mTankNode->setOrientation(Ogre::Quaternion(forward, normal, right));
*/

//////////////////////////////////////////////////////////////////////////////////
	// Smoothened the change in orientation by a certain weightage each frame
	float weight = 0.0001;  // Weight of the new normal

	// Get current orientation and local y direction
	Ogre::Quaternion currentOrientation = mTankBodyNode->getOrientation();
	Ogre::Vector3 localY = currentOrientation.yAxis();

	// Compute a small amount to rotate based on weight
	Ogre::Vector3 newNormal = localY * ( 1 - weight ) + normal * weight;

	// Calculate the angle to rotate
	Ogre::Radian inclinationAngle = Ogre::Math::ACos(localY.dotProduct(newNormal));

	// If angle is not 0
	if(inclinationAngle.valueRadians() != 0.0f)
	{
		// Get rotation quaternion
		Ogre::Vector3 inclinationAxis = ( localY.crossProduct( newNormal) ).normalisedCopy();
		Ogre::Quaternion inclination = Ogre::Quaternion(inclinationAngle, inclinationAxis);
		// Orientate entity based on rotation quaternion
		mTankBodyNode->setOrientation( inclination * currentOrientation );
	}

	return true;
}

Ogre::Vector3 Tank::getTurretForwardDirection(){
		Ogre::Quaternion orientation = mTankTurretNode->getOrientation();
		Ogre::Vector3 localY = mTankBodyNode->getPosition() + orientation.yAxis();
		localY.y = mTerrain->getHeightAtWorldPosition(localY);
		localY -= mTankBodyNode->getPosition();
		Ogre::Vector3 nLocalZ = mTankBodyNode->getPosition() - orientation.zAxis();
		nLocalZ.y = mTerrain->getHeightAtWorldPosition(nLocalZ);
		nLocalZ -= mTankBodyNode->getPosition();
		Ogre::Vector3 direction = -localY.crossProduct(nLocalZ);
		direction.normalise();
		return direction;
}
Ogre::Vector3 Tank::getTankForwardDirection(){
		Ogre::Quaternion orientation = mTankBodyNode->getOrientation();
		Ogre::Vector3 localY = mTankBodyNode->getPosition() + orientation.yAxis();
		localY.y = mTerrain->getHeightAtWorldPosition(localY);
		localY -= mTankBodyNode->getPosition();
		Ogre::Vector3 nLocalZ = mTankBodyNode->getPosition() - orientation.zAxis();
		nLocalZ.y = mTerrain->getHeightAtWorldPosition(nLocalZ);
		nLocalZ -= mTankBodyNode->getPosition();
		Ogre::Vector3 direction = -localY.crossProduct(nLocalZ);
		direction.normalise();
		return direction;
	Ogre::Entity* mEntity = static_cast<Ogre::Entity*>(mTankBodyNode->getAttachedObject(0));

}
Ogre::Vector3 Tank::getBarrelYDirection(){
	return mTankBarrelNode->_getDerivedPosition() * Ogre::Vector3::NEGATIVE_UNIT_Z;
}

Ogre::AxisAlignedBox Tank::getBoundingBox() {
	return static_cast<Ogre::Entity*>(mTankBodyNode->getAttachedObject(0))->getBoundingBox();
}


// No input is needed in wander mode..
void Tank::tankWander() {

	// moving towards water?
	if (mTankBodyNode->getPosition().y < 250 && wander_turning180 == false && wander_delayAfterTurning == false) {
		wander_turning180 = true;

		int randomFactor = rand() % 100; // 0 - 99 random nr
		if (randomFactor < 50)
			mBodyRotate = mTankBodyRotFactor;
		else
			mBodyRotate = -mTankBodyRotFactor;

		// stop moving forward..
		mMove = 0;
		// reset counter
		wander_rotateCounter = 0;
	}

	if (wander_turning180) {
		wander_rotateCounter++;
		if (wander_rotateCounter == 180) {
			wander_turning180 = false;
			wander_delayAfterTurning = true;
			// move forward..
			mMove = -mTankBodyMoveFactor;
		}
	} else {
		// move forward..
		mMove = -mTankBodyMoveFactor;

		// rotate?
		int randomFactor = rand() % 100; // 0 - 99 random nr
		if (randomFactor < 25)
			mBodyRotate = mTankBodyRotFactor; // rotate left
		else if (randomFactor < 50)
			mBodyRotate = -mTankBodyRotFactor; // rotate right
		else 
			mBodyRotate = 0;

		wander_delayAfterTurning = false;
	}
}

float Tank::calculateProjectileRange(){
	float T = ((2*mProjectileInitVelocity)*sin(1))/-10;
	float R = mProjectileInitVelocity*T*cos(1);
	return R;
}

void Tank::shootProjectile(){
	// Create unique name
	std::ostringstream oss;
	oss << mBoxCount;
	std::string entityName = "Cube" + oss.str();
	// Increment box count
	mBoxCount++;

	// Create cube mesh with unique name
	Ogre::Entity* projectile = mSceneMgr->createEntity(entityName, "cube.mesh");
	Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();	
	node->attachObject(projectile);
	// Scale it to appropriate size
	node->scale(0.1, 0.1, 0.1);
	node->showBoundingBox(true);
	projectiles.push_back(node);

	// Create a collision shape
	// Note that the size should match the size of the object that will be displayed
	btCollisionShape* collisionShape = new btBoxShape(btVector3(5, 5, 5));

	// The object's starting transformation
	btTransform startingTrans;
	startingTrans.setIdentity();
	startingTrans.setOrigin(convert(mProjectileSpawnNode->_getDerivedPosition()));
	startingTrans.setRotation(btQuaternion(0,0,0,1));

	// Calculate the direction for the linear velocity
				btVector3 linearVelocity(convert(getBarrelYDirection()));
				
				linearVelocity.normalize();
				// Scale to appropriate velocity
				linearVelocity *= 100.0f;

	// Create the rigid body
	btRigidBody* rigidBody = mPhysicsEngine->createRigidBody(10, startingTrans, collisionShape, node);
	rigidBody->setFriction(10);

	// Give the rigid body an initial velocity
	rigidBody->setLinearVelocity(linearVelocity);
}


bool Tank::tankGotHit() {
	// todo, set hp loss based on distance
}

void Tank::setTankStateToAI(bool new_state) 
{	 
	if (new_state) { 
		tank_state = TANK_STATE_AI;
	} else {
		tank_state = TANK_STATE_USER;
	}
	// reset movement variables
	mMove = 0;
	mBodyRotate = 0;
	mBarrelRotate = 0;
	mTurretRotate = 0;
}
