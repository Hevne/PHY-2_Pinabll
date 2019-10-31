#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "p2SString.h"

class PhysBody;

struct Sprite {

	p2SString name;
	SDL_Rect rect;
	int pos_x;
	int pos_y;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void LoadSprites();
	void DrawLayers();
	void LoadChains();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	SDL_Rect ball;


	PhysBody* sensor;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* spritesheet;

	p2List<Sprite> layer0;
	p2List<Sprite> layer1;
	p2List<Sprite> layer2;
	p2List<Sprite> layer3;

	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
};
