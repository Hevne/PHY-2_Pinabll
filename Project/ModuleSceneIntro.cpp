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

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	spritesheet = App->textures->Load("pinball/sprite_sheet.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

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
	//CIRCLE SENSOR
	circle_sensor_1= App->physics->CreateCircleSensor(217, 198, 26, false);
	circle_sensor_2= App->physics->CreateCircleSensor(183, 138, 26, false);
	circle_sensor_3= App->physics->CreateCircleSensor(257, 121, 26, false);

	plunger = App->physics->CreateRectangle(344, 468, 26, 121,false);
	boxes.add(plunger);
	plunger_x = 344;
	plunger_y = 468;
	

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
	LOG("MOUSE X: %i", App->input->GetMouseX());
	LOG("MOUSE Y: %i", App->input->GetMouseY());

	DrawLayers();
	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		//COMMENTED: CREATE BALL
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 7,true));
		circles.getLast()->data->listener = this;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if(plunger_y<530)
		plunger_y += 1;
		plunger_x += 0.7;

		plunger->body->SetTransform({ PIXEL_TO_METERS(plunger_x), PIXEL_TO_METERS(plunger_y) }, NULL);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		plunger_y = 468;
		plunger_x = 344;
		p2List_item<PhysBody*>* c = circles.getFirst();
		c->data->body->ApplyForce({ 0,-20 }, { c->data->body->GetLocalCenter() }, true);
		plunger->body->SetTransform({ PIXEL_TO_METERS(plunger_x), PIXEL_TO_METERS(plunger_y) }, NULL);
		//TODO eudald: Change box and circle blitting for every single one. SENSOR to respawn ball, life counter, polish code
		
		//c->data->body->ApplyLinearImpulse({ 0,100 }, { c->data->body->GetLocalCenter() }, true);
	
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		//COMMENTED: CREATE BOX
		//boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 26, 121));
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		//Fill shapes with image
		//if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
		App->renderer->Blit(circle, x-4, y-3, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c= ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	//Audio effect
	//App->audio->PlayFx(bonus_fx);

	
	/*if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
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
	layer1.add({ "map_front",{-2,356,362,515 }, 1, 5 });

	
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

	//Mid Layer
	for (int i = 0; i < layer2.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer2[i].pos_x, layer2[i].pos_y, &layer2[i].rect, 0.f);
	}


	//Front Layer
	for (int i = 0; i < layer1.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer1[i].pos_x, layer1[i].pos_y, &layer1[i].rect, 0.f);
	}

	//First layer
	for (int i = 0; i < layer0.count(); i++)
	{
		App->renderer->Blit(spritesheet, layer0[i].pos_x, layer0[i].pos_y, &layer0[i].rect, 0.f);
	}
	
	
	text_score = current_score.GetString();
	App->fonts->BlitText(426, 313, 1, text_score);
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
	App->physics->CreateChain(1, 0, bumper_right, 12);

	int bumper_left[12] = {
	105, 470,
	106, 477,
	153, 496,
	161, 494,
	159, 487,
	115, 465
	};
	App->physics->CreateChain(1, 0, bumper_left, 12);

	int bumper_top[12] = {
	264, 282,
	266, 290,
	273, 290,
	311, 265,
	309, 257,
	300, 255
	};
	App->physics->CreateChain(1, 0, bumper_top, 12);

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
	App->physics->CreateChain(1, 0, triangle_shape, 16);

	int triangle_shape_left[14] = {
	78, 368,
	83, 362,
	91, 363,
	118, 431,
	115, 440,
	107, 441,
	76, 421
	};
	App->physics->CreateChain(2, 0, triangle_shape_left, 14);

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
	App->physics->CreateChain(2, 0, shape_at_2left, 22);

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


	App->physics->CreateCircle(217, 198, 25, false);
	App->physics->CreateCircle(183, 138, 25, false);
	App->physics->CreateCircle(257, 121, 25, false);
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


