#include <vector>
#include "time.hpp"

namespace gameMath{
	uint64_t avgFPS(std::vector<uint64_t> values){
		uint64_t sum = 0;

		for(std::vector<uint64_t>::iterator i = values.begin(); i != values.end(); i++){
			sum += *i;
		}

		return sum / values.size();
	}
}
