//  ----------------------------------
//  Copyright © 2015, Brown University, Providence, RI.
//  
//  All Rights Reserved
//   
//  Use of the software is provided under the terms of the GNU General Public License version 3 
//  as published by the Free Software Foundation at http://www.gnu.org/licenses/gpl-3.0.html, provided 
//  that this copyright notice appear in all copies and that the name of Brown University not be used in 
//  advertising or publicity pertaining to the use or distribution of the software without specific written 
//  prior permission from Brown University.
//  
//  See license.txt for further information.
//  
//  BROWN UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE WHICH IS 
//  PROVIDED “AS IS”, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
//  FOR ANY PARTICULAR PURPOSE.  IN NO EVENT SHALL BROWN UNIVERSITY BE LIABLE FOR ANY 
//  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR FOR ANY DAMAGES WHATSOEVER RESULTING 
//  FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
//  OTHER TORTIOUS ACTION, OR ANY OTHER LEGAL THEORY, ARISING OUT OF OR IN CONNECTION 
//  WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
//  ----------------------------------
//  
///\file VRMenu.cpp
///\author Benjamin Knorlein
///\date 9/26/2019

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

#include "VRMenu.h"

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

#include <iostream>
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <glm/gtc/type_ptr.hpp>

bool VRMenu::m_glew_initialised = false;

VRMenu::VRMenu(int res_x, int res_y, float width, float height, ImFontAtlas* font_atlas, bool is2D) : m_res_x(res_x), m_res_y(res_y), m_width(width), m_height(height), m_font_atlas(font_atlas), m_fbo_initialised(false), m_imgui_initialised(false), m_grabbing_active(false), m_is2D(is2D)
	, m_MSAA_buffers(4), m_font_scale(2.0), m_pose(glm::mat4(1)), m_pose_grab_relative(glm::mat4(1))
{
	if (m_height == 0)
	{
		m_height = m_width * m_res_y / m_res_x;
	}
	m_lastTime = std::chrono::steady_clock::now();
}

VRMenu::~VRMenu()
{
	if (!m_is2D) {
		if (m_fbo_initialised) {
			glDeleteRenderbuffers(1, &m_nDepthBufferId);
			if (m_MSAA_buffers > 1) glDeleteTextures(1, &m_nRenderTextureId);
			glDeleteFramebuffers(1, &m_nRenderFramebufferId);
			glDeleteTextures(1, &m_nRenderTextureId);
			if (m_MSAA_buffers > 1) glDeleteFramebuffers(1, &m_nResolveFramebufferId);
		}
	}
	
	if (m_imgui_initialised)
	{
		ImGui::DestroyContext(m_imgui_context);
	}

}

void VRMenu::initFBO()
{
	std::cerr << "Init fbo" << std::endl;
	glGenFramebuffers(1, &m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nRenderFramebufferId);

	glGenRenderbuffers(1, &m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId);
	if (m_MSAA_buffers > 1){
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, m_res_x, m_res_y);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_res_x, m_res_y);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthBufferId);

	if (m_MSAA_buffers > 1){
		glGenTextures(1, &m_nRenderTextureId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_nRenderTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, m_res_x, m_res_y, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_nRenderTextureId, 0);

		glGenFramebuffers(1, &m_nResolveFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_nResolveFramebufferId);
	}

	glGenTextures(1, &m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D, m_nRenderTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_res_x, m_res_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTextureId, 0);

	// check FBO status
	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//if (status != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	return false;
	//}

	m_fbo_initialised = true;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void VRMenu::initImgui()
{
	std::cerr << "Init imgui" << std::endl; 
	IMGUI_CHECKVERSION();
		
	if (m_font_atlas) //shared font atlas
		m_imgui_context = ImGui::CreateContext(m_font_atlas);
	else{
		// own font_atlas 
		m_imgui_context = ImGui::CreateContext();
	}

	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO &io = ImGui::GetIO();
	io.FontGlobalScale = m_font_scale;
	// Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init("#version 330");

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	m_imgui_initialised = true;
}

void VRMenu::renderToTexture()
{
	//init glew
	if (!m_glew_initialised)
	{
		std::cerr << "Init glew" << std::endl;
    HGLRC  v = wglGetCurrentContext();
    if (v == NULL)
    {
      
      
      //std::cout << "already exists " << std::endl;
    }
    glewInit();
    m_glew_initialised = true;
    
		//m_glew_initialised = true;
	}

	if (!m_imgui_initialised)
		initImgui();

	////first read the previous framebuffer
	GLint drawFboId = 0, readFboId = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

	////if not initialised init FBO
	if (!m_fbo_initialised)
		initFBO();

	////set FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_nRenderFramebufferId);
	GLboolean last_enable_multisample = glIsEnabled(GL_MULTISAMPLE);
	if (!last_enable_multisample && m_MSAA_buffers > 1) {
		glEnable(GL_MULTISAMPLE);
	}

	GLint last_viewport[4];
	glGetIntegerv(GL_VIEWPORT, last_viewport);
	glViewport(0, 0, m_res_x, m_res_y);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float) m_res_x, (float) m_res_y);

	//feed inputs to imgui
	ImGui_ImplOpenGL3_NewFrame();
	newFrame();
	ImGui::NewFrame(); //Here all the events are interpreted

	//run Menu callback
	m_callback();
	
	//we need to setup the root window for full scale and disable scaling here
	//We search for the first window with no parentwindow and set the size and position as well as disable scalin
	for (auto &window : m_imgui_context->Windows){
		if (window->ParentWindow == NULL){
			ImGui::SetWindowPos(window, ImVec2(0, 0));
			ImGui::SetWindowSize(window, ImVec2(m_res_x, m_res_y));
			window->Flags = window->Flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
		}
	}

	ImGui::Render();
	//we need to setup the root window for full scale and disable scaling here
	//We search for the first window with no parentwindow and set the size and position as well as disable scalin

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (m_MSAA_buffers > 1) {
		if (!last_enable_multisample) glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_nResolveFramebufferId);
		glBlitFramebuffer(0, 0, m_res_x, m_res_y, 0, 0, m_res_x, m_res_y,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);
	}

  /*glBindFramebuffer(GL_READ_FRAMEBUFFER, m_nResolveFramebufferId);
  int* buffer = new int[m_res_x * m_res_y * 3];
  glReadPixels(0, 0, m_res_x, m_res_y, GL_BGR, GL_UNSIGNED_BYTE, buffer);
  FILE   *out = fopen("test2.tga", "w");
  short  TGAhead[] = { 0, 2, 0, 0, 0, 0, m_res_x, m_res_y, 24 };
  fwrite(&TGAhead, sizeof(TGAhead), 1, out);
  fwrite(buffer, 3 * m_res_x * m_res_y, 1, out);
  fclose(out);*/


	////set back to previous FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId);

	glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
}

void VRMenu::drawMenu()
{
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(m_pose));

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_nRenderTextureId);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-m_width / 2.0f, -m_height / 2.0f, 0.0f);
	glTexCoord2f(0, 1);
	glVertex3f(-m_width / 2.0f, m_height / 2.0f, 0.0f);
	glTexCoord2f(1, 1);
	glVertex3f(m_width / 2.0f, m_height / 2.0f, 0.0f);
	glTexCoord2f(1, 0);
	glVertex3f(m_width / 2.0f, -m_height / 2.0f, 0.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,0);
	glPopMatrix();

}

void VRMenu::setMenuPose(const glm::mat4 pose)
{
	m_pose = pose;
}

ImGuiIO& VRMenu::getIO()
{
	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO& io = ImGui::GetIO();
	return io;
}

float VRMenu::menu_controller_distance(const glm::mat4 &controllerpose, float max_distance)
{
	glm::vec4 pos = glm::inverse(m_pose) * controllerpose * glm::vec4(0, 0, 0, 1);
	glm::vec4 dir = glm::inverse(m_pose) * controllerpose * glm::vec4(0, 0, -1, 0);

	float distance = -pos.z / dir.z;
	
	if (distance >= max_distance)
		return max_distance;

	glm::vec4 m_interactionPoint = pos + dir * distance;
	m_interactionPoint.x = m_interactionPoint.x / m_width * m_res_x + (0.5f * m_res_x);
	m_interactionPoint.y = -m_interactionPoint.y / m_height * m_res_y + (0.5f * m_res_y);

	if (m_interactionPoint.x > 0 && m_interactionPoint.y > 0
		&& m_interactionPoint.x < m_res_x && m_interactionPoint.y < m_res_y)
	{
		return distance;
	} 
	else
	{
		return max_distance;
	}
}

void VRMenu::setControllerPose(const glm::mat4 &controllerpose)
{
	if (!m_imgui_initialised)
		return;

	if (m_imgui_context->MovingWindow != NULL &&  m_imgui_context->MovingWindow->ParentWindow == NULL &&
		std::string(m_imgui_context->MovingWindow->Name) != std::string("Debug##Default"))
	{
		if (!m_grabbing_active){
			m_pose_grab_relative = glm::inverse(m_pose_grab_relative) * m_pose;
			m_grabbing_active = true;
		}
	} else
	{
		m_grabbing_active = false;
	}

	/*for (auto &window : m_imgui_context->Windows){
		if (window->ParentWindow == NULL && std::string(window->Name) != std::string("Debug##Default")){
			ImGuiWindow * movingParent = m_imgui_context->MovingWindow;
			
			if (!m_grabbing_active && movingParent == window){
				m_pose_grab_relative = glm::inverse(m_pose_grab_relative) * m_pose;
				m_grabbing_active = true;
			}
			else if (m_grabbing_active && movingParent != window)
			{
				m_grabbing_active = false;
			}
		}
	}*/

	if (!m_grabbing_active){
		m_pose_grab_relative = controllerpose;
	}
	else 
	{
		m_pose = controllerpose * m_pose_grab_relative;
		return;
	}

	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO& io = ImGui::GetIO();

	glm::vec4 pos = glm::inverse(m_pose) * controllerpose * glm::vec4(0, 0, 0, 1);
	glm::vec4 dir = glm::inverse(m_pose) * controllerpose * glm::vec4(0, 0, -1, 0);

	float distance = -pos.z / dir.z;
	glm::vec4 m_interactionPoint = pos + dir * distance;

	m_interactionPoint.x = m_interactionPoint.x / m_width * m_res_x + (0.5f * m_res_x);
	m_interactionPoint.y = -m_interactionPoint.y / m_height * m_res_y + (0.5f * m_res_y);

	io.MousePos = ImVec2(m_interactionPoint.x, m_interactionPoint.y);
	if (io.MousePos.x > 0 && io.MousePos.y > 0
		&& io.MousePos.x < m_res_x && io.MousePos.y < m_res_y)
	{
		io.MouseDrawCursor = true;
	} 
	else
	{
		io.MouseDrawCursor = false;
	}
}

void VRMenu::setButtonClick(int button, int state){
	if (!m_imgui_initialised)
		return;
	
	if (button > 5)
		return;

	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO& io = ImGui::GetIO();
	if (state == 1){
		io.MouseDown[button] = true;
	}
	else{
		io.MouseDown[button] = false;
	}
}

void VRMenu::setAnalogValue(float value)
{
	if (!m_imgui_initialised)
		return;

	ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += value*0.1;
}

void VRMenu::set_callback(std::function<void()> callback)
{
	m_callback = callback;
}

void  VRMenu::call_callback()
{
	m_callback();
}

void VRMenu::newFrame(bool setContext)
{
	std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration<double>(nowTime - m_lastTime);
	
	if(setContext)
		ImGui::SetCurrentContext(m_imgui_context);
	ImGuiIO& io = ImGui::GetIO();
	float delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_span).count() / 1000.0f;
	if (delta_time <= 0)
		delta_time = 1;

	io.DeltaTime = delta_time;
	m_lastTime = nowTime;
}
