#ifndef __DemoApp_h_
#define __DemoApp_h_
 
#include "stdafx.h"
#include "BaseApplication.h"
 
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
	
	Ogre::ManualObject* mNormalLine;
	
	// Scene nodes for the tank
	Ogre::SceneNode* mTankNode;
	Ogre::Entity* mTankEntity;

	// For tank movement and rotation
	float mMove;
	float mBodyRotate;
	float mHeightOffset;

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
};
 
#endif // #ifndef __DemoApp_h_