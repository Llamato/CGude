#include <fstream>

#include <GLApp.h>

#define NUM_TRIANGLES 3
#define FLOATS_PER_POINT 3 
#define FLOATS_PER_COLOR 3
#define FLOATS_PER_VERTEX (FLOATS_PER_POINT + FLOATS_PER_COLOR)
#define VERTICIES_PER_TRIANGLE 3
#define FLOATS_PER_TRIANGLE VERTICIES_PER_TRIANGLE * FLOATS_PER_VERTEX

#define ROTATION_PERIOD 10

#define VERTEX_X_OFFSET 0
#define VERTEX_Y_OFFSET 1
#define VERTEX_Z_OFFSET 2
#define COLOR_R_OFFSET 3
#define COLOR_G_OFFSET 4
#define COLOR_B_OFFSET 5


class MyGLApp : public GLApp {
public:
  Mat4 modelView{};
  Mat4 projection{};
  GLuint program{0};
  GLint modelViewMatrixUniform{-1};
  GLint projectionMatrixUniform{-1};
  GLuint vbos{0};
  GLuint vaos{0};
  
  GLfloat vertexData[NUM_TRIANGLES * VERTICIES_PER_TRIANGLE * FLOATS_PER_VERTEX];
  Vec3 triangleCenterPoints[NUM_TRIANGLES];

  void debugPrintVertexArray() {
    std::cout << "X\tY\tZ\tR\tG\tB\n";
    for(size_t currentTriangle = 0; currentTriangle < NUM_TRIANGLES; currentTriangle++) {
      for(size_t currentVertex = 0; currentVertex < VERTICIES_PER_TRIANGLE; currentVertex++) {
        for(size_t currentFloat = 0; currentFloat < FLOATS_PER_VERTEX; currentFloat++) {
          std::cout << vertexData[currentTriangle * VERTICIES_PER_TRIANGLE + currentVertex * FLOATS_PER_VERTEX + currentFloat] << '\t';
        }
        std::cout << '\n';
      }
      std::cout << "\n";
    }
  }

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

  void setVertexColor(GLfloat* vertices, size_t vertexStart, Vec3 color) {
    vertices[vertexStart + COLOR_R_OFFSET] = color.r;
    vertices[vertexStart + COLOR_G_OFFSET] = color.g;
    vertices[vertexStart + COLOR_B_OFFSET] = color.b;
  }

  void setTriangleColors(GLfloat* verticies, size_t triangleStart, Vec3 colorA, Vec3 colorB, Vec3 colorC) {
    setVertexColor(verticies, triangleStart, colorA);
    setVertexColor(verticies, triangleStart + FLOATS_PER_VERTEX, colorB);
    setVertexColor(verticies, triangleStart + 2 * FLOATS_PER_VERTEX, colorC);
  }

  void generateEquilateralTriangle(GLfloat* vertices, size_t start, size_t stride, Vec3 center, float height){
    const float halfBase = height / std::sqrt(3.0f);
    Vec3 A = center + Vec3{ 0.0f,        height * 0.5f,  0.0f };
    Vec3 B = center + Vec3{ -halfBase,  -height * 0.5f,  0.0f };
    Vec3 C = center + Vec3{  halfBase,  -height * 0.5f,  0.0f };
    vertices[start + VERTEX_X_OFFSET] = A.x;
    vertices[start + VERTEX_Y_OFFSET] = A.y;
    vertices[start + VERTEX_Z_OFFSET] = A.z;
    vertices[start + stride + VERTEX_X_OFFSET] = B.x;
    vertices[start + stride + VERTEX_Y_OFFSET] = B.y;
    vertices[start + stride + VERTEX_Z_OFFSET] = B.z;
    vertices[start + 2 * stride + VERTEX_X_OFFSET] = C.x;
    vertices[start + 2 * stride + VERTEX_Y_OFFSET] = C.y;
    vertices[start + 2 * stride + VERTEX_Z_OFFSET] = C.z;
}

void setupTriangles() {
  const Vec3 colorRed = htmlColorToOpenGlColor("#FF0000");
  const Vec3 colorMagenta = htmlColorToOpenGlColor("#FF00FF");
  const Vec3 colorOrange = htmlColorToOpenGlColor("#FFAA00");
  const Vec3 colorTurquesa = htmlColorToOpenGlColor("#00FFFF");
  const Vec3 colorBlue = htmlColorToOpenGlColor("#0000FF");
  const Vec3 colorYellow = htmlColorToOpenGlColor("#FFFF00");
  const Vec3 colorGreen = htmlColorToOpenGlColor("#00FF00");
  setTriangleColors(vertexData, 0, colorRed, colorMagenta, colorOrange);
  setTriangleColors(vertexData, FLOATS_PER_TRIANGLE, colorMagenta, colorBlue, colorTurquesa);
  setTriangleColors(vertexData, 2 * FLOATS_PER_TRIANGLE, colorYellow, colorGreen, colorTurquesa);

  triangleCenterPoints[0] = Vec3{-0.5f , 0.0f, 0};
  generateEquilateralTriangle(vertexData, FLOATS_PER_TRIANGLE,  FLOATS_PER_VERTEX, triangleCenterPoints[0], .66f);
  triangleCenterPoints[1] = Vec3{0.0f , 1.0f, 0};
  generateEquilateralTriangle(vertexData, 0, FLOATS_PER_VERTEX, triangleCenterPoints[1], .66f);
  triangleCenterPoints[2] = Vec3{0.5f , 0.0f, 0};
  generateEquilateralTriangle(vertexData, 2 * FLOATS_PER_TRIANGLE, FLOATS_PER_VERTEX, triangleCenterPoints[2], .66f);
}
  
  MyGLApp()
    : GLApp(800,600,4,"Assignment 11 - Triforce")
  {}
  
  virtual void init() override {
    setupTriangles();
    setupShaders();
    setupGeometry();
  }

  virtual void animate(double animationTime) override {
    
  }

  virtual void draw() override {
    GL(glUseProgram(program));
    modelView = Mat4::translation(0.0f, -1.0f, 0.0f);
    GL(glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_TRUE, modelView));
    
    GL(glBindVertexArray(vaos));
    GL(glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES * VERTICIES_PER_TRIANGLE));
    GL(glBindVertexArray(0));
    GL(glUseProgram(0));
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override{
    GLApp::resize(winDim, fbDim);

    const float ratio = fbDim.aspect();
    projection = Mat4::ortho(-ratio * 1.5f, ratio * 1.5f, -1.5f, 1.5f, -10.0f, 10.0f);
    GL(glUseProgram(program));
    GL(glUniformMatrix4fv(projectionMatrixUniform, 1, GL_TRUE, projection));
    GL(glUseProgram(0));
  }

  std::string loadFile(const std::string& filename) {
    std::ifstream shaderFile{ filename };
    if (!shaderFile)
    {
      throw GLException{ std::string("Unable to open file ") + filename };
    }
    std::string str;
    std::string fileContents;
    while (std::getline(shaderFile, str)) {
      fileContents += str + "\n";
    }
    return fileContents;
  }
  
  GLuint createShaderFromFile(GLenum type, const std::string& sourcePath) {
    const std::string shaderCode = loadFile(sourcePath);
    const std::string fullSource = GLProgram::getShaderPreamble() + shaderCode;
    const GLchar* c_shaderCode = fullSource.c_str();
    const GLuint s = glCreateShader(type);
    GL(glShaderSource(s, 1, &c_shaderCode, NULL));
    glCompileShader(s); checkAndThrowShader(s);
    return s;
  }
  
  void setupShaders() {
    const std::string vertexSrcPath = "vertexShader.vert";
    const std::string fragmentSrcPath = "fragmentShader.frag";
    GLuint vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    GLuint fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    
    program = glCreateProgram();
    GL(glAttachShader(program, vertexShader));
    GL(glAttachShader(program, fragmentShader));
    GL(glLinkProgram(program));
    checkAndThrowProgram(program);
    
    GL(glUseProgram(program));
    modelViewMatrixUniform = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
    GL(glUseProgram(0));
  }
  
  void setupGeometry() {
    GL(glUseProgram(program));
    const GLint vertexPosition = glGetAttribLocation(program, "vertexPosition");
    const GLint vertexColor = glGetAttribLocation(program, "vertexColor");
    
    GL(glGenVertexArrays(1, &vaos));
    GL(glBindVertexArray(vaos));
    
    GL(glGenBuffers(1, &vbos));
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW));
    
    GL(glEnableVertexAttribArray(vertexPosition));
    GL(glEnableVertexAttribArray(vertexColor));
    GL(glVertexAttribPointer(vertexPosition, FLOATS_PER_POINT, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)0));
    GL(glVertexAttribPointer(vertexColor, FLOATS_PER_COLOR, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)(FLOATS_PER_POINT * sizeof(GLfloat))));
    
    GL(glBindVertexArray(0));
    GL(glUseProgram(0));
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override
  {

  }
} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
    std::vector<std::string> args{ argv + 1, argv + argc };
#endif
    try {
        myApp.run();
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
