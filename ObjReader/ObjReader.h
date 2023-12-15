#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
class ObjReader {
public:
	static std::vector<float> read(char* path);
};