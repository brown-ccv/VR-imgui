// MinVR header
#include <api/MinVR.h>
#include "VRMenu.h"
#include <glm/gtc/type_ptr.inl>
#include "VRMenuHandler.h"
//#include <imfilebrowser.h>
#include "ImGuiFileBrowser.h"

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif


// OpenGL platform-specific headers
#ifdef _MSC_VER
#define NOMINMAX
//#include <windows.h>
#include <GL/gl.h>
#include <gl/GLU.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "imgui_stdlib.h"

//#include "main/VREventInternal.h"

using namespace MinVR;
// ImGui::FileBrowser fileDialog;
imgui_addons::ImGuiFileBrowser file_dialog;

class MyVRApp : public VRApp
{
public:
	void menu_callback()
	{
		// create a file browser instance
		if (m_is2d)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Once);
		}

		//...do other stuff like ImGui::NewFrame();

		if (ImGui::Begin("dummy window"))
		{
			// open file dialog when user clicks this button
			if (ImGui::Button("open file dialog"))
			{
				// (optional) set browser properties
				/*	fileDialog.SetTitle("title");
					fileDialog.SetTypeFilters({ ".h", ".cpp" });
					fileDialog.Open();*/
				d_open = true;
			}
			ImGui::InputText("###exampleInputText", &_inputText);
			ImGui::End();

			/*	fileDialog.Display();

				if (fileDialog.HasSelected())
				{
					std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
					fileDialog.ClearSelected();
				}*/
			if (d_open)
			{
				ImGui::OpenPopup("Open File");
				d_open = false;
			}

			if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
			{
				std::cout << file_dialog.selected_fn << std::endl;	 // The name of the selected file or directory in case of Select Directory dialog mode
				std::cout << file_dialog.selected_path << std::endl; // The absolute path to the selected file
			}
			if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
			{
				std::cout << file_dialog.selected_fn << std::endl;	 // The name of the selected file or directory in case of Select Directory dialog mode
				std::cout << file_dialog.selected_path << std::endl; // The absolute path to the selected file
				std::cout << file_dialog.ext << std::endl;			 // Access ext separately (For SAVE mode)
																	 // Do writing of files based on extension here
			}
			////...do other stuff like ImGui::Render();
		}
	}

	void menu_callback2()
	{
		if (m_is2d)
		{
			ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(400, 40), ImGuiCond_Once);
		}
		ImGui::ShowDemoWindow();
	}

	MyVRApp(int argc, char **argv, const std::string &configFile) : VRApp(argc, argv), m_is2d(false)
	{
		int argc_int = this->getLeftoverArgc();
		char **argv_int = this->getLeftoverArgv();

		if (argc_int >= 2)
		{
			for (int i = 1; i < argc_int; i++)
			{
				if (std::string(argv_int[i]) == std::string("use2DUI"))
				{
					m_is2d = true;
				}
			}
		}

		std::cout << "m_is2d is " << m_is2d << std::endl;
		menus = new VRMenuHandler(m_is2d);
		menus->addNewMenu(std::bind(&MyVRApp::menu_callback, this), 1024, 1024, 1, 1);

		VRMenu *menu = menus->addNewMenu(std::bind(&MyVRApp::menu_callback2, this), 1024, 1024, 1, 1);
		menu->setMenuPose(glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -2.0f, 1.0f));
	}

	virtual ~MyVRApp()
	{
		std::cerr << "Delete" << std::endl;
	}

	void onGenericEvent(const VRDataIndex &index)
	{
		if (index.getName() == "WindowClose")
		{
			shutdown();
		}
	}

	virtual void onCursorMove(const VRCursorEvent &event)
	{
		if (event.getName() == "Mouse_Move" && menus != NULL)
		{
			// std::cout << "before " <<event.getPos()[0] << " " <<  event.getPos()[1] << std::endl;
			menus->setCursorPos(event.getPos()[0], event.getPos()[1]);
		}
	}

	virtual void onAnalogChange(const VRAnalogEvent &event)
	{
		if (menus != NULL && menus->windowIsActive() && event.getName() == "HTC_Controller_Right_TrackPad0_Y" || event.getName() == "HTC_Controller_1_TrackPad0_Y")
			menus->setAnalogValue(event.getValue());
	}

	virtual void onTrackerMove(const VRTrackerEvent &event)
	{
		if (event.getName() == "HTC_Controller_Right_Move" || event.getName() == "HTC_Controller_1_Move")
		{
			menus->setControllerPose(glm::make_mat4(event.getTransform()));
		}
	}

	virtual void onButtonDown(const VRButtonEvent &event)
	{
		if (menus != NULL && menus->windowIsActive())
		{
			if (event.getName() == "HTC_Controller_Right_Axis1Button_Down" || event.getName() == "HTC_Controller_1_Axis1Button_Down")
			{
				// left click
				menus->setButtonClick(0, 1);
			}
			else if (event.getName() == "HTC_Controller_Right_GripButton_Down" || event.getName() == "HTC_Controller_1_GripButton_Down")
			{
				// middle click
				menus->setButtonClick(2, 1);
			}
			// else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
			else if (event.getName() == "HTC_Controller_Right_Axis0Button_Down" || event.getName() == "HTC_Controller_1_Axis0Button_Down")
			{
				// right click
				menus->setButtonClick(1, 1);
			}
			else if (event.getName() == "MouseBtnLeft_Down")
			{
				menus->setButtonClick(0, 1);
			}
			else if (event.getName() == "MouseBtnRight_Down")
			{
				menus->setButtonClick(1, 1);
			}
		}
	}

	virtual void onButtonUp(const VRButtonEvent &event)
	{
		if (menus != NULL)
		{
			if (event.getName() == "HTC_Controller_Right_Axis1Button_Up" || event.getName() == "HTC_Controller_1_Axis1Button_Up")
			{
				// left click
				menus->setButtonClick(0, 0);
			}
			else if (event.getName() == "HTC_Controller_Right_GripButton_Up" || event.getName() == "HTC_Controller_1_GripButton_Up")
			{
				// middle click
				menus->setButtonClick(2, 0);
			}
			// else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
			else if (event.getName() == "HTC_Controller_Right_Axis0Button_Up" || event.getName() == "HTC_Controller_1_Axis0Button_Up")
			{
				// right click
				menus->setButtonClick(1, 0);
			}
			if (event.getName() == "MouseBtnLeft_Up")
			{
				menus->setButtonClick(0, 0);
			}
			else if (event.getName() == "MouseBtnRight_Up")
			{
				menus->setButtonClick(1, 0);
			}
			else if (menus->windowIsActive() && event.getName() == "MouseBtnMiddle_ScrollUp")
			{
				menus->setAnalogValue(10);
			}

			if (menus->windowIsActive() && event.getName() == "MouseBtnMiddle_ScrollDown")
			{
				menus->setAnalogValue(-10);
			}
		}
	}

	virtual void onRenderGraphicsContext(const VRGraphicsState &renderState)
	{
		if (renderState.isInitialRenderCall()) {
#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
#endif
			// Init GL
			glEnable(GL_DEPTH_TEST);
			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);
			glClearColor(0, 0, 0, 1);


			menus->initGL();
		}
		menus->renderToTexture();
	}

	virtual void onRenderGraphicsScene(const VRGraphicsState &renderState)
	{

		glClearColor(0.0, 0.0, 0.0, 1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 projectionMatrix = glm::make_mat4(renderState.getProjectionMatrix());
		glm::mat4 viewMatrix = glm::make_mat4(renderState.getViewMatrix());
		
		

		int window_w = renderState.index().getValue("WindowWidth");
		int window_h = renderState.index().getValue("WindowHeight");
		int framebuffer_w = renderState.index().getValue("FramebufferWidth");
		int framebuffer_h = renderState.index().getValue("FramebufferHeight");

		if (m_is2d)
		{
			menus->drawMenu2D( window_w, window_h, framebuffer_w, framebuffer_h);
		}
		else
		{
			menus->drawMenu3D(projectionMatrix, viewMatrix);
		}
		

		
	}

protected:
	VRMenuHandler *menus;
	std::string _inputText = "this is a test";
	bool m_is2d;
	bool d_open = false;
};

int main(int argc, char **argv)
{
	MyVRApp app(argc, argv, argv[2]);
	// This starts the rendering/input processing loop
	app.run();

	return 0;
}
