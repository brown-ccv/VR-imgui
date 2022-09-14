# VR-imgui
A C++ library to render Dear-Imgui on 3D space and used on VR applications.

## Depedencies

- [Cmake](https://cmake.org/) > 3.15
- OpenGL
- Glew

## Supported O.S

- Windows 10 and 11
- Linux
- Mac-OS

## Supported Compilers

- GCC
- Visual Studio.
- Clang 11

## How to build

This library support [Cmake](https://cmake.org/). The source code contains [im-gui v8.7](https://github.com/ocornut/imgui) embedded in the project.

1. Open cmake-gui and configure the project. You might need to link the references to Glew include folder and library paths.
2. Generate the project
3. (Optional) Open the project in visual studio to build and compile the library.

## Example:

      #include <GL/gl.h>
      #include <gl/GLU.h>
      #include "VRMenu.h"
      #include "VRMenuHandler.h"
      bool is_2d = false;
      
      void menu_callback()
      {
      
        if (m_is2d)
        {
          ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
          ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Once);
        }

        //...do other stuff like ImGui::NewFrame();

        if (ImGui::Begin("dummy window"))
        {
          
          ImGui::InputText("###exampleInputText", &_inputText);
          ImGui::End();
        }
      }
      
      
      int main(void)
      {
          GLFWwindow* window;
          VRMenuHandler * menus = new VRMenuHandler(m_is2d);
		     
          
          glfwSetErrorCallback(error_callback);

          if (!glfwInit())
              exit(EXIT_FAILURE);
          
          menus->addNewMenu(std::bind(&menu_callback, this), 1024, 1024, 1, 1);
          glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
          glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

          window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
          if (!window)
          {
              glfwTerminate();
              exit(EXIT_FAILURE);
          }

          while (!glfwWindowShouldClose(window))
          {
              menus->drawMenu(window_w, window_h, framebuffer_w, framebuffer_h);
              glfwSwapBuffers(window);
              glfwPollEvents();
          }

          glfwDestroyWindow(window);

          glfwTerminate();
          exit(EXIT_SUCCESS);
      }


For a more complete example on on how to use the keyboard/mouse input events with the GUI you have to enable the cmake option `BUILD_WITH_EXAMPLE`. It uses [MinVR](https://github.com/MinVR/MinVR) a renderer engine.



