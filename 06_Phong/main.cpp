#include <GLApp.h>

#include "Triangle.h"
#include "AmbientShader.h"
#include "DiffuseShader.h"
#include "PhongShader.h"
#include "BumpPhongShader.h"

class MyGLApp : public GLApp
{
public:
  PhongShader *baseShader;
  Vertex baseVertex1;
  Vertex baseVertex2;
  Vertex baseVertex3;

  Image image{ 600, 600 };

  MyGLApp() : GLApp{ 600, 600, 1, "Phong Lighting" } {}

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

  std::vector<Vec3> getDegenerateColors(void) {
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

    
  Vec3 rotateColorWheel(std::vector<Vec3> colors) {
    static size_t currentWheelColor = 0;
    if(currentWheelColor == colors.size()) {
      currentWheelColor = 0;
    }
    currentWheelColor++;
    return colors[currentWheelColor];
  }

  Material degenMaterial() {
    const static std::vector<Vec3> degenColors = getDegenerateColors();
    const Vec3 color = rotateColorWheel(degenColors);
    Material material{color, color, color};
    return material;
  }

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));

    // define some materials for our triangles
    Material red{ Vec3{1, 0, 0} };
    Material green{ Vec3{0, 1, 0} };
    Material blue{ Vec3{0, 0, 1} };
    Material lightOrange{ Vec3{1, 0.765f, 0.482f } };

    // define the vertices for the first triangle
    Vertex v0{ Vec3{12, 360, 0}, red };
    Vertex v1{ Vec3{240, 588, 0}, green };
    Vertex v2{ Vec3{444, 48, 0}, blue };

    #ifdef SAUCE
    v0.material = degenMaterial();
    v1.material = degenMaterial();
    v2.material = degenMaterial();
    #endif

    // create a simple shader for the first triangle
    const AmbientShader as{};

    // create and draw the first triangle
    Triangle triangle1(v0, v1, v2, as);
    triangle1.draw(image);

    // define new vertices for the second triangle
    Vertex d0{ Vec3{12, 384, 0}, lightOrange };
    Vertex d1{ Vec3{216, 588, 0}, lightOrange };
    Vertex d2{ Vec3{12, 588, 0}, lightOrange };

    #if SAUCE
      d0.material = degenMaterial();
      d1.material = degenMaterial();
      d2.material = degenMaterial();
    #endif

    // create a diffuse shader for the second triangle
    Vec3 lightPos{ 12, 486, 50 };
    Vec3 light_diffuse_color{ 1, 1, 1 };
    const DiffuseShader diffuseShader{ lightPos, light_diffuse_color };

    // create and draw the diffuse shaded triangle
    Triangle triangle2(d0, d1, d2, diffuseShader);
    triangle2.draw(image);

    // create a new vertex for the third triangle
    Vertex v3{ Vec3{36, 60, 0}, blue };

    #ifdef SAUCE
      v3.material = degenMaterial();
    #endif

    // create a phong shader for the third triangle with new light coefficients
    const Vec3 viewer{ image.width / 2.0f, image.height / 2.0f, 500 };
    const Vec3 light_ambient_color{ 0.25098f, 0.25098f, 0.25098f };
    light_diffuse_color = { 0.5196f, 0.5196f, 0.5196f };
    const Vec3 light_specular_color{ 1, 1, 1 };
    lightPos = Vec3{ 0, 0, 500 };
    const PhongShader phongShader{ viewer, lightPos, light_ambient_color, light_diffuse_color, light_specular_color, 20 };

    // create and draw the phong shaded triangle
    Triangle triangle3(v0, v2, v3, phongShader);
    triangle3.draw(image);

    // define new vertices with custom normals for the fourth triangle
    Vertex a{ Vec3{264, 588, 0}, red, Vec3{-1, -1, 1} };
    Vertex b{ Vec3{588, 588, 0}, red, Vec3{0.5f, 0.5f, 1} };
    Vertex c{ Vec3{588, 372, 0}, red, Vec3{-1, -1, 1} };

    #ifdef SAUCE
      a.material = degenMaterial();
      b.material = degenMaterial();
      c.material = degenMaterial(); 
    #endif

    // create and draw the red phong shaded triangle
    Triangle triangle4(a, b, c, phongShader);
    triangle4.draw(image);

    // define a new vertex for the fifth triangle
    Vertex v4(Vec3(588, 360, 0), green);

    #ifdef SAUCE
      v4.material = degenMaterial();
    #endif

    // create a bump shader for the fifth triangle
    BumpPhongShader bump(phongShader, 50, 0.3f);

    // create and draw the bump-phong shaded triangle
    Triangle triangle5(v1, v4, v2, bump);
    baseShader = new PhongShader(phongShader);
    baseVertex1 = v1;
    baseVertex2 = v4;
    baseVertex3 = v2;
    triangle5.draw(image);
    
  }

  virtual void draw() override {
    drawImage(image);
  }

  float triangleFunction(float t) {
    return std::fmaxf(1 - std::fabsf(t), 0.0f);
  }

#ifdef EXTRA
  virtual void animate(double animationTime) override {
    const float maxCellSize = 200.0f;
    const float maxBumpHeight = 1.0f;
    const float animationCycleTime = 10.0f;
    float wholeTime, fractionalTime;
    fractionalTime = std::modf(animationTime / animationCycleTime, &wholeTime);
    const float trinalgeTime = triangleFunction(fractionalTime * 2.0f - 1.0f);
    BumpPhongShader bumpShader{*baseShader, maxCellSize * trinalgeTime, maxBumpHeight * trinalgeTime};
    Triangle animatedTriagle = Triangle{baseVertex1, baseVertex2, baseVertex3, bumpShader};
    animatedTriagle.draw(image);
    drawImage(image);
  }
#endif

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
