#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_mixer.h>

#include <fstream>
#include <vector>

#include <thread>
#include <chrono>

#include "config.hpp"
#include "save.hpp"

const std::string auto_skip_words[] = {
	"}"
};

struct Background{
	std::string path="NULL";
	SDL_Texture* texture=nullptr;
};

struct MenuButton{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	std::string text;
	void (*callback)(void*, std::string);
};

struct onScreenText{
	std::string text;
	SDL_Rect border;
	SDL_Texture* t;
};

namespace callbacks{
	void BUTTON_CHOSEN_CALLBACK(void* param, std::string name){
		SaveFile* save = (SaveFile*)param;
		save->variables[name] = "CHOSEN";
		if(LOG_LOGS){std::cout << name << " PRESSED!" << std::endl;}
	}
}

namespace screen{
	SDL_DisplayMode getScreenSize(){
		SDL_DisplayMode m;
		SDL_GetCurrentDisplayMode(0, &m);
		return m;
	}
}

class NovelBo{
	SDL_Window* window=nullptr;
	SDL_Renderer* rend=nullptr;
	Mix_Music* nowPlaying=nullptr;

	onScreenText text;

	std::vector<MenuButton> buttons;

	public:
		SaveFile* save;

		Background background;

		NovelBo(const char* name, int x, int y, const char* saveName=".default.nbo"){
			if(x == 0 && y == 0){
				SDL_DisplayMode mode = screen::getScreenSize();

				x = mode.w;
				y = mode.h;
			}

			this->window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x, y, 0);
			this->rend = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);

			this->save = new SaveFile(saveName);

			std::string bg = this->save->getString("bg_now");
			if(bg.length() > 0){this->changeBackground(bg.c_str());}
		}

		~NovelBo(){
			this->destruct();
		}

		void clear(){
			SDL_RenderClear(this->rend);
		}

		void update(){
			this->showButtonsIfAny();

			SDL_RenderPresent(this->rend);
			SDL_UpdateWindowSurface(this->window);
		}

		void drawText(const char* text, int x, int y, int w, int h, int size, const char* fontName="resources/font.ttf", uint8_t br=0, uint8_t bg=0, uint8_t bb=0, uint8_t ba=1, uint8_t fr=255, uint8_t fg=255, uint8_t fb=255){
			if(this->text.text == text){
				SDL_RenderCopy(this->rend, this->text.t, nullptr, &this->text.border);
				return;
			}

			SDL_DestroyTexture(this->text.t); // destroy the texture if it changed.

			TTF_Font* font = TTF_OpenFont(fontName, size);
			if(!font){if(LOG_ERROR)std::cout << "ERROR LOADING FONT! " << SDL_GetError() << std::endl; exit(1);}

			SDL_DisplayMode mode = screen::getScreenSize();

			std::string t(text);

			SDL_Color fgC = {fr, fg, fb};
			SDL_Surface* surface = TTF_RenderUTF8_Blended(font, t.c_str(), fgC);
			SDL_Surface* surfaceBG = SDL_CreateRGBSurface(0, w, h, 16, br, bg, bb, ba);
			SDL_Texture* textureBG = SDL_CreateTextureFromSurface(this->rend, surfaceBG);

			bool saveQuit=false;
			
			if(!surface){if(LOG_ERROR){std::cerr << "ERROR CREATING SURFACE!" << std::endl;} saveQuit=true;}

			SDL_Texture* texture = SDL_CreateTextureFromSurface(this->rend, surface);
			if(!texture){if(LOG_ERROR){std::cerr << "ERROR LOADING TEXTURE!" << std::endl;} saveQuit=true;}

			if(saveQuit){
				SDL_FreeSurface(surface);
				SDL_DestroyTexture(texture);
				SDL_FreeSurface(surfaceBG);
				SDL_DestroyTexture(textureBG);

				TTF_CloseFont(font);

				return;
			}

			SDL_Rect textLoc;

			textLoc.x = x;
			textLoc.y = y;
			textLoc.w = ((size / 4) * (t.length() / 2));
			textLoc.h = h;

			SDL_RenderCopy(this->rend, textureBG, nullptr, &textLoc); // fill the bg
			SDL_RenderCopy(this->rend, texture, nullptr, &textLoc); // print the text

			this->text.text = text;
			this->text.t = texture;

			SDL_FreeSurface(surface);
			SDL_FreeSurface(surfaceBG);
			SDL_DestroyTexture(textureBG);

			TTF_CloseFont(font);
		}

		void drawImage(const char* path, int x, int y, int w, int h){
			SDL_Texture* texture = IMG_LoadTexture(this->rend, path);

			if(w == 0 && h == 0){
				SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
			}

			SDL_Rect r;

			r.x = x;
			r.y = y;
			r.w = w;
			r.h = h;

			SDL_RenderCopy(this->rend, texture, nullptr, &r);
			SDL_DestroyTexture(texture);
		}

		void showButtonsIfAny(){
			if(this->buttons.size() < 1){return;}

			for(int i = 0; i < this->buttons.size(); i++){
				MenuButton* button = &this->buttons[i];

				this->drawText(button->text.c_str(), button->x, button->y, button->w, button->h, 128);
			}
		}

		void changeBackground(const char* filename){
			this->background.path = std::string(filename);
			if(this->background.texture != nullptr){SDL_DestroyTexture(this->background.texture); this->background.texture = nullptr;}

			this->background.texture = IMG_LoadTexture(this->rend, filename);

			this->save->variables["bg_now"] = this->background.path;
		}

		void showAlert(const char* text, int duration=30){
			std::thread([&]() mutable {
				for(int i = 0; i < duration; i++){
					SDL_DisplayMode m = screen::getScreenSize();

					this->clear();
						this->drawText(text, 0, m.h / 2, m.w, 100, 128);
					this->update();

					std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 0.1 second
				}
			});
		}

		void buttonAction(int x, int y){
			for(int i = 0; i < this->buttons.size(); i++){
				MenuButton* button = &this->buttons[i];
				if(y >= button->y && y <= button->y + button->h){
					button->callback(this->save, button->text);

					if(LOG_LOGS){std::cout << "COLLIDE!" << std::endl;}
					this->buttons.clear();
				}
			}
		} // process button click

		void processClick(int x, int y){
			if(this->buttons.size() > 0){
				this->buttonAction(x, y);
				return;
			}
		} // click actions

		void addChoiceButton(std::string text){
			SDL_DisplayMode mode = screen::getScreenSize();

			MenuButton b;
			b.text = text;
			b.x = 0;
			b.y = this->buttons.size() > 0 ? this->buttons[this->buttons.size() - 1].h + this->buttons[this->buttons.size() - 1].y + 10 : mode.h / 2;
			b.w = mode.w;
			b.h = 100;

			b.callback = callbacks::BUTTON_CHOSEN_CALLBACK;

			this->buttons.push_back(b);
		}

		bool hasButtons(){
			return this->buttons.size() > 0;
		}

		void showBackground(){
			if(this->background.texture == nullptr){return;}

			SDL_DisplayMode m = screen::getScreenSize();

			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = m.w;
			r.h = m.h;

			SDL_RenderCopy(this->rend, this->background.texture, nullptr, &r);

		}

		void dialogue(const char* text){
			if(this->buttons.size() > 0){return;} // do not display the dialogue if there is an active choice.

			SDL_DisplayMode mode = screen::getScreenSize();

			this->drawText(text, 0, mode.h / 1.5, mode.w, 50, 128);
		}

		void changeSound(const char* path){
			this->soundStop();
			Mix_FreeMusic(this->nowPlaying);

			this->nowPlaying = Mix_LoadMUS(path);
			if(this->nowPlaying == NULL){
				if(LOG_ERROR){std::cerr << "LOADING MUSIC FILE FAILED!" << std::endl;}
				return;
			}
		}

		void soundStart(){
			Mix_PlayMusic(this->nowPlaying, -1);
		}

		void soundStop(){
			Mix_PauseMusic();
		}

		void destruct(){ // since not everyone is using pointers, I decided to make destruct a public method. (probably a bad idea.)
			SDL_DestroyRenderer(this->rend);
			SDL_DestroyWindow(this->window);

			Mix_FreeMusic(this->nowPlaying);

			this->buttons.clear();

			this->save->save();
			delete this->save; // sounds spooky if u dont know what delete on a pointer does.
		}

		void saveGame(){
			this->save->save();
		}
};

struct ScriptImage{
	std::string fileName;
	int x=0, y=0;
};

class ScriptParser{
	private:
		NovelBo* game;
		std::ifstream* f;
	public:
		std::string current = " ";
		std::vector<ScriptImage> images;
		int linesParsed=0;

		ScriptParser(NovelBo* g, const char* scriptFileName){
			this->game = g;
			f = new std::ifstream(scriptFileName);


			std::string tmp;
			int num;
			
			try{
				num = this->game->save->getInt("dialog_num");
			}
			catch(...){
				num = 0;
			}

			if(LOG_LOGS){std::cout << "GOT SCRIPT FILE LINE " << num << "." << std::endl;}

			for(int i = 0; i < num; i++){std::getline(*this->f, tmp);}
			this->linesParsed = num;
		}

		~ScriptParser(){
			f->close();
			delete f;
		}

		void addImage(std::string image, int x, int y=500){
			ScriptImage img;
			img.fileName = image;

			SDL_DisplayMode m = screen::getScreenSize();
			img.x = m.w / x;
			img.y = y;

			this->images.push_back(img);
		}

		void delImage(std::string fname){
			for(int i = 0; i < this->images.size(); i++){
				if(this->images[i].fileName == fname){
					this->images.erase(this->images.begin() + i);
				}
			}
		}

		void nextLine(){
			std::string line;
			if(!std::getline(*this->f, line, ' ')){
				if(LOG_LOGS){std::cout << "EOS" << std::endl;}
				return;
			}

			if(line.length() < 1){this->linesParsed++; this->nextLine(); return;}

			int command;
			try{
				command = std::stoi(line.c_str());
			}
			catch(...){command = -1;} // we found non-command line of script, skipping

			if(command >= 0){
				switch(command){
					case 0: {
							std::vector<std::string> args;
							std::string tmp;
							for(int i = 0; i < 3; i++){
								std::getline(*this->f, tmp, (i==2?';':' '));
								args.push_back(tmp);
							}

							if(args.size() < 3){if(LOG_ERROR){std::cerr << "FAILED TO RETRIEVE COMMAND ARGUMENTS!" << std::endl;} return;}

							this->addImage(args[0], std::stoi(args[1]), std::stoi(args[2])); // todo
							return;
						}
					case 1: {
						std::string imgName;
						if(!std::getline(*this->f, imgName, ';')){
							if(LOG_ERROR){std::cerr << "ERROR WHEN READING IMG NAME TO DELETE!" << std::endl;}
						}

						this->delImage(imgName);
						return;
					}

					case 3: {
							std::string bgName;
							if(!std::getline(*this->f, bgName, ';')){
								if(LOG_ERROR){std::cerr << "ERROR LOADING BACKGROUND IMAGE NAME!";}
								return;
							}

							this->game->changeBackground(bgName.c_str());
							return;
						}
					case 4:
						{
							std::string params;
							if(!std::getline(*this->f, params, ';')){if(LOG_ERROR){std::cerr << "FAILED TO PARSE CHOICE PARAMS!" << std::endl;}}

							char* param;
							param = strtok((char*)params.c_str(), "/");

							game->addChoiceButton(std::string(param));
							while((param = strtok(NULL, "/")) != NULL){
								game->addChoiceButton(std::string(param));
							}
							break;
						}
					case 5: {
							std::string condition;
							if(!std::getline(*this->f, condition, '{')){
								if(LOG_ERROR){
									std::cerr << "FAILED TO GET IF CONDITION!" << std::endl;
								}
							}

							std::string conditionValue = this->game->save->getString(condition);

							if(LOG_LOGS){std::cout << "VALUE (" << condition << "): " << conditionValue << std::endl;}

							if(conditionValue.length() > 0 && conditionValue != "NULL"){
								if(LOG_LOGS){std::cout << "DETECTED BUTTON PRESS IN IF STATEMENT." << std::endl;}
								return;
							}

							std::string l;
							while(std::getline(*this->f, l, ';')){
								this->linesParsed++;
								if(l.find("}") != std::string::npos){return;}
							}
							return;
						}
					case 6: {
							std::string songName;
							if(!std::getline(*this->f, songName, ';')){if(LOG_ERROR){std::cerr << "FAILED TO FIND MUSIC FILE NAME" << std::endl;} return;}

							this->game->changeSound(songName.c_str());
							this->game->soundStart();
						}
					case 7: {
							std::string skip;
							if(!std::getline(*this->f, skip, '}')){if(LOG_ERROR){if(LOG_ERROR){std::cerr << "FAILED TO USE break OPERATOR!" << std::endl;}}}
						}
				}

				return;
			}

			std::string remainingLine;
			std::getline(*this->f, remainingLine, ';');
			std::string cur = line + " " + remainingLine;

			this->linesParsed++;
			this->game->save->variables["dialog_num"] = std::to_string(this->linesParsed);

			for(int i = 0; i < sizeof(auto_skip_words) / sizeof(auto_skip_words[0]); i++){
				if(cur.find(auto_skip_words[i]) != std::string::npos){return;}
			}
			
			std::cout << cur << std::endl;

			this->current = cur;
		}

		void animFG(std::string name, int xToMove, int step){
			if(LOG_LOGS){std::cout << "MOVING BG (FUNCTION)!" << std::endl;}

			int img = -1;
			for(int i = 0; i < this->images.size(); i++){
				if(this->images[i].fileName == name){
					img = i;
				}
			}
			if(img == -1){return;}
		}

		void drawForeground(){
			for(int i = 0; i < this->images.size(); i++){
				this->game->drawImage(this->images[i].fileName.c_str(), this->images[i].x, this->images[i].y, 0, 0);
			}
		}
};

void pollEvents(bool& running, NovelBo& game, ScriptParser& script, int& fpsCap){
	SDL_Event e;

	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch(e.key.keysym.scancode){
					case SDL_SCANCODE_SPACE:
						if(game.hasButtons()){break;}
						else{script.nextLine();}
						break;
					case SDL_SCANCODE_F5:
						game.saveGame();
						break;
					case SDL_SCANCODE_F1:
						game.save->variables["dialog_num"] = "0";
						game.saveGame();

						exit(0);
					case SDL_SCANCODE_F3:
						fpsCap += 10;
						if(fpsCap >= 300){
							fpsCap = 1;
						}
						break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				std::cout << "MOUSE PRESS AT " << e.button.x << ", " << e.button.y << std::endl;
				game.processClick(e.button.x, e.button.y);
				break;
		}
	}
}
