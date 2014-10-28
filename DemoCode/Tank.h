#pragma once

#include "Utilities.h"

enum TANK_STATE {TANK_STATE_USER = 1, TANK_STATE_AI = 2};
enum AI_STATE {AI_STATE_ROAMING = 1, AI_STATE_ATTACKING = 2};

class Tank
{
public:
	Tank(const int id);
	~Tank(void);

	int getId() { return mId; }
	void setTankStateToAI(bool new_state);

	bool keyRealesed(const OIS::KeyEvent &arg);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool frameRenderingQueued(const Ogre::FrameEvent& evts);

	/* tank nodes */
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;
	Ogre::SceneNode* mCameraHolder;

	Ogre::BillboardSet* mHealthBar;
	Ogre::Billboard* mHealthBarBB;
	Ogre::BillboardSet* mSelectionCircle;
	Ogre::Billboard* mSelectionCircleBB;
	
	Ogre::Vector3 getTankForwardDirection();
	Ogre::Vector3 getTurretForwardDirection();

	Ogre::Terrain* mTerrain;

	Ogre::AxisAlignedBox Tank::getBoundingBox();

	bool tankGotHit();

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

	float mTankHealth;

	int mId;
	TANK_STATE tank_state;
	AI_STATE ai_state;

	// AI METHODS n stuff
	void tankWander();
	bool wander_turning180;
	bool wander_delayAfterTurning;
	int wander_rotateCounter;

};

