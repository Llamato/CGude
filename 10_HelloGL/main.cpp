#include <GLEnv.h>
#include <Mat4.h>

#include <iostream>
#include <sstream>

#include <GLProgram.h>

#define FLOATS_PER_VERTEX 3
#define VERTS_PER_TRIANGLE 3
#define NUM_TRIANGLES 2

#define ERROR_MESSAGE_MAX_LENGTH 255

GLuint vbo;
GLuint vao;
GLuint program;

// triangle vertex position data
constexpr float triangles[] = {
  -0.5f,  0.5f, 0.0f, //Top left
  0.5f, 0.5f, 0.0f,  //Top right
  0.5f, -0.5f, 0.0f, //Bottom right
  -0.5f,  0.5f, 0.0f, //Top left
  -0.5f, -0.5f, 0.0f, //Bottom left
  0.5f, -0.5f, 0.0f //Bottom right
};

const GLchar* vertexShaderSource{
R"(in vec3 vPos;
out vec2 vNdc;
void main()
{
  vec4 clip = vec4(vPos, 1.0);
  gl_Position = clip;
  vNdc = clip.xy;
}
)"
};

const GLchar* fragmentShaderSource{
R"(out vec4 fragColor;
in vec2 vNdc;
uniform vec3 colorOrange;
uniform vec3 colorGreen;

void main()
{
  vec2 vNorminal = vNdc + vec2(0.5);
  fragColor = mix(vec4(colorOrange, 1.0), vec4(colorGreen, 1.0), float(1.0 - vNorminal.x > vNorminal.y));
}
)"
};

Vec3 htmlColorToOpenGlColor(const std::string html) {
  std::string htmlR = html.substr(1, 2);
  std::string htmlG = html.substr(3, 2);
  std::string htmlB = html.substr(5, 2);
  unsigned int absoluteR = std::stoul(htmlR, nullptr, 16);
  unsigned int absoluteG = std::stoul(htmlG, nullptr, 16);
  unsigned int absoluteB = std::stoul(htmlB, nullptr, 16);
  float relativeR = static_cast<float>(absoluteR) / UINT8_MAX;
  float relativeG = static_cast<float>(absoluteG) / UINT8_MAX;
  float relativeB = static_cast<float>(absoluteB) / UINT8_MAX; 
  return Vec3{relativeR, relativeG, relativeB};
}

GLint getGlUniformLocation(GLuint shaderProgram, const char* varname) {
  GLint uniform = glGetUniformLocation(shaderProgram, varname);
  char infoLog[ERROR_MESSAGE_MAX_LENGTH];
  glGetProgramInfoLog(shaderProgram, ERROR_MESSAGE_MAX_LENGTH, NULL, infoLog);
  if(uniform == GL_INVALID_VALUE || uniform == GL_INVALID_OPERATION){
    printf("%s %s\nError (%i): %s\n", varname, "uniform could not be found!", uniform, infoLog);
    return -1;
  }
  return uniform;
}

void setGlUniform3f(GLuint shaderProgram, const char* varname, Vec3 vector) {
  GLuint uniform = getGlUniformLocation(shaderProgram, varname);
  if(uniform != GL_INVALID_VALUE && uniform != GL_INVALID_OPERATION) glUniform3f(uniform, vector.x, vector.y, vector.z);
}

static void draw(void* arg=nullptr) {
  GL( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );
  GL( glClear(GL_COLOR_BUFFER_BIT) );
  
  GL( glBindVertexArray(vao) );
  GL( glUseProgram(program) );
  setGlUniform3f(program, "colorOrange", htmlColorToOpenGlColor("#e58033"));
  setGlUniform3f(program, "colorGreen", htmlColorToOpenGlColor("#33e533"));
  GL( glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES * VERTS_PER_TRIANGLE) );
  GL( glBindVertexArray(0) );
}

static void setupShaders() {
  // create the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const std::string fullSourceV = GLProgram::getShaderPreamble() + vertexShaderSource;
  const GLchar* c_shaderCodeV = fullSourceV.c_str();
  GL( glShaderSource(vertexShader, 1, &c_shaderCodeV, NULL) );
  GL( glCompileShader(vertexShader) );
  checkAndThrowShader(vertexShader);

  // create the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const std::string fullSourceF = GLProgram::getShaderPreamble() + fragmentShaderSource;
  const GLchar* c_shaderCodeF = fullSourceF.c_str();
  GL( glShaderSource(fragmentShader, 1, &c_shaderCodeF, NULL) );
  GL( glCompileShader(fragmentShader) );
  checkAndThrowShader(fragmentShader);

  // link shaders into program
  program = glCreateProgram();
  GL( glAttachShader(program, vertexShader) );
  GL( glAttachShader(program, fragmentShader) );
  GL( glLinkProgram(program) );
  checkAndThrowProgram(program);

  GL( glDeleteShader(vertexShader) );
  GL( glDeleteShader(fragmentShader) );
}

static void setupGeometry() {
  // define VAO for triangle
  GL( glGenVertexArrays(1, &vao) );
  GL( glBindVertexArray(vao) );

  // upload vertex positions to VBO
  GL( glGenBuffers(1, &vbo) );
  GL( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
  GL( glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW) );

  GL( glEnableVertexAttribArray(0) );
  GL( glVertexAttribPointer(0, FLOATS_PER_VERTEX, GL_FLOAT, GL_FALSE, VERTS_PER_TRIANGLE * sizeof(float), (void*)0) );

  GL( glBindVertexArray(0) );
}

#ifndef __EMSCRIPTEN__
static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

static void sizeCallback(GLFWwindow* window, int width, int height) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  GL( glViewport(0, 0, w, h) );
}

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    try {
        GLEnv glEnv{ 800,600,1,"My First OpenGL Program",true,false };
        glEnv.setKeyCallback(keyCallback);
        glEnv.setResizeCallback(sizeCallback);
        setupShaders();
        setupGeometry();
        while (!glEnv.shouldClose()) {
            draw();
            glEnv.endOfFrame();
        }
    }
    catch (const GLException& e) {
        std::stringstream ss;
        ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
        std::cerr << ss.str().c_str() << std::endl;
#else
        MessageBoxA(
            NULL,
            ss.str().c_str(),
            "OpenGL Error",
            MB_ICONERROR | MB_OK
        );
#endif
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#else
int main(int argc, char** argv) {
  GLEnv glEnv{800,600,1,"My First OpenGL Program",true,false};
  setupShaders();
  setupGeometry();
  emscripten_set_main_loop_arg(draw, nullptr, 0, 1);
  while (!glEnv.shouldClose()) {
    draw();
    glEnv.endOfFrame();
  }
  return EXIT_SUCCESS;
}
#endif
