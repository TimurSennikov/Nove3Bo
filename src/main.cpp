#include "core.hpp"

int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);

	NovelBo game("window", 0, 0);

	ScriptParser script(&game, "script.txt");

	bool running = true;
	while(running){
		pollEvents(running, game, script);

		game.clear();
			game.showBackground();
			script.drawForeground();
			game.dialogue(script.current.c_str());
		game.update();

		SDL_Delay(1000 / 10);
	}

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}
