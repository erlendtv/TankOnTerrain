#ifndef __DemoApp_h_
#define __DemoApp_h_
 
#include "stdafx.h"
#include "BaseApplication.h"
#include "Tank.h"
#include <vector>
#include "PhysicsEngine.h"

enum zoom_level {ZOOM1 = 500, ZOOM2 = 1000, ZOOM3 = 1500, ZOOM4 = 2000, ZOOM5 = 2500};

using namespace std;
 
class DemoApp : public BaseApplication
{
private:
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
    OgreBites::Label* mInfoLabel;
	Ogre::RaySceneQuery* mRaySceneQuery;

	// Physics
	PhysicsEngine* mPhysicsEngine;
	int mBoxCount;
 
    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);

	Ogre::Terrain* mTerrain;

	// Camera attached to object?
	bool cameraAttachedToNode;
	
	// TESTING
	Ogre::ManualObject* mNormalLine;
	// TESTING
	
	// Scene nodes for the different tank parts
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;
	Ogre::SceneNode* mProjectileSpawnNode;
	Ogre::BillboardSet* mHealthBar;
	Ogre::Billboard* mHealthBarBB;
	Ogre::BillboardSet* mSelectionCircle;
	Ogre::Billboard* mSelectionCircleBB;

	// "Godmode" node to hold camera
	Ogre::SceneNode* mGodCameraHolder;

	// Constants
	float tankBodyMoveFactor;
	float tankBodyRotFactor;
	float tankTurretRotFactor;
	float tankBarrelRotFactor;
	// For tank movement and rotation
	float mMove;
	float mBodyRotate;
	float mTurretRotate;
	float mBarrelRotate;
	float mBarrelPitch;
	float mHeightOffset;
	// button states
	bool insertBtnIsDown;

	// Select object
	Tank* selectedTank;
	void selectTank();
	int tankCounter;
	bool isTankSelected;
	int mExplosionCount;

	Tank* selectedTankForGUI;
	
	// The tanks
	std::vector<Tank> mTanks;    // declares a vector of tanks
	bool addNewTank(const Ogre::Vector3 spawnPoint);
	std::vector<Ogre::SceneNode*> projectiles;

	// Current camera zoom
	Ogre::Vector3 camHeightAtPos;
	zoom_level currentZoom;

	bool isColliding(Ogre::Vector3 one, Ogre::Vector3 two);
	void spawnExposionParticleSystem(Ogre::Vector3 position);
	void spawnExplosionParticleSystem(Ogre::Vector3 position);
	void getTankInfoForGUI();



public:
    DemoApp(void);
    virtual ~DemoApp(void);
 
protected:
    virtual void createScene(void);
    virtual void createFrameListener(void);
    virtual void destroyScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	void updateDetailsPanel(Tank* tank);
	void checkProjectileCollision();
	void createWorldObstacles();
	void checkWorldCollisions();
	float getProjectileHeightAtXZ(Ogre::Vector3 position);


};


#endif // #ifndef __DemoApp_h_