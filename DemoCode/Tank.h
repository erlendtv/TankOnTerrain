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
	void setTankStateToAI(bool new_state);

	bool keyRealesed(const OIS::KeyEvent &arg);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool frameRenderingQueued(const Ogre::FrameEvent& evts);
	void shootProjectile();
	void createSmokeParticleSystem();
	float calculateProjectileRange();


	/* tank nodes */
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;
	Ogre::SceneNode* mCameraHolder;
	Ogre::SceneNode* mProjectileSpawnNode;

	Ogre::BillboardSet* mHealthBar;
	Ogre::Billboard* mHealthBarBB;
	Ogre::BillboardSet* mSelectionCircle;
	Ogre::Billboard* mSelectionCircleBB;
	
	Ogre::Vector3 getTankForwardDirection();
	Ogre::Vector3 getTurretForwardDirection();
	Ogre::Vector3 getBarrelYDirection();

	// NEEDED FROM DEMOAPP

	PhysicsEngine* mPhysicsEngine;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Terrain* mTerrain;
	int mBoxCount;
	std::vector<Ogre::SceneNode*>* projectiles;

	Ogre::AxisAlignedBox Tank::getBoundingBox();

	bool tankGotHit();

	std::vector<Tank>* mTanks;

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

	float mTankHealth;

	Tank* mCurrentlyAttacking;
	int mSmokeSystemCount;

	int mId;
	TANK_STATE tank_state;
	AI_STATE ai_state;

	// AI METHODS n stuff
	void tankWander();
	bool wander_turning180;
	bool wander_delayAfterTurning;
	int wander_rotateCounter;

	bool attack_rotating_body;
	bool attack_move;
	int attack_move_counter;

	void tankAttacking(Tank* tank_to_attack);

	int ready_to_shoot;
};

