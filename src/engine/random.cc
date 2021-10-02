#include "engine.h"

using namespace ts;

double Engine::getRandom() {
	this->randomSeed = ((this->randomSeed * 1103515245U) + 12345U) & 0x7fffffff; // ripped from glibc
	return (double)this->randomSeed / (double)pow(2, 31);
}

void Engine::setRandomSeed(int seed) {
	this->randomSeed = seed;
}

int Engine::getRandomSeed() {
	return this->randomSeed;
}
