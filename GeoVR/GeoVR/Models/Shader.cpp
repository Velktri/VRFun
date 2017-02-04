#include "Shader.h"



Shader::Shader(const char* vertFile, const char* fragFile) {
	std::string tempvert = ReadShaderFile(vertFile);
	std::string tempfrag = ReadShaderFile(fragFile);
	const GLchar* vertSource = tempvert.c_str();
	const GLchar* fragSource = tempfrag.c_str();

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertSource, NULL);
	glCompileShader(vertShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertShader);
	glAttachShader(ShaderProgram, fragShader);
	glBindFragDataLocation(ShaderProgram, 0, "outColor");
	glLinkProgram(ShaderProgram);

	glDeleteShader(fragShader);
	glDeleteShader(vertShader);
}


Shader::~Shader() {
	glDeleteProgram(ShaderProgram);
}

void Shader::Use() {
	glUseProgram(ShaderProgram);

	// Create transformations and get their uniform location
	for each (ShaderUniforms shade in UniformList) {
		ShaderList[shade.Name] = glGetUniformLocation(ShaderProgram, shade.Name.c_str());
	}
}

GLuint Shader::GetShader() {
	return ShaderProgram;
}


std::string Shader::ReadShaderFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
		CheckUniform(line);
	}

	fileStream.close();

	std::unordered_map<std::string, std::vector<std::string>> StructList = ParseStructs(content);
	if (!StructList.empty()) {
		std::vector<ShaderUniforms> tempList;
		for each (ShaderUniforms uniforms in UniformList) {
			std::unordered_map<std::string, std::vector<std::string>>::iterator i = StructList.find(uniforms.Type);
			if (i != StructList.end()) {
				std::string name = uniforms.Name;
				for each (std::string attr in i->second) {
					ShaderUniforms data;
					data.Type = i->first;
					data.Name = name + "." + attr;
					tempList.push_back(data);
				}
			}
		}

		while (!StructList.empty()) {
			std::string type = StructList.begin()->first;
			for (int i = 0; i < UniformList.size(); i++) {
				if (UniformList[i].Type == type) {
					UniformList.erase(UniformList.begin() + i);
					break;
				}
			}
			StructList.erase(StructList.begin());
		}
		UniformList.insert(UniformList.end(), tempList.begin(), tempList.end());
	}

	return content;
}

void Shader::CheckUniform(std::string line) {
	std::istringstream iss(line);
	std::string item;
	std::vector<std::string> words;
	while (std::getline(iss, item, ' ')) {
		words.push_back(item);
	}

	if (words.size() > 0 && words.at(0).compare("uniform") == 0) {
		ShaderUniforms data;

		std::string word = words.at(2);
		word.pop_back();
		data.Name = word;
		data.Type = words.at(1);
		UniformList.push_back(data);
	}

	iss.clear();
}

std::unordered_map<std::string, std::vector<std::string>> Shader::ParseStructs(std::string Shader) {
	std::unordered_map<std::string, std::vector<std::string>> StructList;
	std::istringstream iss(Shader);
	std::string line;
	std::string StructName = "";

	while (std::getline(iss, line)) {
		std::string item;
		std::istringstream iss2(line);
		std::vector<std::string> words;

		while (std::getline(iss2, item, ' ')) {
			if (item.length() > 0 && item.compare("\t") != 0) {
				words.push_back(item);
			}
		}

		if (words.size() > 0 && words.at(0).compare("};") == 0) {
			StructName = "";
		} else if (words.size() > 0 && words.at(0).compare("struct") == 0) {
			StructName = words.at(1);
			StructList.insert(std::pair<std::string, std::vector<std::string>>(StructName, std::vector<std::string>()));
		} else if (words.size() > 0 && StructName.length() > 0) {
			std::string word = words.at(1);
			word.pop_back();
			StructList[StructName].push_back(word);
		}

		iss2.clear();
	}

	iss.clear();
	return StructList;
}
