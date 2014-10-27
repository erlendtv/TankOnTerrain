#ifndef __DemoApp_h_
#define __DemoApp_h_
 
#include "stdafx.h"
#include "BaseApplication.h"
#include "Tank.h"
#include <vector>
#include "PhysicsEngine.h"

enum zoom_level {ZOOM1 = 500, ZOOM2 = 750, ZOOM3 = 1000, ZOOM4 = 1250, ZOOM5 = 1500};

using namespace std;
 
class DemoApp : public BaseApplication
{
private:
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
    OgreBites::Label* mInfoLabel;

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
	
	// The tanks
	std::vector<Tank> mTanks;    // declares a vector of tanks
	bool addNewTank(const Ogre::Vector3 spawnPoint);

	// Current camera zoom
	Ogre::Vector3 camHeightAtPos;
	zoom_level currentZoom;
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
	void shootBox(const btVector3& position, const btQuaternion& orientation, const btVector3& linearVelocity);
};


#endif // #ifndef __DemoApp_h_