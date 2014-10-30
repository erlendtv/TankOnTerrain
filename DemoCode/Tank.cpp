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
	mTankBodyMoveFactor = 200;
	mTankBodyRotFactor = 100;
	mTankTurretRotFactor = 100;
	mTankBarrelPitchFactor = 100;
	mId = id;
	//tank_state = TANK_STATE_AI;
	tank_state = TANK_STATE_USER; // testing
	ai_state = AI_STATE_ROAMING;
	wander_turning180 = false;
	wander_rotateCounter = 0;
	wander_delayAfterTurning = false;
	mTankHealth = 1; // full hp
	attack_rotating_body = false;
	attack_move = false;
	attack_move_counter = 0;
	mProjectileInitVelocity = 800;
	mSmokeSystemCount = 0;
	ready_to_shoot = 1000;
	mKills = 0;
}


Tank::~Tank(void)
{
}

bool Tank::keyRealesed(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
		case OIS::KC_W:
			mMove += mTankBodyMoveFactor;
			break;

		case OIS::KC_S:
			mMove -= mTankBodyMoveFactor;
			break;

		case OIS::KC_A:
			mBodyRotate -= mTankBodyRotFactor;
			break;

		case OIS::KC_D:
			mBodyRotate += mTankBodyRotFactor;
			break;

		case OIS::KC_LEFT:
			mTurretRotate -= mTankTurretRotFactor;
			break;
 
		case OIS::KC_RIGHT:
			mTurretRotate += mTankTurretRotFactor;;
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
		case OIS::KC_W:
			mMove -= mTankBodyMoveFactor;
			break;

		case OIS::KC_S:
			mMove += mTankBodyMoveFactor;
			break;

		case OIS::KC_A:
			mBodyRotate += mTankBodyRotFactor;
			break;

		case OIS::KC_D:
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
		case OIS::KC_SPACE:
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

			int i = 0;
			for(std::vector<Tank>::iterator it = mTanks->begin(); it != mTanks->end(); ++it) {
				if (it->getId() != getId()) {
					if ((mTankBodyNode->getPosition() - mTanks->at(i).mTankBodyNode->getPosition()).length() < 500) {
						mCurrentlyAttacking = &mTanks->at(i);
						ai_state = AI_STATE_ATTACKING;
						mBodyRotate = 0;
						mMove = 0;
					}
				}
				i++;
			}

		} else if (ai_state == AI_STATE_ATTACKING) {
			// todo: check if mCurrentlyAttacking tank exists..
			tankAttacking(mCurrentlyAttacking);

			if (attack_move) {
				tankWander();
				attack_move_counter++;
				if (attack_move_counter > 1000) {
					attack_move = false;
					mMove = 0;
					mBodyRotate = 0;
				}
			} else {
				int randomInt = rand() % 1000;
				if (randomInt < 1) {
					attack_move = true;
					attack_move_counter = 0;
				}
			}
		}
	}

	// Move and rotate the tank
	mTankBodyNode->translate(mMove * evt.timeSinceLastFrame, 0, 0, Ogre::Node::TransformSpace::TS_LOCAL);
	mTankBodyNode->yaw(Ogre::Degree(mBodyRotate * evt.timeSinceLastFrame));

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
		mTankTurretNode->yaw(Ogre::Degree(mTurretRotate * evt.timeSinceLastFrame));

		// Calculate the tank barrel's current pitch
		mBarrelPitch += (mBarrelRotate * evt.timeSinceLastFrame);

		// Clamp tank barrel rotation between 0 and 30 degrees
		if(mBarrelPitch > 30)
			mBarrelPitch = 30;
		else if(mBarrelPitch < 0)
			mBarrelPitch = 0;
		else
			mTankBarrelNode->roll(Ogre::Degree(-mBarrelRotate * evt.timeSinceLastFrame));


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
		Ogre::Quaternion orientation = mTankTurretNode->_getDerivedOrientation();
		Ogre::Vector3 localY = mTankTurretNode->_getDerivedPosition() + orientation.yAxis();
		localY.y = mTerrain->getHeightAtWorldPosition(localY);
		localY -= mTankTurretNode->_getDerivedPosition();
		Ogre::Vector3 nLocalZ = mTankTurretNode->_getDerivedPosition() - orientation.zAxis();
		nLocalZ.y = mTerrain->getHeightAtWorldPosition(nLocalZ);
		nLocalZ -= mTankTurretNode->_getDerivedPosition();
		Ogre::Vector3 direction = localY.crossProduct(nLocalZ);
		direction.normalise();
		return -direction;
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
	std::string entityName = "Cube" + std::to_string(mId) + oss.str();
	// Increment box count
	mBoxCount++;

	// Create cube mesh with unique name
	Ogre::Entity* projectile = mSceneMgr->createEntity(entityName, "sphere.mesh");
	projectile->setMaterialName("Examples/EnvMappedRustySteel");
	Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();	
	node->attachObject(projectile);
	// Scale it to appropriate size
	node->scale(0.075, 0.075, 0.075);
	(*projectiles).push_back(node);

	// Create a collision shape
	// Note that the size should match the size of the object that will be displayed
	//btCollisionShape* collisionShape = new btBoxShape(btVector3(5, 5, 5));
	btCollisionShape* collisionShape = new btSphereShape(1);

	// The object's starting transformation
	btTransform startingTrans;
	startingTrans.setIdentity();
	startingTrans.setOrigin(convert(mProjectileSpawnNode->_getDerivedPosition()));
	startingTrans.setRotation(btQuaternion(0,0,0,1));

	// Calculate the direction for the linear velocity
	btVector3 linearVelocity(convert(getTurretForwardDirection()));
	// Add the pitch of the barrel
	linearVelocity.setY(mBarrelPitch * 0.01);
	linearVelocity.normalize();
	
	// Scale to appropriate velocity
	linearVelocity *= mProjectileInitVelocity;

	// Create the rigid body
	btRigidBody* rigidBody = mPhysicsEngine->createRigidBody(25, startingTrans, collisionShape, node);
	rigidBody->setFriction(10);

	// Give the rigid body an initial velocity
	rigidBody->setLinearVelocity(linearVelocity);

	createSmokeParticleSystem();
}


void Tank::tankAttacking(Tank* tank_to_attack) {

	/* move turret towards target! */
	Ogre::Vector3 destination = tank_to_attack->mTankBodyNode->_getDerivedPosition();    
	Ogre::Vector3 direction = destination - mTankBodyNode->_getDerivedPosition(); 
	Ogre::Vector3 forward;
	forward = mTankTurretNode->_getDerivedOrientation() * Ogre::Vector3(-1, 0 ,0);
	forward.y = 0;                                                    // Ignore pitch difference angle
	direction.y = 0;
	direction.normalise();
	forward.normalise();
	Ogre::Quaternion q = forward.getRotationTo(direction);
	mTankTurretNode->rotate(q);

	// shooting distance?
	direction = destination - mTankBodyNode->_getDerivedPosition(); 
	if (direction.length() < 500) {
		// shoot
		if (ready_to_shoot == 0) {
			shootProjectile();
			ready_to_shoot = 500;
		} else {
			ready_to_shoot--;
		}

		mMove = 0;
	} else {
		// move tank body against target and drive
		forward = mTankBodyNode->_getDerivedOrientation() * Ogre::Vector3(-1, 0 ,0);
		forward.y = 0;                                                    // Ignore pitch difference angle
		direction.y = 0;
		direction.normalise();
		forward.normalise();
		Ogre::Quaternion q = forward.getRotationTo(direction);
		mTankBodyNode->rotate(q);

		// drive
		mMove = -1;
	}

}

void Tank::tankGotHit() {
	// todo, set hp loss based on distance'
	mTankHealth -= 0.25;

	float healthBarAdjuster = (1.0 - mTankHealth)/2;	// This must range from 0.0 to 0.5
	// Set the health bar to the appropriate level
	mHealthBarBB->setTexcoordRect(0.0 + healthBarAdjuster, 0.0, 0.5 + healthBarAdjuster, 1.0);
	
	//mTankBodyNode->attachObject(mHealthBar);

	/* tell DemoApp that this tank is dead */
	if (mTankHealth <= 0) {
		respawn();
	}
}

void Tank::respawn() {

	float x = (rand() % 10000)-5000;
	float z = (rand() % 10000)-5000;
	float y = mTerrain->getHeightAtWorldPosition(x,0,z);
	mTankBodyNode->setPosition(x,y+100,z);

	mTankHealth = 1; // 100% hp
	mKills = 0;

	float healthBarAdjuster = (1.0 - mTankHealth)/2;	// This must range from 0.0 to 0.5
	// Set the health bar to the appropriate level
	mHealthBarBB->setTexcoordRect(0.0 + healthBarAdjuster, 0.0, 0.5 + healthBarAdjuster, 1.0);
}

void Tank::setTankStateToAI(bool new_state) 
{	 
	if (new_state) { 
		tank_state = TANK_STATE_AI;
		ai_state = AI_STATE_ROAMING;
	} else {
		tank_state = TANK_STATE_USER;
	}
	// reset movement variables
	mMove = 0;
	mBodyRotate = 0;
	mBarrelRotate = 0;
	mTurretRotate = 0;
}

void Tank::createSmokeParticleSystem(){
	// Create unique name
	std::ostringstream oss;
	oss << mSmokeSystemCount;
	std::string entityName = "smoke" + std::to_string(mId) + oss.str();
	// Increment box count
	mSmokeSystemCount++;

	Ogre::ParticleSystem* particleSystem = mSceneMgr->createParticleSystem(entityName,"Examples/JetEngine1");
	Ogre::SceneNode* particleSysNode = mProjectileSpawnNode->createChildSceneNode();
	particleSysNode->translate(20,0,0);
	particleSysNode->attachObject(particleSystem);
	
}
