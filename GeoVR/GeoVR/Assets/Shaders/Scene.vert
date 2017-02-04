#version 450 core
layout (location = 0) in vec3 position;

out vec3 ourColor;
  
uniform mat4 model;
uniform mat4 viewProjection;

void main() {
	ourColor = vec3(0.25f, 0.25f, 0.25f);
    gl_Position = viewProjection * model * vec4(position, 1.0f);

    if (position.x == 0.0f) {
    	ourColor = vec3(0.0f, 0.0f, 1.0f);
	} else if (position.z == 0.0f) {
		ourColor = vec3(1.0f, 0.0f, 0.0f);
    }
}