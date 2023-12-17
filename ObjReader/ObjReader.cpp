#include "ObjReader.h"

std::vector<float> ObjReader::read(char* path) {
	std::ifstream objFile;
	objFile.open(path);
	if (objFile.fail()) {
		std::cout << "error while opening the obj file\n";
		exit(1);
	}
	else {
		std::cout << "open file successfully\n";
	}
	std::string dataType;
	float left = 300.0, right = -300.0, bottom = 300.0, top = -300.0, near = -300.0, far = 300.0;
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec2> texture;
	std::vector<glm::vec3> normal;
	texture.push_back(glm::vec2(0.0f, 0.0f));
	vertex.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	normal.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	std::vector<float> allData; // vertex + normal + texture coord
	int cnt = 0;
	double _min = 200.0;
	while (objFile >> dataType) {
		cnt++;
		if (dataType.compare("v") == 0) {
			glm::vec3 tmp;
			objFile >> tmp.x >> tmp.y >> tmp.z;
			if (abs(tmp.x) < _min) _min = tmp.x;
			if (abs(tmp.y) < _min) _min = tmp.y;
			if (abs(tmp.z) < _min) _min = tmp.z;
			// if(cnt > 100 && cnt < 300) std::cout << tmp.x << " " << tmp.y << " " << tmp.z << " " << _min << "\n";
			vertex.push_back(tmp);
			if (tmp.x < left) left = tmp.x;
			if (tmp.x > right) right = tmp.x;
			if (tmp.y > top) top = tmp.y;
			if (tmp.y < bottom) bottom = tmp.y;
			if (tmp.z > near) near = tmp.z;
			if (tmp.z < far) far = tmp.z;
		}
		else if (dataType.compare("vt") == 0) {
			glm::vec2 tmp;
			objFile >> tmp.x >> tmp.y;
			texture.push_back(tmp);
		}
		else if (dataType.compare("vn") == 0) {
			glm::vec3 tmp;
			objFile >> tmp.x >> tmp.y >> tmp.z;
			normal.push_back(tmp);
		}
		else if (dataType.compare("f") == 0) {
			int vertexIdx[3], textureIdx[3], normalIdx[3];
			std::string tmp;
			char ch1, ch2;
			int temp;
			std::stringstream ss;
			for (int i = 0; i < 3; i++) {
				objFile >> tmp;
				// std::cout << tmp << " ";
				ss.clear(); ss << tmp;
				ss >> vertexIdx[i] >> ch1 >> textureIdx[i]; // >> ch2 >> normalIdx[i];
				// std::cout << vertexIdx[i] << " " << textureIdx[i] << std::endl;
			}
			// std::cout << vertexIdx[0] << " " << vertexIdx[1] << " " << vertexIdx[2] << '\n';
			// std::cout << textureIdx[0] << ' ' << textureIdx[1] << " " << textureIdx[2] << '\n';
			glm::vec3 normal = glm::cross(vertex[vertexIdx[0]] - vertex[vertexIdx[1]], vertex[vertexIdx[0]] - vertex[vertexIdx[2]]);
			// std::cout << "vertex number: " << vertex.size() << " / ";
			// std::cout << "normal number: " << normal.size() << " / ";
			// std::cout << "texture coord number: " << texture.size() << " / ";
			// std::cout << "current vertex index: " << vertexIdx[0] << " " << vertexIdx[1] << " " << vertexIdx[2] << "\n";
			for (int i = 0; i < 3; i++) {
				allData.push_back(vertex[vertexIdx[i]].x);
				allData.push_back(vertex[vertexIdx[i]].y);
				allData.push_back(vertex[vertexIdx[i]].z);
				allData.push_back(normal.x);
				allData.push_back(normal.y);
				allData.push_back(normal.z);
				/*allData.push_back(normal[normalIdx[i]].x);
				allData.push_back(normal[normalIdx[i]].y);
				allData.push_back(normal[normalIdx[i]].z);*/
				allData.push_back(texture[textureIdx[i]].x);
				allData.push_back(texture[textureIdx[i]].y);
			}
		}
		else {
			char buf[100];
			objFile.getline(buf, 100);
			// std::cout << buf << '\n';
			// std::cout << "invalid format: " << dataType << " " << tmp.x << " " << tmp.y << " " << tmp.z << "\n";
		}
	}
	allData.push_back(right - left);
	allData.push_back(top - bottom);
	allData.push_back(near - far);
	std::cout << "minimum value of vertex data: " << _min << "\n";
	std::cout << "number of vertex: " << vertex.size() - 1 << "\n";
	std::cout << "number of normal: " << normal.size() - 1 << "\n";
	std::cout << "number of texture coordinate: " << texture.size() << "\n";
	std::cout << "number of face: " << allData.size() / 18 << "\n";
	std::cout << "read " << cnt << " lines\n";
	std::cout << "max width: " << right - left << "\n";
	std::cout << "max height: " << top - bottom << "\n";
	std::cout << "max depth: " << near - far << "\n";
	objFile.close();
	return allData;
}