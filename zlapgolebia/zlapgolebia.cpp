#include <allegro5\allegro.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>

const int RES_X = 1024;
const int RES_Y = 768;
const float FPS = 100;

const char FILE_BACKGROUND[] = "grafika\\background.png";
const char FILE_PIDGEON[] = "grafika\\enemy.png";
const char FILE_PLAYER_1[] = "grafika\\player1.png";
const char FILE_PLAYER_2[] = "grafika\\player2.png";
const char FILE_PLAYER_3[] = "grafika\\player3.png";
const char FILE_MENU[] = "grafika\\menu.png";
enum Modes {
	MENU, GAME, PLAYER_SELECT, HIGHSCORE, EXIT
};

typedef struct s_actor {
	int w;
	int h;
	float x;
	float y;
	ALLEGRO_BITMAP *img;
} Actor;




Actor createActor(int w, int h, float x, float y, const char* imgName) {
	ALLEGRO_COLOR mask_color = al_map_rgb(255, 0, 255);
	Actor newActor;
	newActor.w = w;
	newActor.h = h;
	newActor.x = x;
	newActor.y = y;
	newActor.img = al_load_bitmap(imgName);
	al_convert_mask_to_alpha(newActor.img, mask_color);
	return newActor;
}

void drawActor(Actor* actor, int flags) {
	al_draw_bitmap(actor->img, actor->x, actor->y, flags);
}

void drawActor(Actor* actor) {
	drawActor(actor, 0);
}

int main(void) {

	ALLEGRO_DISPLAY *okno = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_DISPLAY *display = NULL;
	
		
	al_init();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	ALLEGRO_KEYBOARD_STATE klawiatura;
	if (!al_init()) {
		al_show_native_message_box(NULL, NULL, NULL, "ALLEGRO INIT FAIL", NULL, NULL);
		return -1;
	}
	
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		al_show_native_message_box(NULL, NULL, NULL, "TIMER INIT FAIL", NULL, NULL);
		return -1;
	}

	okno = al_create_display(RES_X, RES_Y);

	if (!okno) {
		al_show_native_message_box(NULL, NULL, NULL, "ALLEGRO DISPLAY FAIL", NULL, NULL);
		al_destroy_timer(timer);
		return -1;
	}
	event_queue = al_create_event_queue();
	if (!event_queue) {
		al_show_native_message_box(NULL, NULL, NULL, "ALLEGRO EVENT QUEUE FAIL", NULL, NULL);
		al_destroy_display(okno);
		al_destroy_timer(timer);
		return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(okno));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	
	Actor player1 = createActor(150, 162, 0, RES_Y / 2 - 162 / 2, FILE_PLAYER_1);
	Actor player2 = createActor(150, 162, 0, RES_Y / 2 - 162 / 2, FILE_PLAYER_2);
	Actor player3 = createActor(150, 162, 0, RES_Y / 2 - 162 / 2, FILE_PLAYER_3);
	Actor enemy = createActor(70, 45, RES_X / 2, RES_Y / 2 - 10, FILE_PIDGEON);

	ALLEGRO_COLOR mask_color = al_map_rgb(255, 0, 255);
	
	ALLEGRO_BITMAP  *background = al_load_bitmap(FILE_BACKGROUND);
	//ALLEGRO_BITMAP *img_samolot = al_load_bitmap(FILE_PLAYER_2);
	//al_convert_mask_to_alpha(img_samolot, mask_color);
	//ALLEGRO_BITMAP *img_golab = al_load_bitmap(FILE_PIDGEON);
	//al_convert_mask_to_alpha(img_golab, mask_color);
	
	
	//float x1 = 10, y1 = 10;
	//float x2 = RES_X, y2 = RES_Y / 2 - 10;

	Modes currentMode = Modes::MENU;
	ALLEGRO_BITMAP  *menu = al_load_bitmap(FILE_MENU);
	
	al_start_timer(timer);

	while (1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {

			al_get_keyboard_state(&klawiatura);

			if (al_key_down(&klawiatura, ALLEGRO_KEY_ESCAPE)) {
				break;
			}
			if (al_key_down(&klawiatura, ALLEGRO_KEY_DOWN)) {
				if (player2.y + player2.h < RES_Y) {
					player2.y++;
				}
			}
			if (al_key_down(&klawiatura, ALLEGRO_KEY_UP)) {
				if (player2.y > 0 )  {
					player2.y--;
				}
			}
		
			enemy.x-=2;

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		//	al_draw_bitmap(menu, 0, 0, 0);
			al_draw_bitmap(background, 0, 0, 0);
			drawActor(&player2);
			drawActor(&enemy,ALLEGRO_FLIP_HORIZONTAL);

			//al_draw_bitmap(enemy.img, enemy.x, enemy.y, ALLEGRO_FLIP_HORIZONTAL);
			al_flip_display();
			if (enemy.x == -50) {
				enemy.x = RES_X;
			}
		}// else if (ev.type == ALLEGRO_EVENT_ DISPLAY_CLOSE) {
			//break;		
		//}
	}


	al_destroy_display(okno);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_bitmap(player2.img);
	al_destroy_bitmap(enemy.img);
	al_destroy_bitmap(menu);
	al_destroy_bitmap(background);
	al_uninstall_system();

	return 0;
}