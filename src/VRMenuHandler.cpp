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
///\file VRMenuHandler.cpp
///\author Benjamin Knorlein
///\date 10/1/2019

///\ Fixed bug on not 1:1 screen coordinates to pixel display mapping. 
///\author Camilo Diaz
///\date 8/27/2021

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glew.h>

#include "VRMenuHandler.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"

#include <iostream>

VRMenuHandler::VRMenuHandler(bool is2D) :m_active_menu(NULL), m_is2D(is2D), m_imgui2D_initialised(false), m_isHover(false), m_font_scale{ 1.0f }
{
	m_font_atlas = IM_NEW(ImFontAtlas)();

}

VRMenuHandler::~VRMenuHandler()
{
	//delete and clean menus
	for (auto menu : m_menus)
		delete menu;

	m_menus.clear();

	
	//shutdown opengl3 
	ImGui_ImplOpenGL3_Shutdown();

	if (m_imgui2D_initialised)
	{

		ImGui::DestroyContext();

	}

	//delete font atlas
	delete m_font_atlas;

}

void VRMenuHandler::renderToTexture()
{

	if (!m_is2D) {
		//render menus to texture
		for (auto& menu : m_menus)
			menu->renderToTexture();
	}
}

void VRMenuHandler::drawMenu(int window_width, int window_height, int framebuffer_width, int framebuffer_height)
{
	if (!m_is2D) {
		//draw menus
		for (auto& menu : m_menus)
			menu->drawMenu();
	}
	else if (!m_menus.empty())
	{
		if (!VRMenu::m_glew_initialised)
		{
			glewInit();
		}

		if (!m_imgui2D_initialised) {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext(m_font_atlas);
			ImGuiIO& io = ImGui::GetIO();
			ImGui_ImplOpenGL3_Init("#version 330");
			io.FontGlobalScale = m_font_scale;
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			m_imgui2D_initialised = true;
		}
		ImGuiIO& io = ImGui::GetIO();

		glViewport(0, 0, (GLsizei)framebuffer_width, (GLsizei)framebuffer_height);

		io.DisplaySize = ImVec2((float)window_width, (float)window_height);
		if (window_width > 0 && window_height > 0)
			io.DisplayFramebufferScale = ImVec2((float)framebuffer_width / window_width, (float)framebuffer_height / window_height);


		ImGui_ImplOpenGL3_NewFrame();
		m_menus[0]->newFrame(false);
		ImGui::NewFrame(); //Here all the events are interpreted

		for (auto& menu : m_menus)
			menu->call_callback();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetCurrentContext()->HoveredWindow != NULL)
			m_isHover = true;
		else
			m_isHover = false;
	}
}

void VRMenuHandler::setControllerPose(const glm::mat4& controllerpose, float max_distance)
{
	if (!m_is2D) {
		m_active_menu = NULL;
		float min_distance = max_distance;
		for (auto& menu : m_menus)
		{
			if (menu->isGrabbed())
			{
				m_active_menu = menu;
				break;
			}

			float dist = menu->menu_controller_distance(controllerpose);
			if (dist < min_distance)
			{
				min_distance = dist;
				m_active_menu = menu;
			}
		}

		for (auto& menu : m_menus)
			if (menu == m_active_menu) {
				menu->setControllerPose(controllerpose);
			}
			else {
				menu->getIO().MouseDrawCursor = false;
				//menu->getIO().MousePos = ImVec2(-1, -1);
			}
	}
}

void VRMenuHandler::setButtonClick(int button, int state)
{
	if (!m_is2D) {
		if (m_active_menu)
			m_active_menu->setButtonClick(button, state);
	}
	else if (m_imgui2D_initialised)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (state == 1) {
			io.MouseDown[button] = true;
		}
		else {
			io.MouseDown[button] = false;
		}
	}
}

void VRMenuHandler::setAnalogValue(float value)
{
	if (!m_is2D) {
		if (m_active_menu) {
			m_active_menu->setAnalogValue(value);
		}
	}
	else if (m_imgui2D_initialised)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += value * 0.1;
	}
}

void VRMenuHandler::setCursorPos(float x, float y)
{
	if (m_is2D && m_imgui2D_initialised) {
		ImGuiIO& io = ImGui::GetIO();
		//std::cout << "after " <<x << " " << y << std::endl;

		io.MousePos = ImVec2(x, y);
	}
}

VRMenu* VRMenuHandler::addNewMenu(std::function<void()> callback, int res_x, int res_y, float width, float height, float font_size)
{
	m_font_scale = font_size;
	VRMenu* menu = new VRMenu(res_x, res_y, width, height, m_font_atlas, m_is2D, font_size);
	menu->set_callback(callback);
	m_menus.push_back(menu);
	return menu;
}

void VRMenuHandler::deleteMenu(VRMenu* menu)
{
	std::vector<VRMenu*>::iterator position = std::find(m_menus.begin(), m_menus.end(), menu);
	if (position != m_menus.end()) // == myVector.end() means the element was not found
		m_menus.erase(position);
	delete menu;
}
