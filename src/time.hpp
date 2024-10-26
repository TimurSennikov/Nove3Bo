#include <chrono>

struct frameTime{
	uint64_t start=0;
	uint64_t end=0;
	uint64_t diff=0;

	uint64_t frame=0;
};

struct localTime{
	uint64_t frame=0;
};

namespace gameTime{
	localTime now;

	uint64_t millis(){
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void regStart(frameTime& data){
		data.start = gameTime::millis();
	}

	void regEnd(frameTime& data){
		data.end = gameTime::millis();
	}

	void regDiff(frameTime& data){
		data.diff = data.end - data.start;
		if(data.diff <= 0){data.diff = 1;}
	}
	
	int fps(frameTime& data){
		return (int)(1000 / data.diff);
	}
	
	void tick(frameTime& data){
		data.frame++;
	}

	void tick(){
		gameTime::now.frame++;
	}
}
