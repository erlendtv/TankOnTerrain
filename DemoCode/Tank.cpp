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