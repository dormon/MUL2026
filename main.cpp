#include<iostream>

#include<SDL3/SDL.h>
#include<geGL/geGL.h>

#include<geGL/StaticCalls.h>
using namespace ge::gl;

int main(){
  auto window = SDL_CreateWindow("MUL2026",1024,768,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init();

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
    fColor = texture(frame,vCoord);
  }
  ).";

  auto vs = std::make_shared<Shader>(GL_VERTEX_SHADER  ,vsSrc);
  auto fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER,fsSrc);

  auto prg = std::make_shared<Program>(vs,fs);

  float data[] = {
    0,0,0,1,
    1,0,1,1,
    1,0,1,1,
    0,0,0,1,
  };
    
  GLuint frameTexture;
  glCreateTextures(GL_TEXTURE_2D,1,&frameTexture);
  glTextureStorage2D(frameTexture,1,GL_RGBA32F,2,2);
  glTextureSubImage2D(frameTexture,0,0,0,2,2,GL_RGBA,GL_FLOAT,data);
  glTextureParameteri(frameTexture,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTextureParameteri(frameTexture,GL_TEXTURE_MAG_FILTER,GL_NEAREST);


  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_EVENT_QUIT)running = false;
  
    }
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
