#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "p2SString.h"

class PhysBody;
class b2RevoluteJoint;

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
	void NewScore(int inc);
	void CreateBall();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	SDL_Rect ball_rect;
	SDL_Rect left_bumper_rect;
	SDL_Rect right_bumper_rect;
	SDL_Rect top_bumper_rect;


	PhysBody* sensor_top_1 = nullptr;
	PhysBody* sensor_top_2 = nullptr;
	PhysBody* sensor_top_3 = nullptr;
	PhysBody* sensor_mid_1 = nullptr;
	PhysBody* sensor_mid_2 = nullptr;
	PhysBody* sensor_bot_1 = nullptr;
	PhysBody* sensor_bot_2 = nullptr;
	PhysBody* sensor_lost = nullptr;
	PhysBody* circle_sensor_1 = nullptr;
	PhysBody* circle_sensor_2 = nullptr;
	PhysBody* circle_sensor_3 = nullptr;
	PhysBody* plunger = nullptr;
	PhysBody* left_bumper = nullptr;
	PhysBody* right_bumper = nullptr;
	PhysBody* top_bumper = nullptr;
	PhysBody* ball = nullptr;
	PhysBody* triangle_left = nullptr;
	PhysBody* triangle_right = nullptr;

	PhysBody* circle_01 = nullptr;
	PhysBody* circle_02 = nullptr;
	PhysBody* circle_03 = nullptr;

	PhysBody* box_bumper_left = nullptr;
	PhysBody* box_bumper_right = nullptr;
	PhysBody* box_bumper_top = nullptr;
	PhysBody* box_plunger = nullptr;

	b2RevoluteJoint* left_bumper_joint = nullptr;
	b2RevoluteJoint* right_bumper_joint = nullptr;
	b2RevoluteJoint* top_bumper_joint = nullptr;
	
	bool sensed;

	SDL_Texture* circle = nullptr;
	SDL_Texture* box = nullptr;
	SDL_Texture* rick = nullptr;
	SDL_Texture* spritesheet = nullptr;

	p2List<Sprite> layer0;
	p2List<Sprite> layer1;
	p2List<Sprite> layer2;
	p2List<Sprite> layer3;

	uint hit_fx;
	uint plunger_fx;
	uint lost_fx;
	uint sensor_rect_fx;
	uint respawn_fx;
	uint combo_fx;
	uint triangle_fx;

	p2Point<int> ray;
	bool ray_on;

	const char* text_score;
	bool inc_score = true;
	int score = 0;
	p2SString current_score = { "000" };

	int plunger_x;
	int plunger_y;
	

	bool respawn = false;
};
