#include "core.hpp"
#include "time.hpp"

int main(){
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);

	NovelBo game("window", 0, 0);

	ScriptParser script(&game, "script.txt");

	bool running = true;

	SDL_DisplayMode mode = screen::getScreenSize();

	frameTime t;
	std::string fps;

	while(running){
		pollEvents(running, game, script);
		
		gameTime::regStart(t);

		game.clear();
			game.showBackground();
			script.drawForeground();
			game.dialogue(script.current.c_str());

			if(LOG_FPS){game.drawText(fps.c_str(), 0, 0, 300, mode.h / 10, 128);}

			game.update();

		SDL_Delay(1000 / TARGET_FPS);

		gameTime::regEnd(t);
		gameTime::regDiff(t);
		if(t.frame % 10 == 0){fps = std::to_string(gameTime::fps(t)) + " FPS";}

		gameTime::tick(t);
	}	

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}
