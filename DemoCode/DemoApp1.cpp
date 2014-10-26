#include "DemoApp.h"
 
//-------------------------------------------------------------------------------------
DemoApp::DemoApp(void)
{
}
//-------------------------------------------------------------------------------------
DemoApp::~DemoApp(void)
{
}

bool DemoApp::setup(void)
{
	BaseApplication::setup();

	// Set the camera's position and where it is looking
	mCamera->setPosition(0, 100, 300);
	mCamera->lookAt(0, 50, 0);

	mMove = 0;
	mBodyRotate = 0;

	return true;
};

//-------------------------------------------------------------------------------------
void DemoApp::createScene(void)
{
	// Set ambient lighting
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
 
    // Create a light
    Ogre::Light *light = mSceneMgr->createLight("Light1");
    light->setType(Ogre::Light::LT_POINT);
    light->setPosition(Ogre::Vector3(250, 150, 250));
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue::White);

	// Use a plane to represent the ground
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	entGround->setMaterialName("Examples/Rockwall");
	entGround->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

	// Create tank body entity
	Ogre::Entity* tankBody = mSceneMgr->createEntity("chbody1", "chbody.mesh");
	tankBody->setCastShadows(true);
	tankBody->setMaterialName("ch_tank_material");

	// Create scene node for the tank
	mTankNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mTankNode->attachObject(tankBody);

	// Move it above the ground
	mTankNode->translate(0, 18, 0);

}

bool DemoApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
	if(mShutDown)
		return false;
 
	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	// Move and rotate the tank
	mTankNode->translate(mMove, 0, 0, Ogre::Node::TransformSpace::TS_LOCAL);
	mTankNode->yaw(Ogre::Degree(mBodyRotate));

	mTrayMgr->frameRenderingQueued(evt);

	return true;
}

// OIS::KeyListener
bool DemoApp::keyPressed( const OIS::KeyEvent &arg )
{
	BaseApplication::keyPressed(arg);

    switch (arg.key)
	{
		case OIS::KC_W:
			mMove -= 0.1;
			break;

		case OIS::KC_S:
			mMove += 0.1;
			break;

		case OIS::KC_A:
			mBodyRotate += 0.1;
			break;

		case OIS::KC_D:
			mBodyRotate -= 0.1;
			break;

		case OIS::KC_ESCAPE: 
			mShutDown = true;
			break;

		default:
			break;
	}

	return true;
}

bool DemoApp::keyReleased( const OIS::KeyEvent &arg )
{
	BaseApplication::keyReleased(arg);

	switch (arg.key)
	{
		case OIS::KC_W:
			mMove += 0.1;
			break;

		case OIS::KC_S:
			mMove -= 0.1;
			break;

		case OIS::KC_A:
			mBodyRotate -= 0.1;
			break;

		case OIS::KC_D:
			mBodyRotate += 0.1;
			break;

		default:
			break;
	}

	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        DemoApp app;
 
        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif