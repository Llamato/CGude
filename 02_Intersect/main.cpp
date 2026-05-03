#include <GLApp.h>
#include <cmath>
#include <optional>

class MyGLApp : public GLApp {
public:
  Image image{1024,1024};
  
  MyGLApp() : GLApp{1024,1024,1,"Intersection Demo"} {}

  struct PolarVector2 {
    float radius;
    float angle; 
  };

  struct Light {
    struct PolarVector2 position;
    Vec3 diffuseColor;
  };

  const Vec3 sphereCenter{0.0f, 0.0f, -4.0f};
  const float radius = 2.0f;
  const Vec3 rayOrigin{0.0f, 0.0f, 4.0f};
  const Vec3 topLeftCorner{-2.0f, 2.0f, 0.0f}, topRightCorner{2.0f, 2.0f, 0.0f};
  const Vec3 bottomLeftCorner{-2.0f, -2.0f, 0.0f}, bottomRightCorner{2.0f, -2.0f, 0.0f};
  const float lightRingHeight = 0.0f;
  std::vector<struct Light> lights;

  Vec3 convertHtmlColorToOpenGlColor(const std::string html) {
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

  Vec2 convertPolarToCatesian(const struct PolarVector2 polarVector) {
    Vec2 catesianVector;
    catesianVector.x = polarVector.radius * cosf(polarVector.angle);
    catesianVector.y = polarVector.radius * sinf(polarVector.angle);
    return catesianVector;
  }

  struct PolarVector2 convertCatesianToPolar(const Vec2 catesianVector) {
    struct PolarVector2 polarVector;
    polarVector.radius = sqrt(pow(catesianVector.x, 2) + pow(catesianVector.y, 2));
    polarVector.angle = atan2(catesianVector.y, catesianVector.x);
    return polarVector;
  }

  void rotateLightRing(const float angle) {
    for(size_t currentLight = 0; currentLight < lights.size(); currentLight++) {
      lights[currentLight].position.angle = fmod(angle + 2.0f * M_PI / lights.size() * currentLight, 2.0f * M_PI);
    }
  }

  std::optional<Vec3> raySphereIntersect(const Vec3& sphereCenter, const float& radius, const Vec3& rayOrigin, const Vec3& pixelPos) {
    // COMPLETED:
    // Implement a ray/sphere intersection here
    // the sphere is given as sphereCenter and
    // radius, the ray starts at rayOrigin and
    // passes through pixelPos. If no interscation
    // is found, simply return {} otherwise return
    // the intersection position
    // If the function works correctly, you should
    // see a glossy red sphere, illuminated from
    // the top front

    float x_1 = rayOrigin.x;
    float y_1 = rayOrigin.y;
    float z_1 = rayOrigin.z;
    float x_2 = pixelPos.x;
    float y_2 = pixelPos.y;
    float z_2 = pixelPos.z;
    float x_3 = sphereCenter.x;
    float y_3 = sphereCenter.y;
    float z_3 = sphereCenter.z;
    float r = radius;
    float a = pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2) + pow(z_2 - z_1, 2);
    float b = 2 * ((x_2 - x_1) * (x_1 - x_3) + (y_2 - y_1) * (y_1 - y_3) + (z_2 - z_1) * (z_1 - z_3));
    float c = pow(x_3, 2) + pow(y_3, 2) + pow(z_3, 2) + pow(x_1, 2) + pow(y_1, 2) + pow(z_1, 2) - 2 * (x_3 * x_1 + y_3 * y_1 + z_3 * z_1) - pow(r, 2);
    float isIntersecting = pow(b, 2) - 4 * a * c;
    if(isIntersecting >= 0){
      float intersectionPoint1 = (-b - sqrt(isIntersecting)) / (2 * a);
      float intersectionPoint2 = (-b + sqrt(isIntersecting)) / (2 * a);
      Vec3 rayDirection = pixelPos - rayOrigin;
      float pointOfClosestIntersection;
      if(intersectionPoint1 < 0) pointOfClosestIntersection = intersectionPoint2;
      else if(intersectionPoint2 < 0) pointOfClosestIntersection = intersectionPoint1;
      else if(intersectionPoint1 < intersectionPoint2) pointOfClosestIntersection = intersectionPoint1;
      else pointOfClosestIntersection = intersectionPoint2;
      return rayOrigin + rayDirection * pointOfClosestIntersection;
    }
    return {};
  }
  
  Vec3 computeLighting(const Vec3& rayOrigin, const Vec3& lightPos, const Vec3& intersectionPoint, const Vec3& normal, const Vec3& specularColor, const Vec3& diffuseColor, const Vec3& ambientColor) {
    const Vec3 viewDir  = Vec3::normalize(rayOrigin-intersectionPoint);
    const Vec3 lightDir = Vec3::normalize(lightPos-intersectionPoint);
    const Vec3 reflectedDir = normal * 2.0f * Vec3::dot(normal, lightDir) - lightDir;
    
    const Vec3 specular = specularColor * pow(std::max(0.0f,Vec3::dot(reflectedDir, viewDir)),16.0f);
    const Vec3 diffuse  = diffuseColor * std::max(0.0f,Vec3::dot(normal, lightDir));
    const Vec3 ambient  = ambientColor;
    return specular + diffuse + ambient;
  }

  void updateImage() {
    const Vec3 deltaX = (topRightCorner-topLeftCorner) / float(image.width);
    const Vec3 deltaY = (topRightCorner-bottomRightCorner) / float(image.height);
    for(uint32_t y = 0;y< uint32_t(image.height);++y) {
      for(uint32_t x = 0;x< uint32_t(image.width);++x) {
        const Vec3 pixelPos = bottomLeftCorner + deltaX*float(x) + deltaY*float(y);
        Vec3 color = Vec3{0.0f, 0.0f, 0.0f};
        for(Light currentLight : lights) {
          const std::optional<Vec3> intersection = raySphereIntersect(sphereCenter, radius, rayOrigin, pixelPos);
          if(intersection) {
            const Vec3 normal = (*intersection - sphereCenter) / radius;
            const float ambiantRatio = 0.001f;
            const float specularRatio = 0.0f;
            const Vec3 ambiantLightColor = currentLight.diffuseColor * ambiantRatio;
            const Vec3 specularReflectionColor = Vec3{1.0f,1.0f,1.0f} * specularRatio;
            color = color + computeLighting(rayOrigin, {convertPolarToCatesian(currentLight.position), lightRingHeight}, *intersection, normal, specularReflectionColor, currentLight.diffuseColor, ambiantLightColor);
          }
        };
        image.setNormalizedValue(x,y,0,color.r);
        image.setNormalizedValue(x,y,1,color.g);
        image.setNormalizedValue(x,y,2,color.b);
        image.setValue(x,y,3,255);
      }
    }
  }

  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glClearColor(0,0,0,0));

    const struct Light keyLight = {convertCatesianToPolar(Vec2{0.0f,4.0f}), Vec3{1.0f, 1.0f, 1.0f}};
    const struct Light fillLight = {{keyLight.position.radius, 0.0f}, convertHtmlColorToOpenGlColor("#5BCFFA")};
    const struct Light rimLight = {{keyLight.position.radius, 0.0f}, convertHtmlColorToOpenGlColor("#F5A9B8")};
    lights = {keyLight, fillLight, rimLight};
    for(size_t currentLight = 0; currentLight < lights.size(); currentLight++) {
      lights[currentLight].position.angle += 2.0f * M_PI / lights.size() * currentLight;
    }
    updateImage();
  }

  const float rotationSpeed = 1.0f;
  virtual void animate(double animationTime) override {
    float angle = animationTime * rotationSpeed;
    rotateLightRing(angle);
    updateImage();
  }

  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    drawImage(image);
  }


  //Not being called somehow
  virtual void sizeCallback(GLFWwindow* window, int width, int height) {
    image.width = width;
    image.height = height;
    std::cout << "sizeCallback called, width: " << width << '\t' << " height: " << height << '\n';
  }

} myApp;

#ifdef _WIN32s
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