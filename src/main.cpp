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
	std::string gameTick;

	int fpsCap = TARGET_FPS;

	while(running){
		pollEvents(running, game, script, fpsCap);
		
		gameTime::regStart(t);
		gameTick = std::to_string(gameTime::now.frame);

		game.clear();
			game.showBackground();
			script.drawForeground();
			game.dialogue(script.current.c_str());

			if(LOG_FPS){game.drawText(fps.c_str(), 0, 0, 300, mode.h / 10, 128);}
			if(LOG_FRAME_NUM){game.drawText(gameTick.c_str(), ((mode.w) - ((mode.w / 10) * gameTick.length())), 0, 300, mode.h / 10, 128);}

			game.update();

		SDL_Delay(1000 / fpsCap);

		gameTime::regEnd(t);
		gameTime::regDiff(t);
		if(t.frame % 10 == 0){fps = std::to_string(gameTime::fps(t)) + " FPS";}

		gameTime::tick(t);
		gameTime::tick();
	}	

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}
