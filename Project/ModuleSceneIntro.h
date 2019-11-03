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
	void ActivateSensors(PhysBody* BodyB);
	void DrawActiveSensors();
	void DrawActivePlates();
	void CheckActiveSensors();

public:
	//UI Buttons
	SDL_Rect play; 
	SDL_Rect play_highlight; 
	SDL_Rect play_background; 
	SDL_Rect replay;
	SDL_Rect exit_game;
	SDL_Rect small_highlight;
	SDL_Rect tube_highlight;
	SDL_Rect triangle_ray_left;
	SDL_Rect triangle_ray_right;
	SDL_Rect small_background;
	SDL_Rect arrow;
	SDL_Rect car;

	PhysBody* play_button = nullptr;
	PhysBody* replay_button = nullptr;
	PhysBody* exit_button = nullptr;

	int timer = 0;
	bool startTimer = false;

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
	PhysBody* sensor_top_side_1 = nullptr;
	PhysBody* sensor_top_side_2 = nullptr;
	PhysBody* sensor_top_side_3 = nullptr;
	PhysBody* sensor_car = nullptr;
	PhysBody* sensor_tube = nullptr;
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
	PhysBody* circle_active = nullptr;

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
	p2List<Sprite> layer_instructions;

	uint hit_fx;
	uint plunger_fx;
	uint lost_fx;
	uint sensor_rect_fx;
	uint respawn_fx;
	uint combo_fx;
	uint triangle_fx;
	uint flip_fx;
	uint tube_fx;

	p2Point<int> ray;
	bool ray_on;

	const char* text_current_score;
	const char* text_highest_score;
	const char* text_lowest_score;
	const char* text_mid_score;
	bool inc_score = true;
	int score = 0;
	int high_score = 0;
	int medium_score = 0;
	int low_score = 0;
	p2SString current_score = { "000" };
	p2SString highest_score = { "000" };
	p2SString lowest_score = { "000" };
	p2SString mid_score = { "000" };

	int plunger_x;
	int plunger_y;

	
	uint lifes = 5;
	bool on_game = false;
	bool show_controls = false;
	bool respawn = false;

	//Sensors
	bool sensor_top_1_active = false;
	bool sensor_top_2_active = false;
	bool sensor_top_3_active = false;
	bool sensor_mid_1_active = false;
	bool sensor_mid_2_active = false;
	bool sensor_bot_1_active = false;
	bool sensor_bot_2_active = false;

	//Plates
	bool plate_mid_1_active = true;
	bool plate_mid_2_active = true;
	bool plate_bot_1_active = true;
	bool plate_bot_2_active = true;
	bool plate_top_1_active = true;
	bool plate_top_2_active = true;
	bool plate_top_3_active = true;

	bool tube_highlight_active = false;
	uint highlight_timer = 0;
	bool tube_sound = true;

	bool triangle_ray_left_active = false;
	bool triangle_ray_right_active = false;
	uint triangleray_timer_left = 0;
	uint triangleray_timer_right = 0;

	bool spritechange = false;
	bool spritechange_active = false;
	uint spritechange_timer = 0;
};
