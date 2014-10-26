#pragma once
class Tank
{
public:
	Tank(void);
	~Tank(void);

	bool keyRealesed(const OIS::KeyEvent &arg);
	bool keyPressed(const OIS::KeyEvent &arg);

	/* tank nodes */
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;
	Ogre::SceneNode* mCameraHolder;
	
	Ogre::Vector3 getTankForwardDirection(Tank tank);
	Ogre::Vector3 getTurretForwardDirection(Tank tank);

private:

	/* tank movement variables */
	float mMove;
	float mBodyRotate;
	float mTurretRotate;
	float mBarrelRotate;
	float mBarrelPitch;
	float mHeightOffset;
	float mTankBodyMoveFactor;
	float mTankBodyRotFactor;
	float mTankTurretRotFactor;
	float mTankBarrelPitchFactor;
	

};

