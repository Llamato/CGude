#include <GLApp.h>
#include <FontRenderer.h>

class MyGLApp : public GLApp {
public:
  Image image{640,480};
  FontRenderer fr{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fe{nullptr};
  std::string text;

  MyGLApp() : GLApp{800,800,1,"Color Picker"} {}

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

  Vec3 blend(Vec3 color1, Vec3 color2, float t) {
    return  (1.0f-t) * color1 + t * color2;
  }
  
  float lerp(float a, float b, float t) {
    return a + (b - a) * t;
  }

  Vec3 calcHSVh(float angle) {
    const uint8_t segmentCount = 6;
    float segment;
    const float positionInSegment = std::modf(angle * segmentCount, &segment);
    switch(static_cast<int>(round(segment))) {
      case 0: return Vec3{1.0f, positionInSegment, 0.0f};
      case 1: return Vec3{1.0f-positionInSegment, 1.0f, 0.0f};
      case 2: return Vec3{0.0f, 1.0f, positionInSegment};
      case 3: return Vec3{0.0f, 1.0f - positionInSegment, 1.0f};
      case 4: return Vec3{positionInSegment, 0.0f, 1.0f};
      case 5: return Vec3{1.0f, 0.0f, 1.0f - positionInSegment};
    }
    return Vec3{0.0f, 0.0f, 0.0f};
  }

  Vec3 calcHSVs(float height, Vec3 hued) {
    const float iheight = 1.0f - height;
    Vec3 saturated;
    saturated.r = lerp(hued.r, 1.0f, iheight);
    saturated.g = lerp(hued.g, 1.0f, iheight);
    saturated.b = lerp(hued.b, 1.0f, iheight);
    return saturated;
  }

  Vec3 calcHSVv(float radius, Vec3 saturated) {
    return radius * saturated;
  }

  Vec3 convertPosFromHSVToRGB(float x, float y, float z) {
    // Completed:
    // enter code here that interprets the mouse's
    // x, y position as H ans S (I suggest to set
    // V to 1.0) and converts that tripple to RGB
    
    const float h = x;
    const float s = y;
    const float v = z;

    return calcHSVv(v, calcHSVs(s, calcHSVh(h)));
  }
  
  int absoluteV = UINT8_MAX;
  void constructHSV(double xPosition, double yPosition) {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;

    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb = convertPosFromHSVToRGB(float(x)/image.width, float(y)/image.height, float(absoluteV)/UINT8_MAX);
        image.setNormalizedValue(x,y,0,rgb.r); image.setNormalizedValue(x,y,1,rgb.g);
        image.setNormalizedValue(x,y,2,rgb.b); image.setValue(x,y,3,255);
      }
    }
  }

  void constructTGV(double xPosition, double yPosition) {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;

    std::vector<Vec3> flag = getTransgenderColors();

    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb = convertPosFromHSVToRGB(float(x)/image.width, float(y)/image.height, float(absoluteV)/UINT8_MAX);
        image.setNormalizedValue(x,y,0,rgb.r); image.setNormalizedValue(x,y,1,rgb.g);
        image.setNormalizedValue(x,y,2,rgb.b); image.setValue(x,y,3,255);
      }
    }
  }

  void constructText(double xPosition, double yPosition) {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;

    float relativeV = float(absoluteV)/UINT8_MAX;
    const Vec3 hsv{float(360*xPosition/s.width),float(1.0-yPosition/s.height), relativeV};
    const Vec3 rgb = convertPosFromHSVToRGB(float(xPosition/s.width), float(1.0-yPosition/s.height), relativeV);
    std::stringstream ss; 
    ss << "HSV: " << hsv << "  RGB: " << rgb; 
    text = ss.str();
  }

  virtual void init() override {
    fe = fr.generateFontEngine();
    constructHSV(0, 0);
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
  }
  
  virtual void mouseMove(double xPosition, double yPosition) override {
    constructText(xPosition, yPosition);
  }

  virtual void mouseWheel(double x_offset, double y_offset, double xPosition, double yPosition) override {
    absoluteV = std::clamp(absoluteV+int(y_offset), 0, UINT8_MAX);
    constructHSV(xPosition, yPosition);
    constructText(xPosition, yPosition);
  }

  virtual void draw() override {
    drawImage(image);

    const Dimensions dim{ glEnv.getFramebufferSize() };
    fe->render(text, dim.aspect(), 0.03f, {0,-0.9f}, Alignment::Center, {0,0,0,1});
  }
} myApp;

#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
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
