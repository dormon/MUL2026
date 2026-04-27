#include "geGL/DebugMessage.h"
#include "opencv2/videoio.hpp"
#include<iostream>

#include<SDL3/SDL.h>
#include<geGL/geGL.h>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>

#include<geGL/StaticCalls.h>
using namespace ge::gl;

int main(){
  
  cv::VideoCapture video;
  cv::Mat bgrFrame;

  video.open("../mtm.webm");

  auto w = video.get(cv::CAP_PROP_FRAME_WIDTH );
  auto h = video.get(cv::CAP_PROP_FRAME_HEIGHT);

  auto window = SDL_CreateWindow("MUL2026",w,h,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init();

  ge::gl::setHighDebugMessage();

  auto vsSrc = R".(
  #version 460

  out vec2 vCoord;
  


  void main(){
    vCoord = vec2(gl_VertexID%2,gl_VertexID/2);
    gl_Position = vec4(vCoord*1.9-0.95,0,1);
  }

  ).";

  auto fsSrc = R".(
  #version 460
  in vec2 vCoord;
  out vec4 fColor;

  layout(binding=0)uniform sampler2D frame;

  void main(){
    fColor = texture(frame,vec2(vCoord.x,1-vCoord.y));
  }
  ).";

  auto vs = std::make_shared<Shader>(GL_VERTEX_SHADER  ,vsSrc);
  auto fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER,fsSrc);

  auto prg = std::make_shared<Program>(vs,fs);

    
  GLuint frameTexture;
  glCreateTextures(GL_TEXTURE_2D,1,&frameTexture);
  glTextureStorage2D(frameTexture,1,GL_RGB8,w,h);
  glTextureParameteri(frameTexture,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTextureParameteri(frameTexture,GL_TEXTURE_MAG_FILTER,GL_NEAREST);


  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_EVENT_QUIT)running = false;
  
    }

    video.read(bgrFrame);
    glTextureSubImage2D(frameTexture,0,0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,bgrFrame.data);


    //rendering
    glClearColor(0.5,0.5,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);
  
    glBindTextureUnit(0,frameTexture);


    prg->use();
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);


    SDL_GL_SwapWindow(window);

  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
