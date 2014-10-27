#include "stdafx.h"
#include "Tank.h"


Tank::Tank(void)
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
		default:
			break;
	}

	return true;
}


bool Tank::frameRenderingQueued(const Ogre::FrameEvent& evt, const Ogre::Terrain* mTerrain)
{
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