#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	left_bumper_rect = { 582,134,62,20 };
	right_bumper_rect = { 505,135,63,19 };
	top_bumper_rect = { 432,181,58,19 };

	play = { 198,207,49, 14 };
	replay = { 282,228,49, 9 };
	exit_game = { 282,208,70, 9 };

	arrow = { 258,4,6, 12 };

	play_background = { 461,273, 142,32 };
	play_highlight  = { 461,316, 142,32 };

	small_background = { 140,317, 136,26 };
	small_highlight  = { 313,272, 136,26 };




	App->renderer->camera.x = App->renderer->camera.y = 0;
	circle = App->textures->Load("pinball/wheel.png");
	box = App->textures->Load("pinball/crate.png");
	spritesheet = App->textures->Load("pinball/sprite_sheet.png");
	plunger_fx = App->audio->LoadFx("audio/fx/rebound.wav");
	hit_fx = App->audio->LoadFx("audio/fx/hit.wav");
	lost_fx = App->audio->LoadFx("audio/fx/lost.wav");
	sensor_rect_fx = App->audio->LoadFx("audio/fx/sensor_rect.wav");
	combo_fx = App->audio->LoadFx("audio/fx/bonus.wav");
	triangle_fx = App->audio->LoadFx("audio/fx/triangle.wav");
	respawn_fx = App->audio->LoadFx("audio/fx/respawn.wav");
	flip_fx = App->audio->LoadFx("audio/fx/flip.wav");
	App->audio->PlayMusic("audio/music/loop.ogg");

	App->fonts->Load("fonts/score_font14h.png", "0123456789", 1, 11, 14, 10);
	LoadSprites();
	LoadChains();

	//RECTANGLE SENSOR
	sensor_top_1 = App->physics->CreateRectangleSensor(197, 47, 14, 12);
	sensor_top_2 = App->physics->CreateRectangleSensor(224, 47, 14, 12);
	sensor_top_3 = App->physics->CreateRectangleSensor(252, 47, 14, 12);
	sensor_mid_1 = App->physics->CreateRectangleSensor(290, 233, 14, 12);
	sensor_mid_2 = App->physics->CreateRectangleSensor(274, 217, 14, 12);
	sensor_bot_1 = App->physics->CreateRectangleSensor(290, 333, 14, 12);
	sensor_bot_2 = App->physics->CreateRectangleSensor(274, 317, 14, 12);
	sensor_lost = App->physics->CreateRectangleSensor(190, 530, 200, 10);
	//CIRCLE SENSOR
	circle_sensor_1= App->physics->CreateCircleSensor(217, 198, 26, false);
	circle_sensor_2= App->physics->CreateCircleSensor(183, 138, 26, false);
	circle_sensor_3= App->physics->CreateCircleSensor(257, 121, 26, false);

	//PLUNGER Creation
	plunger = App->physics->CreateRectangle(344, 468, 26, 121,false);
	boxes.add(plunger);
	plunger->body->SetActive(false);
	plunger_x = 344;
	plunger_y = 468;

	// bumper left 105, 470
	int posx1 = 118;
	int posy1 = 475;
	int dim1 = 20;
	b2BodyDef b1;
	b1.type = b2_staticBody;
	b1.position.Set(PIXEL_TO_METERS(posx1), PIXEL_TO_METERS(posy1));

	b2Body* circle1 = App->physics->world->CreateBody(&b1);

	b2CircleShape shape1;
	shape1.m_radius = PIXEL_TO_METERS(dim1) * 0.5f;

	b2FixtureDef fix1;
	fix1.shape = &shape1;
	circle1->CreateFixture(&fix1);


	int posx2 = 230;
	int posy2 = 475;
	int dim2 = 20;
	b2BodyDef b2;
	b2.type = b2_staticBody;
	b2.position.Set(PIXEL_TO_METERS(posx2), PIXEL_TO_METERS(posy2));

	b2Body* circle2 = App->physics->world->CreateBody(&b2);

	b2CircleShape shape2;
	shape2.m_radius = PIXEL_TO_METERS(dim2) * 0.5f;

	b2FixtureDef fix2;
	fix2.shape = &shape2;
	circle2->CreateFixture(&fix2);

	int posx3 = 302;
	int posy3 = 260;
	int dim3 = 20;
	b2BodyDef b3;
	b3.type = b2_staticBody;
	b3.position.Set(PIXEL_TO_METERS(posx3), PIXEL_TO_METERS(posy3));

	b2Body* circle3 = App->physics->world->CreateBody(&b3);

	b2CircleShape shape3;
	shape3.m_radius = PIXEL_TO_METERS(dim3) * 0.5f;

	b2FixtureDef fix3;
	fix3.shape = &shape3;
	circle3->CreateFixture(&fix3);

	//Left Bumper
	box_bumper_left = App->physics->CreateRectangle(posx1 + dim1, posy1, 60, 18, true);
	left_bumper_joint = App->physics->CreateRevolutionJoint(circle1, box_bumper_left->body, circle1->GetWorldCenter(), -0.05f, 0.15f, 75.f, 50.f);


	//Right Bumper
	box_bumper_right = App->physics->CreateRectangle(posx2 - dim2, posy2, 60, 18, true);
	right_bumper_joint = App->physics->CreateRevolutionJoint(circle2, box_bumper_right->body, circle2->GetWorldCenter(),-0.15f, 0.05f, 75.f, -50.f);

	//Top Bumper
	box_bumper_top = App->physics->CreateRectangle(posx3 - dim3, posy3, 58, 10, true);
	top_bumper_joint = App->physics->CreateRevolutionJoint(circle3, box_bumper_top->body, circle3->GetWorldCenter(),-0.15f, 0.05f, 75.f, -50.f);


	//UI Buttons

	play_button = App->physics->CreateRectangle(400 + play_background.w * 0.5f, 460 + play_background.h * 0.5f, play_background.w, play_background.h, false);
	play_button->body->SetAwake(false);


	replay_button = App->physics->CreateRectangle(410 + small_background.w * 0.5f, 450 +small_background.h * 0.5f, +small_background.w, +small_background.h, false);
	replay_button->body->SetAwake(false);
	replay_button->isActive = false;

	exit_button = App->physics->CreateRectangle(410 + small_background.w * 0.5f, 477 + small_background.h * 0.5f, +small_background.w, +small_background.h, false);
	exit_button->body->SetAwake(false);
	exit_button->isActive = false;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if (play_button->body->IsAwake()) {
		LOG("IM AWAKE");
	}
	LOG("MOUSE X: %i", App->input->GetMouseX());
	LOG("MOUSE Y: %i", App->input->GetMouseY());

	DrawLayers();
	//BUMPERS CONTROLL
	if (App->input->GetKey(SDL_SCANCODE_Z)== KEY_DOWN) {
		left_bumper_joint->SetMotorSpeed(-50.f);
		App->audio->PlayFx(flip_fx);
	}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		right_bumper_joint->SetMotorSpeed(50.f);
		top_bumper_joint->SetMotorSpeed(50.f);
		App->audio->PlayFx(flip_fx);

	}
	if (left_bumper_joint->GetJointAngle() <= left_bumper_joint->GetLowerLimit()) {
		left_bumper_joint->SetMotorSpeed(50.f);
	}
	if (right_bumper_joint->GetJointAngle() >= right_bumper_joint->GetUpperLimit()) {
		right_bumper_joint->SetMotorSpeed(-50.f);
	}
	if (top_bumper_joint->GetJointAngle() >= top_bumper_joint->GetUpperLimit()) {
		top_bumper_joint->SetMotorSpeed(-50.f);
	}

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN || respawn)
	{
		on_game = true;
		App->audio->PlayFx(respawn_fx);
		//COMMENTED: CREATE BALL
		CreateBall();
		respawn = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && plunger->body->IsActive())
	{
		if(plunger_y<530)
		plunger_y += 1;
		plunger_x += 0.7;
		plunger->body->SetTransform({ PIXEL_TO_METERS(plunger_x), PIXEL_TO_METERS(plunger_y) }, NULL);

	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP && plunger->body->IsActive())
	{
		App->audio->PlayFx(plunger_fx);
		plunger_y = 468;
		plunger_x = 344;
		ball->body->ApplyForce({ 0,-80 }, { ball->body->GetLocalCenter() }, true);
		plunger->body->SetActive(false);
		plunger->body->SetTransform({ PIXEL_TO_METERS(plunger_x), PIXEL_TO_METERS(plunger_y) }, NULL);
		ball->body->GetFixtureList()->SetRestitution(0.3f);
	}

	// Prepare for raycast ------------------------------------------------------

	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// Bumpers draw functions ------------------------------------------------------

		box_bumper_left->body->GetPosition();
		App->renderer->Blit(spritesheet, 118-8, 460, &left_bumper_rect, 1.0f,box_bumper_left->GetRotation(),0,0);

		box_bumper_right->body->GetPosition();
		App->renderer->Blit(spritesheet,175, 460, &right_bumper_rect, 1.0f,box_bumper_right->GetRotation(),60,0);

		box_bumper_top->body->GetPosition();
		App->renderer->Blit(spritesheet, 250, 250, &top_bumper_rect, 1.0f,box_bumper_top->GetRotation(),60,0);


	p2List_item<PhysBody*>* b = boxes.getFirst();
	while(b != NULL)
	{
		int x, y;
		b->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, b->data->GetRotation());

		if(ray_on)
		{
			int hit = b->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		b = b->next;
	}


	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	//Draw Wheel animations
	if (circle_active) {
		SDL_Rect rect = { 286,92,38,31 };
		App->renderer->Blit(spritesheet, (int)circle_active->GetPosition().x -  18, (int)circle_active->GetPosition().y- 20, &rect);
	}

	//DRAW UI
	// PLAY BUTTON
	if (play_button->isActive && !replay_button->isActive && !exit_button->isActive) {
		App->renderer->Blit(spritesheet, 400, 460, &play_background);
		if (play_button->Contains(App->input->GetMouseX(), App->input->GetMouseY())) {
			App->renderer->Blit(spritesheet, 400, 460, &play_highlight);
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
				timer = 0;
				startTimer = true;
				on_game = true;
				App->audio->PlayFx(respawn_fx);
				//COMMENTED: CREATE BALL
				CreateBall();
				respawn = false;
				show_controls = true;
			}
		}
		App->renderer->Blit(spritesheet, 445, 468, &play);
		App->renderer->Blit(spritesheet, 508, 470, &arrow);
		if (timer < 5 && startTimer) {
			timer++;
		}
		else if(startTimer){
			play_button->isActive = false;
			exit_button->isActive = true;
		}
	}

	// REPLAY BUTTON
	if (!play_button->isActive && replay_button->isActive && !exit_button->isActive) {
		App->renderer->Blit(spritesheet, 410, 450, &small_background);
		if (replay_button->Contains(App->input->GetMouseX(), App->input->GetMouseY())) {
			App->renderer->Blit(spritesheet, 410, 450, &small_highlight);
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
				replay_button->isActive = false;
				exit_button->isActive = true;
				on_game = true;
				App->audio->PlayFx(respawn_fx);
				//COMMENTED: CREATE BALL
				CreateBall();
				App->audio->PlayFx(combo_fx);
				show_controls = true;
			}
		}
		App->renderer->Blit(spritesheet, 452, 457, &replay);
		App->renderer->Blit(spritesheet, 530, 456, &arrow);
	}	
	
	// EXIT BUTTON
	if (!play_button->isActive && !replay_button->isActive && exit_button->isActive) {
		App->renderer->Blit(spritesheet, 410, 477, &small_background);
		if (exit_button->Contains(App->input->GetMouseX(), App->input->GetMouseY())) {
			App->renderer->Blit(spritesheet, 410, 477, &small_highlight);
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
				exit_button->isActive = false;
				replay_button->isActive = true;
				p2List_item<PhysBody*>* c = circles.getLast();
				App->physics->world->DestroyBody(c->data->body);		
				current_score = { "0,0,0" };
				App->audio->PlayFx(combo_fx);
				show_controls = false;
			}
		}
		App->renderer->Blit(spritesheet, 442, 485, &exit_game);
		App->renderer->Blit(spritesheet, 530, 483, &arrow);
	}

	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{


	if (bodyB == triangle_left || bodyB == triangle_right) {

		App->audio->PlayFx(triangle_fx);
	}

	if (bodyB == sensor_top_1 || bodyB == sensor_top_2 || bodyB == sensor_top_3 || bodyB == sensor_mid_1 || bodyB == sensor_mid_2 || bodyB == sensor_bot_1 || bodyB == sensor_bot_2) {

		//TODO EUDALD: DYNAMIC ANIMATONS AND ACTIVATE/DEACTIVATE BLITS IN FUNCTION OF SENSORS. SEE ORIGINAL GAME FOR REFERENCE. SFX.
		if (App->scene_intro->inc_score == true)
		{
			App->scene_intro->NewScore(500);
			App->scene_intro->inc_score = false;
		}
		App->audio->PlayFx(sensor_rect_fx);
	}

	if (bodyB == circle_sensor_1 || bodyB == circle_sensor_2 || bodyB == circle_sensor_3) {

		if (App->scene_intro->inc_score == true)
		{
			App->scene_intro->NewScore(500);
			App->scene_intro->inc_score = false;
		}
		App->audio->PlayFx(hit_fx);
	}

	if (bodyB == sensor_lost)
	{
		App->audio->PlayFx(lost_fx);
		lifes--;
		if (lifes > 0)
		{
			respawn = true;
		}
		else
		{
			if (score > high_score)
			{
				medium_score = high_score;
				mid_score = highest_score;
				high_score = score;
				highest_score = current_score;
			}
			else if (score > medium_score)
			{
				low_score = medium_score;
				lowest_score = mid_score;
				medium_score = score;
				mid_score = current_score;

			}
			else if (score > low_score)
			{
				low_score = score;
				lowest_score = current_score;
			}
			current_score = { "0,0,0" };
			score = 0;
			lifes = 1;
		}
	}

	if (bodyB == circle_01 || bodyB == circle_02 || bodyB == circle_03) {
		circle_active = bodyB;

	}
	else {
		circle_active = nullptr;
	}
}

void ModuleSceneIntro::LoadSprites()
{
	//First Layer
	layer0.add({ "tube",{1093,259,313,394 }, 1, 5 });

	//Fences left
	layer0.add({ "fence_front_left1",{231,67,18,17 }, 53, 305 });
	layer0.add({ "fence_front_left2",{231,67,18,17 }, 71, 295 });
	layer0.add({ "fence_front_left3",{231,67,18,17 }, 89, 286 });
	//Plates left
	//layer0.add({ "plate_front_left1",{232,103,17,17 }, 55, 306 });
	layer0.add({ "plate_front_left1",{232,103,17,17 }, 73, 296 });
	layer0.add({ "plate_front_left2",{232,103,17,17 }, 91, 287 });
	//Plates right bottom
	layer0.add({ "plate_front_right1",{206,101,13,20 }, 281, 295 });
	layer0.add({ "plate_front_right2",{206,101,13,20 }, 297, 312 });
	//Plates right up
	layer0.add({ "plate_front__top_right1",{206,101,13,20 }, 281, 196 });
	layer0.add({ "plate_front__top_right2",{206,101,13,20 }, 296, 212 });
	//Plates right sided
	layer0.add({ "plate_sided__top_right1",{207,64,5,22 }, 301, 97 });
	layer0.add({ "plate_sided__top_right2",{207,64,5,22 }, 302, 116 });
	layer0.add({ "plate_sided__top_right3",{207,64,5,22 }, 303, 136 });
	//Car draw
	layer0.add({ "plate_sided__top_right3",{383,507,171,91 }, 385, 159 });

	//Front Layer
	layer1.add({ "light_top_1", { 168,97,29,28 }, 183, 33 });
	layer1.add({ "light_top_2", { 168,97,29,28 }, 210, 33 });
	layer1.add({ "light_top_3", { 168,97,29,28 }, 237, 33 });
	layer1.add({ "light_mid_1", { 168,97,29,28 }, 276, 219 });
	layer1.add({ "light_mid_2", { 168,97,29,28 }, 261, 203 });
	layer1.add({ "light_bot_1", { 168,97,29,28 }, 276, 320 });
	layer1.add({ "light_top_2", { 168,97,29,28 }, 261, 304 });
	layer1.add({ "map_front",{-2,356,362,515 }, 1, 5 });

	//Controls layer
	layer_instructions.add({ "ZM",{202,0,41,21},362,450 });
	layer_instructions.add({ "Arrow" , { 157,0,21,21 }, 482, 450 });
	layer_instructions.add({ "flippers", { 0,873,78,10 }, 402, 453 });
	layer_instructions.add({ "plunger", { 0,887,74,10 }, 502, 453 });

	//Back Layer
	layer3.add({ "map_back",{-1,884,578,521 }, 0, -15 });

}

void ModuleSceneIntro::DrawLayers()
{
	//Back Layer
	for (int i = 0; i < layer3.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer3[i].pos_x, layer3[i].pos_y, &layer3[i].rect, 0.f);
	}

	p2List_item<PhysBody*>* c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		//Fill shapes with image
		App->renderer->Blit(circle, x-4, y - 3, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	//Mid Layer
	for (int i = 0; i < layer2.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer2[i].pos_x, layer2[i].pos_y, &layer2[i].rect, 0.f);
	}


	//Front Layer
	for (int i = 0; i < layer1.count(); i++)
	{
		if (!on_game)
		{
			App->renderer->Blit(spritesheet, layer1[i].pos_x, layer1[i].pos_y, &layer1[i].rect, 0.f);
		}
		else if (on_game)
		{
			if (layer1[i].name == "map_front")
			{
				App->renderer->Blit(spritesheet, layer1[i].pos_x, layer1[i].pos_y, &layer1[i].rect, 0.f);
			}
		}
	}
	

	//First layer
	for (int i = 0; i < layer0.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer0[i].pos_x, layer0[i].pos_y, &layer0[i].rect, 0.f);
	}

	//controls
	if (show_controls)
	{
		for (int i = 0; i < layer_instructions.count(); i++)
		{
			App->renderer->Blit(spritesheet, layer_instructions[i].pos_x, layer_instructions[i].pos_y, &layer_instructions[i].rect, 0.f);
		}
	}

	text_highest_score = highest_score.GetString();
	text_mid_score = mid_score.GetString();
	text_lowest_score = lowest_score.GetString();
	text_current_score = current_score.GetString();

	App->fonts->BlitText(426, 313, 1, text_highest_score);
	App->fonts->BlitText(426, 345, 1, text_mid_score);
	App->fonts->BlitText(426, 377, 1, text_lowest_score);
	App->fonts->BlitText(426, 409, 1, text_current_score);


}

void ModuleSceneIntro::LoadChains()
{
	int bumper_right[12] = {
	184, 487,
	182, 492,
	188, 497,
	235, 480,
	240, 472,
	232, 464
	};
	//right_bumper = App->physics->CreateChain(1, 0, bumper_right, 12);


	int bumper_left[12] = {
	105, 470,
	106, 477,
	153, 496,
	161, 494,
	159, 487,
	115, 465
	};
	//left_bumper = App->physics->CreateChain(1, 0, bumper_left, 12,true);

	int bumper_top[12] = {
	264, 282,
	266, 290,
	273, 290,
	311, 265,
	309, 257,
	300, 255
	};
	//top_bumper = App->physics->CreateChain(1, 0, bumper_top, 12);
	//top_bumper->body->GetFixtureList()->SetRestitution(1.0f);


	int triangle_shape[16] = {
	267, 421,
	237, 440,
	228, 440,
	224, 432,
	252, 364,
	261, 363,
	267, 369,
	267, 396
	};
	triangle_right  = App->physics->CreateChain(1, 0, triangle_shape, 16);
	triangle_right->body->GetFixtureList()->SetRestitution(1.5f);

	int triangle_shape_left[14] = {
	78, 368,
	83, 362,
	91, 363,
	118, 431,
	115, 440,
	107, 441,
	76, 421
	};
	triangle_left = App->physics->CreateChain(2, 0, triangle_shape_left, 14);
	triangle_left->body->GetFixtureList()->SetRestitution(1.5f);

	int shape_at_right[16] = {
	235, 461,
	244, 475,
	290, 449,
	302, 437,
	300, 352,
	291, 346,
	284, 352,
	287, 431
	};
	App->physics->CreateChain(2, 0, shape_at_right, 16);

	int shape_at_2right[24] = {
	238, 508,
	237, 499,
	305, 461,
	314, 452,
	321, 441,
	316, 329,
	284, 296,
	282, 284,
	309, 268,
	316, 268,
	321, 274,
	321, 510
	};
	App->physics->CreateChain(2, 0, shape_at_2right, 24);

	int shape_at_2left[22] = {
	44, 352,
	50, 347,
	58, 349,
	60, 359,
	57, 430,
	62, 436,
	108, 461,
	101, 475,
	52, 447,
	43, 438,
	41, 427
	};
	PhysBody* chain2 = App->physics->CreateChain(2, 0, shape_at_2left, 22);
	b2Filter filter;
	filter.groupIndex = -8;
	chain2->body->GetFixtureList()->SetFilterData(filter);
	int group = chain2->body->GetFixtureList()->GetFilterData().groupIndex;
	LOG("Group chain index: %i", group);

	int shape_at_top_right[38] = {
	311, 268,
	311, 243,
	308, 230,
	297, 215,
	283, 199,
	284, 188,
	301, 170,
	307, 158,
	307, 102,
	296, 88,
	285, 81,
	263, 79,
	262, 40,
	264, 33,
	281, 35,
	292, 40,
	303, 50,
	311, 65,
	321, 273
	};
	App->physics->CreateChain(2, 0, shape_at_top_right, 36);

	int shape_at_top[12] = {
	234, 68,
	239, 72,
	244, 68,
	244, 46,
	239, 42,
	234, 45
	};
	App->physics->CreateChain(2, -1, shape_at_top, 12);
	App->physics->CreateChain(-28, -1, shape_at_top, 12);

	int shape_at_top_left[20] = {
	182, 80,
	186, 74,
	186, 33,
	181, 30,
	172, 30,
	161, 36,
	154, 45,
	154, 58,
	157, 69,
	165, 76
	};
	App->physics->CreateChain(2, -1, shape_at_top_left, 20);

	int borders[82] = {
	329, 68,
	324, 48,
	308, 30,
	281, 17,
	167, 17,
	148, 26,
	135, 40,
	134, 59,
	144, 74,
	144, 85,
	135, 98,
	133, 178,
	137, 190,
	146, 203,
	163, 216,
	173, 228,
	184, 247,
	187, 272,
	181, 296,
	176, 304,
	166, 308,
	154, 304,
	142, 291,
	122, 255,
	103, 260,
	108, 273,
	102, 282,
	47, 309,
	50, 316,
	26, 349,
	22, 435,
	25, 446,
	35, 459,
	105, 498,
	105, 520,
	0, 520,
	0, -1,
	405, -1,
	405, 520,
	360, 520,
	333, 114
	};
	App->physics->CreateChain(0, 0, borders, 82);


	circle_01 = App->physics->CreateCircle(217, 198, 25, false);
	circle_01->body->GetFixtureList()->SetRestitution(1.f);
	circle_02 = App->physics->CreateCircle(183, 138, 25, false);
	circle_02->body->GetFixtureList()->SetRestitution(1.f);
	circle_03 = App->physics->CreateCircle(257, 121, 25, false);
	circle_03->body->GetFixtureList()->SetRestitution(1.f);
	//Plunger




}

void ModuleSceneIntro::NewScore(int inc)
{
	if (score + inc < 900000000)
	{
		score += inc;
	}

	current_score = p2SString("%d", score);
}

void ModuleSceneIntro::CreateBall()
{
	plunger->body->SetActive(true);
	ball = App->physics->CreateCircle(340, 390, 7, true);
	ball->body->GetFixtureList()->SetRestitution(0.2f);


	circles.add(ball);
	circles.getLast()->data->listener = this;
}
