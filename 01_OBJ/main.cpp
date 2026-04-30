#include "Vec3.h"
#include <GLApp.h>
#include <OBJFile.h>
#include <Mat4.h>
#include <cstdint>

class MyGLApp : public GLApp {
public:
  double angle{0.0};
  std::vector<float> data;
  
  MyGLApp() : GLApp{640,480,1,"Shared vertices to explicit representation demo"} {}

  void printVec3(Vec3 vec) {
    printf("Vec3{%f.02, f.02, f.02}\n", vec.x, vec.y, vec.z);
  }

  void printObj(OBJFile obj) {
    size_t indices_remaining = obj.indices.size();
    size_t vertices_remaining = obj.vertices.size();
    size_t normals_remaining = obj.normals.size();
    while(indices_remaining > 0 || vertices_remaining > 0 || normals_remaining > 0){
      if(indices_remaining > 0){
        indices_remaining--;
        printf("index %lu: (%lu, %lu, %lu)\n", indices_remaining, obj.indices[indices_remaining][0], obj.indices[indices_remaining][1], obj.indices[indices_remaining][2]);
      }
      if(vertices_remaining > 0){
        vertices_remaining--;
        printf("vertex %lu: (%f.02, %f.02, %f.02)\n", vertices_remaining, obj.vertices[vertices_remaining][0], obj.vertices[vertices_remaining][1], obj.vertices[vertices_remaining][2]);
      }
      if(normals_remaining > 0){
        normals_remaining--;
        printf("normal %lu: (%f.02, %f.02, %f.02)\n", normals_remaining, obj.normals[normals_remaining][0], obj.normals[normals_remaining][1], obj.normals[normals_remaining][2]);
      }
      printf("\n");
    }
    printf("printed %lu indices, %lu vertices, %lu normals\n", obj.indices.size(), obj.vertices.size(), obj.normals.size());
  }

  void writeTriangleData(void) {
    data.push_back(0.0f); data.push_back(0.5f); data.push_back(0.0f);  // position
    data.push_back(1.0f); data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); // color
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); // normal

    data.push_back(-0.5f); data.push_back(-0.5f); data.push_back(0.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f); data.push_back(1.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);

    data.push_back(0.5f); data.push_back(-0.5f); data.push_back(0.0f);
    data.push_back(0.0f); data.push_back(1.0f); data.push_back(0.0f); data.push_back(1.0f);
    data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);
  }

  std::vector<Vec3> getCheckerboardColors(void) {
    Vec3 magenta = Vec3{1.0f, 0.0f, 1.0f};
    Vec3 black = Vec3{0.0f, 0.0f, 0.0f};
    std::vector<Vec3> colors;
    colors.push_back(magenta);
    colors.push_back(black);
    return colors;
  }

  std::vector<Vec3> getTransgenderColors(void) {
    Vec3 blue = htmlColorToOpenGlColor("#5BCFFA");
    Vec3 pink = htmlColorToOpenGlColor("#F5A9B8");
    Vec3 white = Vec3{1.0f, 1.0f, 1.0f};
    std::vector<Vec3> colors;
    colors.push_back(blue);
    colors.push_back(pink);
    colors.push_back(white);
    colors.push_back(pink);
    colors.push_back(blue);
    return colors;
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

  struct MeshDimensions {
    Vec3 minimum;
    Vec3 maximum;
  };

  struct MeshDimensions getMeshExtends(const std::vector<Vec3>& vertices) {
    float minx = 1.0f;
    float maxx = -1.0f;
    float miny = 1.0f;
    float maxy = -1.0f;
    float minz = 1.0f;
    float maxz = -1.0f;
    for(size_t currentVertex = 0; currentVertex < vertices.size(); currentVertex++) {
      float curx = vertices[currentVertex].x;
      float cury = vertices[currentVertex].y;
      float curz = vertices[currentVertex].z;
      if(curx < minx) minx = curx;
      if(curx > maxx) maxx = curx;
      if(cury < miny) miny = cury;
      if(cury > maxy) maxy = cury;
      if(curz < minz) minz = curz;
      if(curz > maxz) maxz = curz;
    }
    struct MeshDimensions extends = {Vec3{minx, miny, minz}, Vec3{maxx, maxy, maxz}};
    return extends;
  }
  
  void writeObjData(const OBJFile& obj, const std::vector<Vec3>& colors) {
    struct MeshDimensions meshDimensions = getMeshExtends(obj.vertices);
    const size_t verticesPerTriangle = 3;
    for(size_t currentIndex = 0; currentIndex < obj.indices.size(); currentIndex++) {
      OBJFile::IndexType currentTriangle = obj.indices[currentIndex];
      for(size_t currentVertex = 0; currentVertex < verticesPerTriangle; currentVertex++) {
        float posx = obj.vertices[currentTriangle[currentVertex]].x;
        float posy = obj.vertices[currentTriangle[currentVertex]].y;
        float posz = obj.vertices[currentTriangle[currentVertex]].z;
        data.push_back(posx);
        data.push_back(posy);
        data.push_back(posz);

        Vec3 currentColor = colors[static_cast<size_t>(((posy - meshDimensions.minimum.y) / (meshDimensions.maximum.y - meshDimensions.minimum.y)) * colors.size())];
        data.push_back(currentColor.r);
        data.push_back(currentColor.g);
        data.push_back(currentColor.b);
        data.push_back(1.0f);

        data.push_back(obj.normals[currentTriangle[currentVertex]].x);
        data.push_back(obj.normals[currentTriangle[currentVertex]].y);
        data.push_back(obj.normals[currentTriangle[currentVertex]].z);
      }
    }
  }

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glEnable(GL_DEPTH_TEST));

    const OBJFile m{"bunny.obj", true};
    writeObjData(m, getTransgenderColors());
  }
  
  virtual void animate(double animationTime) override {
    angle = animationTime*30;
  }
  
  virtual void draw() override {
    setDrawProjection(Mat4::perspective(45, glEnv.getFramebufferSize().aspect(), 0.0001f, 100.0f));
    setDrawTransform(Mat4::lookAt({0,0,2},{0,0,0},{0,1,0}) * Mat4::rotationY(float(angle)));
    drawTriangles(data, TrisDrawType::LIST, false, true);
  }
  
  const unsigned int ACTIVE_MODEL_BUNNY = 0;
  const unsigned int ACTIVE_MODEL_CUBE = 1; 
  bool activeModel = ACTIVE_MODEL_BUNNY;
  virtual void keyboardChar(unsigned int key) override {
    const unsigned int KEYCODE_SPACE = 32;
    if(key != KEYCODE_SPACE) return;
    if(activeModel == ACTIVE_MODEL_CUBE){
      activeModel = ACTIVE_MODEL_BUNNY;
      data.clear();
      const OBJFile m{"bunny.obj", true};
      writeObjData(m, getTransgenderColors());
    }else{
      activeModel = ACTIVE_MODEL_CUBE;
      data.clear();
      const OBJFile m{"blendercube.obj", true};
      writeObjData(m, getCheckerboardColors());
    }
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
