#pragma once

class Projectile
{
public:
	Projectile(void);
	~Projectile(void);

	Ogre::SceneNode* node;
	bool exist;
	int tankId;
	float Lived;

};

