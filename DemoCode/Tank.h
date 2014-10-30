#pragma once

#include "Utilities.h"
#include "PhysicsEngine.h"
#include "Projectile.h"

enum TANK_STATE {TANK_STATE_USER = 1, TANK_STATE_AI = 2};
enum AI_STATE {AI_STATE_ROAMING = 1, AI_STATE_ATTACKING = 2};

class Tank
{
public:
	Tank(const int id);
	~Tank(void);

	int getId() { return mId; }
	float getHp() { return mTankHealth;}
	int getKills() {return mKills;}
	int getDeaths() {return mDeaths;}
 	void setTankStateToAI(bool new_state);
	void setAiState(AI_STATE new_state) {ai_state = new_state;}
	Tank* getCurrentlyAttacking() { return mCurrentlyAttacking;}
	void addKill(){mKills++;}
	bool keyRealesed(const OIS::KeyEvent &arg);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool frameRenderingQueued(const Ogre::FrameEvent& evts);
	void shootProjectile();
	void createSmokeParticleSystem();
	float calculateProjectileRange();
	void createDeathParticleSystem();


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
	std::vector<Projectile*>* projectiles;

	Ogre::AxisAlignedBox Tank::getBoundingBox();

	bool tankGotHit(float lived);

	std::vector<Tank>* mTanks;

	TANK_STATE tank_state;
	AI_STATE ai_state;
	void respawn();

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
	int mKills;
	int mDeaths;

	Tank* mCurrentlyAttacking;
	int mSmokeSystemCount;

	int mId;

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

