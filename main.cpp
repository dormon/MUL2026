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
  
  uniform float iTime;

  void main(){



    vCoord = vec2(gl_VertexID%2,gl_VertexID/2);
    
    mat4 R=mat4(1);
    R[0][0]=cos(iTime);
    R[1][1]=cos(iTime);
    R[0][1]=sin(iTime);
    R[1][0]=-sin(iTime);

    gl_Position = vec4(vCoord*1.9-0.95,0,1);

    vCoord.y = 1-vCoord.y;
  }

  ).";

  auto fsSrc = R".(
  #version 460
  in vec2 vCoord;
  out vec4 fColor;

  layout(binding=0)uniform sampler2D frame;

  uniform int filterOn;

  void main(){
  
    vec4 color;

    if(filterOn==1){
      vec2 uv = vCoord;


      vec4 color0 = texture(frame,uv);
      float intensity = dot(color0,vec4(0.3,0.6,0.1,0));
      //for(int i=0;i<300;++i){
      //  vec4 color0 = texture(frame,uv);
      //  float intensity = dot(color0,vec4(0.3,0.6,0.1,0));
      //  float a = radians(intensity*360);
      //  uv += vec2(cos(a),sin(a))*0.0001;
      //}

      color = vec4(sin(intensity*30.)*.5+.5);
      
    }else{
      color = texture(frame,vCoord);
    }

    fColor = color;
  }
  ).";

  auto vs = std::make_shared<Shader>(GL_VERTEX_SHADER  ,vsSrc);
  auto fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER,fsSrc);

  auto prg = std::make_shared<Program>(vs,fs);
  prg->setNonexistingUniformWarning(false);

    
  GLuint frameTexture;
  glCreateTextures(GL_TEXTURE_2D,1,&frameTexture);
  glTextureStorage2D(frameTexture,1,GL_RGB8,w,h);
  glTextureParameteri(frameTexture,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTextureParameteri(frameTexture,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTextureParameteri(frameTexture,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTextureParameteri(frameTexture,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


  bool running = true;
  bool playing = true;
  bool filterOn = true;
  float iTime = 0.;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_EVENT_QUIT)running = false;
      if(event.type == SDL_EVENT_KEY_DOWN){
        if(event.key.key == SDLK_SPACE)playing = !playing;
        if(event.key.key == SDLK_F    )filterOn = !filterOn;
      }
    }

    if(playing){
      video.read(bgrFrame);
      glTextureSubImage2D(frameTexture,0,0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,bgrFrame.data);
    }


    //rendering
    glClearColor(0.5,0.5,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);
  
    glBindTextureUnit(0,frameTexture);

    iTime += 0.01;
    prg->use();
    prg->set1f("iTime",iTime);
    prg->set1i("filterOn",filterOn);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);


    SDL_GL_SwapWindow(window);

  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
