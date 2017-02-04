#version 450 core
in vec3 Normal;
in vec3 CameraPosition;
out vec4 color;

float lambert(vec3 N, vec3 L) {
	return max(dot(normalize(N), normalize(L)), 0.15);
}

void main() {
	vec3 LightColor = vec3(0.5f, 0.5f, 0.5f);
	vec3 result = LightColor * lambert(Normal, CameraPosition) * 1.3f;

	color = vec4(result, 1.0f);
}