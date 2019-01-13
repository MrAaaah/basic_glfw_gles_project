#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>


#define ASSERT(cond) do {\
    if (cond == 0)\
    {\
    printf("[ASSERT %s:%d]: %s\n", __FILE__, __LINE__, #cond);\
    exit(1);\
    }\
} while(0)


#define FULLSCREEN

#ifdef FULLSCREEN
static const GLuint WIDTH = 1920;
static const GLuint HEIGHT = 1080;
#else
static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
#endif

void file_read(const char * filepath, char ** content)
{
    FILE * fh = fopen(filepath, "r");
    ASSERT(fh != NULL);

    fseek(fh, 0, SEEK_END);
    size_t len = ftell(fh);
    rewind(fh);

    *content = malloc(len + 1);
    fread(*content, 1, len, fh);
    (*content)[len] = '\0';

    fclose(fh);
}

GLint common_get_shader_program(const char *vertex_shader_filepath, 
        const char *fragment_shader_filepath) {
	enum Consts {INFOLOG_LEN = 512};
	GLchar infoLog[INFOLOG_LEN];
	GLint fragment_shader;
	GLint shader_program;
	GLint success;
	GLint vertex_shader;

	/* Vertex shader */
    char * vertex_shader_src;
    file_read(vertex_shader_filepath, &vertex_shader_src);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, (const char **)&vertex_shader_src, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	/* Fragment shader */
    char * fragment_shader_src;
    file_read(fragment_shader_filepath, &fragment_shader_src);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char **)&fragment_shader_src, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	/* Link shaders */
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(void) {
	GLuint shader_program, vbo;
	GLint pos;
	GLFWwindow* window;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWmonitor * primary_monitor = NULL;
    #ifdef FULLSCREEN
    primary_monitor = glfwGetPrimaryMonitor();
    #endif
	window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, primary_monitor, NULL);
	glfwMakeContextCurrent(window);

	printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
	printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    glfwSetKeyCallback(window, key_callback);

	shader_program = common_get_shader_program("vertex.vs", "fragment.fs");
	pos = glGetAttribLocation(shader_program, "position");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, WIDTH, HEIGHT);

    float vertices[] = {
        -0.5f,  -0.5f, 0.0f, 1.0f,
        +0.5f,  -0.5f, 0.0f, 1.0f,
        +0.0f,  +0.5f, 0.0f, 1.0f,
    };

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(pos, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    float rotation_angle = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);

        double time = glfwGetTime();

        rotation_angle = time;

        float rotation_mat[16] = {
            cosf(rotation_angle), sinf(rotation_angle), 0.0f, 0.0f,
            -sinf(rotation_angle), cosf(rotation_angle), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "rotation_mat"),
                1,
                0,
                rotation_mat);
        
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return EXIT_SUCCESS;
}
