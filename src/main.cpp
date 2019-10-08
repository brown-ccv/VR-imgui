// MinVR header
#include <api/MinVR.h>
#include "VRMenu.h"
#include <glm/gtc/type_ptr.inl>
#include "VRMenuHandler.h"

// OpenGL platform-specific headers
#ifdef _MSC_VER
#define NOMINMAX
#include <windows.h>
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


//#include "main/VREventInternal.h"

using namespace MinVR;

class MyVRApp : public VRApp
{
public:
	void menu_callback()
	{
		//ImGui::ShowDemoWindow();
		ImGui::Begin("Test VR Window");

		ImGui::End();
	}

	void menu_callback2()
	{
		ImGui::ShowDemoWindow();
	}

	MyVRApp(int argc, char** argv, const std::string& configFile) : VRApp(argc, argv)
	{
		menus = new VRMenuHandler();
		menus->addNewMenu(std::bind(&MyVRApp::menu_callback, this), 1024, 1024, 1, 1);
		
		VRMenu* menu = menus->addNewMenu(std::bind(&MyVRApp::menu_callback2, this), 1024, 1024, 1, 1);
		menu->setMenuPose(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, 2, 0, 1));
	}

	virtual ~MyVRApp()
	{
		std::cerr << "Delete" << std::endl;
	}

	virtual void onAnalogChange(const VRAnalogEvent &event) {
		if (event.getName() == "HTC_Controller_Right_TrackPad0_Y" || event.getName() == "HTC_Controller_1_TrackPad0_Y")
			menus->setAnalogValue(event.getValue());
	}

	virtual void onTrackerMove(const VRTrackerEvent &event) {
		if (event.getName() == "HTC_Controller_Right_Move" || event.getName() == "HTC_Controller_1_Move") {
			menus->setControllerPose(glm::make_mat4(event.getTransform()));
		}
	}

	virtual void onButtonDown(const VRButtonEvent &event)
	{
		if (event.getName() == "HTC_Controller_Right_Axis1Button_Down" || event.getName() == "HTC_Controller_1_Axis1Button_Down")
		{
			//left click
			menus->setButtonClick(0, 1);
		}
		else if (event.getName() == "HTC_Controller_Right_GripButton_Down" || event.getName() == "HTC_Controller_1_GripButton_Down")
		{
			//middle click
			menus->setButtonClick(2, 1);
		}
		//else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
		else if (event.getName() == "HTC_Controller_Right_Axis0Button_Down" || event.getName() == "HTC_Controller_1_Axis0Button_Down")
		{
			//right click
			menus->setButtonClick(1, 1);
		}
	}

	virtual void onButtonUp(const VRButtonEvent &event)
	{
		if (event.getName() == "HTC_Controller_Right_Axis1Button_Up" || event.getName() == "HTC_Controller_1_Axis1Button_Up")
		{
			//left click
			menus->setButtonClick(0, 0);

		}
		else if (event.getName() == "HTC_Controller_Right_GripButton_Up" || event.getName() == "HTC_Controller_1_GripButton_Up")
		{
			//middle click
			menus->setButtonClick(2, 0);
		}
		//else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
		else if (event.getName() == "HTC_Controller_Right_Axis0Button_Up" || event.getName() == "HTC_Controller_1_Axis0Button_Up")
		{
			//right click
			menus->setButtonClick(1, 0);
		}
	}

	virtual void onRenderGraphicsContext(const VRGraphicsState &renderState)
	{
		menus->renderToTexture();
	}

	virtual void onRenderGraphicsScene(const VRGraphicsState &renderState) {
		
		glClearColor(0.0, 0.0, 0.0, 1.0);
		//std::cerr << "On Render" << std::endl;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_NORMALIZE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(renderState.getProjectionMatrix());

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(renderState.getViewMatrix());

		menus->drawMenu();

		glFlush();
	}
	

protected:
	VRMenuHandler *menus;
};


int main(int argc, char **argv)
{
	MyVRApp app(argc, argv, argv[2]);
	// This starts the rendering/input processing loop
	app.run();

	return 0;
}

