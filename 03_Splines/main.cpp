#include <iostream>
#include <GLApp.h>
#include <Vec2.h>
#include <Vec4.h>
#include <Mat4.h>

class MyGLApp : public GLApp {
public:
  double sa = 0;
  double ca = 0;
  const size_t maxLineSegments = 100;

  MyGLApp() : GLApp{1024,1024,1,"Spline Demo"} {}
  
  virtual void init() override {
    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    setBackground(1, 1, 1, 1);
  }
  
  virtual void animate(double animationTime) override {
    sa = sin(animationTime);
    ca = cos(animationTime);
  }

  // TODO:
      // complete the function drawPolySegment
      // this function takes as argument the
      // geometry matrix of the polygon method
      // i.e. hermite, bezier, or b-spline
      // and draws the polygonal curve as a
      // line strip, the curve is given as
      // five paramters, i.e. the four control
      // points (or, in case of the hermite
      // curve two points and two derivative
      // vectors), the geometry matrix, and the
      // color of the curve. The result should
      // be written into the the vector curve
      // the format is x,y,z,r,g,b,a for each
      // point along the line
      // The result will be three curves, a
      // Hermite curve on the top, a Bezier
      // curve in the middle and a B-Spline
      // at the bottom

  bool mat4equal(const Mat4& m1, const Mat4& m2) {
    for(size_t i = 0; i < 4*4; i++) {
      if(m1[i] != m2[i]) {
        return false;
      }
    }
    return true;
  }

  const size_t xOffset = 0;
  const size_t yOffset = 1;
  const size_t zOffset = 2;
  const size_t redOffset = 3;
  const size_t greenOffset = 4;
  const size_t blueOffset = 5;
  const size_t alphaOffset = 6;
  const size_t pointSize = 7;

  void drawPolySegment(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Mat4& g, const Vec4& color) {
    std::vector<float> curve((maxLineSegments+1)*7);

    const Mat4 hermitBaseMatrix = Mat4{
      1, 0, 0, 0,
      0, 0, 1, 0,
     -3, 3,-2,-1,
      2,-2, 1, 1
    };

    if(mat4equal(g, hermitBaseMatrix)) {
      //Matrix solution did not work. Gonna have to look into that...
      for (size_t i = 0; i<=maxLineSegments; ++i) {
        const float t = float(i)/float(maxLineSegments);

        const float t2 = t * t;
        const float t3 = t2 * t;
        const float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
        const float h2 = -2.0f * t3 + 3.0f * t2;
        const float h3 = t3 - 2.0f * t2 + t;
        const float h4 = t3 - t2;
        const float x = h1 * p0.x + h2 * p1.x + h3 * p2.x + h4 * p3.x;
        const float y = h1 * p0.y + h2 * p1.y + h3 * p2.y + h4 * p3.y;

        curve[i*pointSize+xOffset] = x;
        curve[i*pointSize+yOffset] = y;
        curve[i*pointSize+zOffset] = 1.0f;
        curve[i*pointSize+redOffset] = color.r;
        curve[i*pointSize+greenOffset] = color.g;
        curve[i*pointSize+blueOffset] = color.b;
        curve[i*pointSize+alphaOffset] = 1.0f;
      }
    }

    drawLines(curve, LineDrawType::STRIP, 3);
  }
 
  void drawHermiteSegment(const Vec2& p0, const Vec2& p1, const Vec2& m0, const Vec2& m1, const Vec4& color) {
    Mat4 g{
      1, 0, 0, 0,
      0, 0, 1, 0,
     -3, 3,-2,-1,
      2,-2, 1, 1
    };
    drawPolySegment(p0,p1,m0,m1,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p0.x+m0.x,p0.y+m0.y,0,0,0,1,1,
               p1.x+m1.x,p1.y+m1.y,0,0,0,1,1,
               p1.x,p1.y,0,1,0,0,1}, 20, true);
  }
  
  void drawBezierSegmentDeCasteljau(const Vec2& p0, const Vec2& p1,
                                    const Vec2& p2, const Vec2& p3,
                                    const Vec4& color) {
    // TODO SOLUTION 2:
  }

  void drawBezierSegment(const Vec2& p0, const Vec2& p1, const Vec2& p2,
                         const Vec2& p3, const Vec4& color) {
    Mat4 g{
      1, 0, 0, 0,
     -3, 3, 0, 0,
      3,-6, 3, 0,
     -1, 3,-3, 1
    };
    //drawPolySegment(p0,p1,p2,p3,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p1.x,p1.y,0,0,0,1,1,
               p2.x,p2.y,0,0,0,1,1,
               p3.x,p3.y,0,1,0,0,1}, 20, true);
  }
  
  void drawBSplineSegment(const Vec2& p0, const Vec2& p1, const Vec2& p2,
                          const Vec2& p3, const Vec4& color) {
    Mat4 g{
      1/6.0f, 4/6.0f, 1/6.0f, 0/6.0f,
     -3/6.0f, 0/6.0f, 3/6.0f, 0/6.0f,
      3/6.0f,-6/6.0f, 3/6.0f, 0/6.0f,
     -1/6.0f, 3/6.0f,-3/6.0f, 1/6.0f
    };
    //drawPolySegment(p0,p1,p2,p3,g,color);
    drawPoints({p0.x,p0.y,0,1,0,0,1,
               p1.x,p1.y,0,0,0,1,1,
               p2.x,p2.y,0,0,0,1,1,
               p3.x,p3.y,0,1,0,0,1}, 20, true);
  }
  
  virtual void draw() override {

    {
      setDrawTransform(Mat4::translation(0.0f,0.7f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 m0{float(sa)*0.2f,float(ca)*0.2f};
      const Vec2 m1{0.0f,-0.2f};
      const Vec2 p1{0.5f,0.0f};
      drawHermiteSegment(p0,p1,m0,m1,{0.0f,0.0f,0.0f,1.0f});
    }

    {
      setDrawTransform(Mat4::translation(0.0f,0.0f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBezierSegment(p0,p1,p2,p3,{0.0f,0.0f,0.0f,1.0f});
    }
   
    {
      setDrawTransform(Mat4::translation(0.0f,-0.2f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBezierSegmentDeCasteljau(p0,p1,p2,p3,{0.0f,0.0f,0.0f,1.0f});
    }

    {
      setDrawTransform(Mat4::translation(0.0f,-0.7f,0.0f));
      const Vec2 p0{-0.5f,0.0f};
      const Vec2 p1{float(sa)*0.2f-0.5f,float(ca)*0.2f};
      const Vec2 p2{0.5f,0.2f};
      const Vec2 p3{0.5f,0.0f};
      drawBSplineSegment(p0,p0,p0,p1,{1.0f,0.0f,0.0f,1.0f});
      drawBSplineSegment(p0,p0,p1,p2,{0.0f,1.0f,0.0f,1.0f});
      drawBSplineSegment(p0,p1,p2,p3,{0.0f,0.0f,1.0f,1.0f});
      drawBSplineSegment(p1,p2,p3,p3,{0.0f,1.0f,1.0f,1.0f});
      drawBSplineSegment(p2,p3,p3,p3,{1.0f,0.0f,1.0f,1.0f});
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
