#include "shader.h"

u32 create_shader_program(const char vertex_path[], const char fragment_path[]) {
	// Compile vertex shader
	char *vertex_shader_source = load_file_to_string(vertex_path);

	u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar* const*)&vertex_shader_source, NULL);
	free(vertex_shader_source);
	glCompileShader(vertex_shader);

	{
		i32 success;
		char info_log[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
			printf("Vertex shader compilation failed:\n%s", info_log);
		}
	}

	// Compile fragment shader
	char *fragment_shader_source = load_file_to_string(fragment_path);

	u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar* const*)&fragment_shader_source, NULL);
	free(fragment_shader_source);
	glCompileShader(fragment_shader);

	i32 success;
	char info_log[512];
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		printf("Fragment shader compilation failed:\n%s", info_log);
	}

	// Link the vertex and fragment shader
	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	{
		i32 success;
		char info_log[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, info_log);
			printf("Shader linking failed:\n%s", info_log);
		}
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

void shader_set_mat4(u32 id, const char name[], mat4 *value) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, value);
}