#pragma once

#include "Utilities.h"
#include "PhysicsEngine.h"

enum TANK_STATE {TANK_STATE_USER = 1, TANK_STATE_AI = 2};
enum AI_STATE {AI_STATE_ROAMING = 1, AI_STATE_ATTACKING = 2};

class Tank
{
public:
	Tank(const int id);
	~Tank(void);

	int getId() { return mId; }
	void setTankStateToAI(bool new_state) { if (new_state) { tank_state = TANK_STATE_AI;} else {tank_state = TANK_STATE_USER;}}

	bool keyRealesed(const OIS::KeyEvent &arg);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool frameRenderingQueued(const Ogre::FrameEvent& evts);
	void shootProjectile();
	float calculateProjectileRange();


	/* tank nodes */
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;
	Ogre::SceneNode* mCameraHolder;
	Ogre::SceneNode* mProjectileSpawnNode;
	
	//cast movable to entity
	
	
	Ogre::Vector3 getTankForwardDirection();
	Ogre::Vector3 getTurretForwardDirection();
	Ogre::Vector3 getBarrelYDirection();

	// NEEDED FROM DEMOAPP

	PhysicsEngine* mPhysicsEngine;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Terrain* mTerrain;
	int mBoxCount;
	std::vector<Ogre::SceneNode*> projectiles;

	Ogre::AxisAlignedBox Tank::getBoundingBox();



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

	// for shooting
	float mProjectileInitVelocity;



	int mId;
	TANK_STATE tank_state;
	AI_STATE ai_state;

	// AI METHODS n stuff
	void tankWander();
	bool wander_turning180;
	bool wander_delayAfterTurning;
	int wander_rotateCounter;

};

