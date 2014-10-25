#ifndef __DemoApp_h_
#define __DemoApp_h_
 
#include "stdafx.h"
#include "BaseApplication.h"
#include "Tank.h"
#include <vector>

using namespace std;
 
class DemoApp : public BaseApplication
{
private:
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
    OgreBites::Label* mInfoLabel;
 
    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);

	Ogre::Terrain* mTerrain;

	// Camera attached to object?
	bool cameraAttachedToNode;
	
	Ogre::ManualObject* mNormalLine;
	
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

	// Select object
	Ogre::SceneNode* selectedTankNode;
	void selectTank();
	
	// The tanks
	std::vector<Tank> mTanks;    // declares a vector of tanks

	// Current camera zoom
	Ogre::Vector3 camHeightAtPos;
	float currentZoom;
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
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

};
 
#endif // #ifndef __DemoApp_h_