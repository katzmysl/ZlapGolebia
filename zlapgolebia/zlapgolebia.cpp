#include <allegro5\allegro.h>
#include <allegro5\allegro_native_dialog.h>
#include <allegro5\allegro_image.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <stdlib.h>
#include <time.h>

const int RES_X = 1024;
const int RES_Y = 768;
const float FPS = 100;
const int KEY_NONE = -1;
const int MAX_ENEMIES = 100;
const int SPEED_PLAYER = 3;
const int SPEED_ENEMY = 1;

const char FILE_BACKGROUND[] = "grafika\\background.png";
const char FILE_PIDGEON[] = "grafika\\enemy.png";
const char FILE_PLAYER_1[] = "grafika\\player1.png";
const char FILE_PLAYER_2[] = "grafika\\player2.png";
const char FILE_PLAYER_3[] = "grafika\\player3.png";
const char FILE_MENU[] = "grafika\\menu.png";

enum Modes {
	MENU, GAME, PLAYER_SELECT, HIGHSCORE, EXIT, STOP, INSTRUCTION
};

typedef struct s_bitmaps {
	ALLEGRO_BITMAP  *background;
	ALLEGRO_BITMAP  *player1;
	ALLEGRO_BITMAP  *player2;
	ALLEGRO_BITMAP  *player3;
	ALLEGRO_BITMAP  *enemy;
} Bitmaps;

typedef struct s_actor {
	int w;
	int h;
	float x;
	float y;
	ALLEGRO_BITMAP *img;
} Actor;

typedef struct s_game {
	ALLEGRO_BITMAP  *background;
	Actor player;
	Actor enemy;
	Actor enemies[MAX_ENEMIES];
	int enemyCount;
	int currentKeyPressed;
} Game;

typedef struct s_menu {
	int curOption;
	int selectedPlayer;
	ALLEGRO_FONT *font;	
} Menu;

Bitmaps bitmaps;
Game game;
Menu menu;

bool probability(int promile) {
	int r = rand() % 1000;
	if (r < promile) {
		return true;
	}
	else {
		return false;
	}
}

int randFromTo(int from, int to) {
	return rand() % (to - from) + from;
}

Actor createActor(int w, int h, float x, float y, ALLEGRO_BITMAP *img) {
	ALLEGRO_COLOR mask_color = al_map_rgb(255, 0, 255);
	Actor newActor;
	newActor.w = w;
	newActor.h = h;
	newActor.x = x;
	newActor.y = y;
	newActor.img = img;
	return newActor;
}

Actor copyActor(Actor* actor) {
	Actor newActor;
	newActor.w = actor->w;
	newActor.h = actor->h;
	newActor.x = actor->x;
	newActor.y = actor->y;
	newActor.img = actor->img;
	return newActor;
}

void drawActor(Actor* actor, int flags) {
	al_draw_bitmap(actor->img, actor->x, actor->y, flags);
}

void drawActor(Actor* actor) {
	drawActor(actor, 0);
}

void drawMenuOption(int y, char* text, bool selected) {
	ALLEGRO_COLOR color;
	if (selected) {
		color = al_map_rgb(0, 255, 152);
	}
	else {
		color = al_map_rgb(255, 0, 0);
	}
	al_draw_text(menu.font, color, RES_X / 2, y, ALLEGRO_ALIGN_CENTER, text);
}

void doTimerMenu() {

	al_clear_to_color(al_map_rgb_f(0, 0, 0));
	drawMenuOption(RES_Y / 2 - 150, "NEW GAME", menu.curOption == 0);
	drawMenuOption(RES_Y / 2 - 100, "HIGHSCORES", menu.curOption == 1);
	drawMenuOption(RES_Y / 2 - 50, "INSTRUCTION", menu.curOption == 2);
	drawMenuOption(RES_Y / 2 , "EXIT", menu.curOption == 3);
	al_flip_display();
}

void generateEnemy() {
	if (game.enemyCount < MAX_ENEMIES && probability(5)) {
		Actor newEnemy = copyActor(&game.enemy);
		newEnemy.y = randFromTo(50, RES_Y - 50);
		game.enemies[game.enemyCount] = newEnemy;
		game.enemyCount++;
	}
}

void doTimerGame() {

	switch (game.currentKeyPressed)	{
		case ALLEGRO_KEY_UP:
			if (game.player.y > 0) {
				game.player.y-=SPEED_PLAYER;
			}
			break;
		case ALLEGRO_KEY_DOWN:
			if (game.player.y + game.player.h < RES_Y) {
				game.player.y+= SPEED_PLAYER;
			}
			break;
	}
	int enemiesToDelete = 0;
	for (int i = 0; i < game.enemyCount; i++){
		game.enemies[i].x -= SPEED_ENEMY;
		if (game.enemies[i].x < -50) {
			enemiesToDelete++;
		}
	}
	if (enemiesToDelete > 0) {
		for (int i = enemiesToDelete; i < game.enemyCount; i++) {
			game.enemies[i-enemiesToDelete] = game.enemies[i];
		}
		game.enemyCount -= enemiesToDelete;
	}

	generateEnemy();

	al_draw_bitmap(game.background, 0, 0, 0);
	for (int i = 0; i < game.enemyCount; i++) {
		drawActor(&game.enemies[i], ALLEGRO_FLIP_HORIZONTAL);
	}	
	drawActor(&game.player);
	al_flip_display();
	if (game.enemy.x == -50) {
		game.enemy.x = RES_X;
	}
}

void doTimerPlayerSelect() {

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(menu.font, al_map_rgb(255, 0, 0), RES_X / 2, 100, ALLEGRO_ALIGN_CENTER, "Select your player");

	al_draw_bitmap(bitmaps.player1, RES_X / 2 - 400 , 250, 0);
	al_draw_bitmap(bitmaps.player2, RES_X / 2 - 100 , 250, 0);
	al_draw_bitmap(bitmaps.player3, RES_X / 2 + 200, 250, 0);

	al_draw_rectangle(100 + menu.selectedPlayer*300, 210, 300 + menu.selectedPlayer * 300, 450, al_map_rgb(255, 0, 0), 3);

	al_flip_display();
}

void doTimerHighscore() {}

void doTimerExit() {}

void doTimer(Modes currentMode) {
	switch (currentMode) {
		case MENU:
			doTimerMenu();
			break;
		case GAME:
			doTimerGame();
			break;
		case PLAYER_SELECT:
			doTimerPlayerSelect();
			break;
		case HIGHSCORE:
			doTimerHighscore();
			break;
		case EXIT:
			doTimerExit();
			break;
		default:
			break;
	}	
}

Modes onMenuEnterPress() {
	Modes newMode = MENU;
	switch (menu.curOption) {
	case 0:
		newMode = PLAYER_SELECT;
		break;
	case 1:
		newMode = HIGHSCORE;
		break;
	case 2:
		newMode = INSTRUCTION;
		break;
	case 3:
		newMode = STOP;
		break;
	default:
		break;
	}
	return newMode;
}

Modes doKeyboardMenu(ALLEGRO_EVENT ev) {
	Modes newMode = MENU;
	switch (ev.keyboard.keycode)
	{
	case ALLEGRO_KEY_UP:
		if (menu.curOption == 0) {
			menu.curOption = 3;
		}
		else {
			menu.curOption--;
		}
		break;
	case ALLEGRO_KEY_DOWN:
		if (menu.curOption == 3) {
			menu.curOption = 0;
		}else{
			menu.curOption++;
		}
		break;
	case ALLEGRO_KEY_ENTER:
		newMode = onMenuEnterPress();
		break;
	case ALLEGRO_KEY_ESCAPE:
		newMode = STOP;
		break;
	}
	return newMode;
}

Modes doKeyboardGame(ALLEGRO_EVENT ev) {
	Modes newMode = GAME;
	if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
		newMode = MENU;
	}
	else {
		game.currentKeyPressed = ev.keyboard.keycode;
	}
	return newMode;
}

Modes doKeyboardPlayerSelect(ALLEGRO_EVENT ev) {
	Modes newMode = PLAYER_SELECT;
	switch (ev.keyboard.keycode)
	{
	case ALLEGRO_KEY_LEFT:
		if (menu.selectedPlayer == 0) {
			menu.selectedPlayer = 2;
		}
		else {
			menu.selectedPlayer--;
		}
		break;
	case ALLEGRO_KEY_RIGHT:
		if (menu.selectedPlayer == 2) {
			menu.selectedPlayer = 0;
		}
		else {
			menu.selectedPlayer++;
		}
		break;
	case ALLEGRO_KEY_ENTER:
		if (menu.selectedPlayer == 0) {
			game.player.img = bitmaps.player1;
		} else if (menu.selectedPlayer == 1) {
			game.player.img = bitmaps.player2;
		} else {
			game.player.img = bitmaps.player3;
		}
		newMode = GAME;
		break;
	case ALLEGRO_KEY_ESCAPE:
		newMode = MENU;
		break;
	}
	return newMode;
}

Modes doKeyboardHighscore(ALLEGRO_EVENT ev) {
	return HIGHSCORE;
}

Modes doKeyboardExit(ALLEGRO_EVENT ev) {
	return EXIT;
}

Modes doKeyboard(Modes currentMode, ALLEGRO_EVENT ev) {
	switch (currentMode) {
	case MENU:
		currentMode = doKeyboardMenu(ev);
		break;
	case GAME:
		currentMode = doKeyboardGame(ev);
		break;
	case PLAYER_SELECT:
		currentMode = doKeyboardPlayerSelect(ev);
		break;
	case HIGHSCORE:
		currentMode = doKeyboardHighscore(ev);
		break;
	case EXIT:
		currentMode = doKeyboardExit(ev);
		break;
	default:
		break;
	}
	return currentMode;
}

void onModeChange(Modes oldMode, Modes newMode) {
	switch (newMode) {
	case MENU:
		menu.curOption = 0;
		break;
	case GAME:
		game.currentKeyPressed = KEY_NONE;
		game.enemyCount = 0;
		break;
	case PLAYER_SELECT:
		menu.selectedPlayer = 0;
		break;
	case HIGHSCORE:
		break;
	case EXIT:
		break;
	default:
		break;
	}
}

void loadBitmaps() {
	ALLEGRO_COLOR mask_color = al_map_rgb(255, 0, 255);
	bitmaps.background = al_load_bitmap(FILE_BACKGROUND);
	bitmaps.enemy = al_load_bitmap(FILE_PIDGEON);
	bitmaps.player1 = al_load_bitmap(FILE_PLAYER_1);
	bitmaps.player2 = al_load_bitmap(FILE_PLAYER_2);
	bitmaps.player3 = al_load_bitmap(FILE_PLAYER_3);
	al_convert_mask_to_alpha(bitmaps.enemy, mask_color);
	al_convert_mask_to_alpha(bitmaps.player1, mask_color);
	al_convert_mask_to_alpha(bitmaps.player2, mask_color);
	al_convert_mask_to_alpha(bitmaps.player3, mask_color);
}

int main(void) {

	srand(time(NULL));
	
	ALLEGRO_DISPLAY *okno = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_DISPLAY *display = NULL;
	
	al_init();
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
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
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	menu.font = al_load_ttf_font("grafika//arial.ttf", 25, 0);
	loadBitmaps();
	game.player = createActor(150, 162, 0, RES_Y / 2 - 162 / 2, bitmaps.player1);
	game.enemy = createActor(70, 45, RES_X + 50, RES_Y / 2 - 10, bitmaps.enemy);
	game.background = bitmaps.background;

	Modes currentMode = MENU;
	Modes oldMode = MENU;
	onModeChange(oldMode,currentMode);
	
	al_start_timer(timer);

	while (1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			doTimer(currentMode);
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			currentMode = doKeyboard(currentMode, ev);
			if (currentMode == Modes::STOP) {
				break;
			}
			else {
				if (currentMode != oldMode) {
					oldMode = currentMode;
					onModeChange(oldMode, currentMode);
				}
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			game.currentKeyPressed = KEY_NONE;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
	}

	al_destroy_display(okno);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_bitmap(bitmaps.background);
	al_destroy_bitmap(bitmaps.enemy);
	al_destroy_bitmap(bitmaps.player1);
	al_destroy_bitmap(bitmaps.player2);
	al_destroy_bitmap(bitmaps.player3);
	al_uninstall_system();

	return 0;
}