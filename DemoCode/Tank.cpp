#include "stdafx.h"
#include "Tank.h"


Tank::Tank(void)
{
}


Tank::~Tank(void)
{
}

bool Tank::keyRealesed(const OIS::KeyEvent &arg)
{

}

bool Tank::keyPressed(const OIS::KeyEvent &arg)
{

}

Ogre::Vector3 Tank::getTurretForwardDirection(){
		Ogre::Quaternion orientation = mTankTurretNode->getOrientation();
		Ogre::Vector3 localY = mTankBodyNode->getPosition() + orientation.yAxis();
		Ogre::Vector3 local.y = mTerrain->getHeightAtWorldPosition(localY);
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
		Ogre::Vector3 local.y = mTerrain->getHeightAtWorldPosition(localY);
		localY -= mTankBodyNode->getPosition();
		Ogre::Vector3 nLocalZ = mTankBodyNode->getPosition() - orientation.zAxis();
		nLocalZ.y = mTerrain->getHeightAtWorldPosition(nLocalZ);
		nLocalZ -= mTankBodyNode->getPosition();
		Ogre::Vector3 direction = -localY.crossProduct(nLocalZ);
		direction.normalise();
		return direction;
}