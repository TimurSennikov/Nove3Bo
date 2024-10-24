#include <iostream>
#include <map>
#include <fstream>
#include <string.h>

#include "config.hpp"
#include "log.hpp"

class SaveFile{
	private:
		std::ifstream i;
		std::string f;
	public:
		std::map<std::string, std::string> variables;

		SaveFile(std::string fName){
			this->i.open(fName);
			this->f = fName;

			std::string line;
			while(getline(this->i, line)){
				const char* key = strtok((char*)line.c_str(), "=");
				const char* value = strtok(NULL, "=");

				this->variables[std::string(key)] = std::string(value);
			}
		}

		~SaveFile(){
			this->i.close();
		}

		void save(){
			if(this->variables.size() < 1 && LOG_WARNING){std::cerr << "FSAVER: WARNING: NOTHING TO SAVE IN A FILE, ABORTING!" << std::endl; return;}

			std::ofstream o(this->f);
			for(std::map<std::string, std::string>::iterator i = this->variables.begin(); i != this->variables.end(); i++){
				if((i->second == PRESSED_VALUE || i->second == CHOSEN_VALUE) && IGNORE_BUTTONS){continue;} // ignoring

				o << i->first << "=" << i->second << std::endl;
			}

			if(LOG_LOGS){std::cout << "FSAVER: NOTIFY: GAME DATA SAVED AS " << this->f << std::endl;}
			o.close();
		}
		
		int getInt(std::string key){
			try{
				if(this->variables[key].length() < 1){return 0;}
				std::string value = this->variables[key];
				return std::stoi(value);
			}
			catch(...){
				return 0;
			}

			return 0;
		}

		std::string getString(std::string key){
			try{
				if(this->variables[key].length() > 0){
					return this->variables[key];
				}

				return "NULL";
			}
			catch(...){
				return "NULL";
			}
		}
};
