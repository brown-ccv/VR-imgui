﻿//  ----------------------------------
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

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glew.h>

#include "VRMenuHandler.h"
#include "imgui_impl_opengl3.h"
#include "imgui.h"

#include <iostream>

VRMenuHandler::VRMenuHandler(bool is2D) :m_active_menu(NULL), m_is2D(is2D), m_imgui2D_initialised(false), m_isHover(false), m_font_scale{1.0f}
{
	m_font_atlas = IM_NEW(ImFontAtlas)();
	
}

VRMenuHandler::~VRMenuHandler()
{
	//delete and clean menus
	for (auto menu : m_menus)
		delete menu;
	
	m_menus.clear();

	//delete font atlas
	delete m_font_atlas;
	//shutdown opengl3 
	ImGui_ImplOpenGL3_Shutdown();
	if(m_imgui2D_initialised)
		ImGui::DestroyContext();
}

void VRMenuHandler::renderToTexture()
{
	if (!m_is2D) {
		//render menus to texture
		for (auto& menu : m_menus)
			menu->renderToTexture();
	}
}

void VRMenuHandler::drawMenu()
{
	if (!m_is2D) {
		//draw menus
		for (auto& menu : m_menus)
			menu->drawMenu();
	}
	else if(!m_menus.empty())
	{
		if (!VRMenu::m_glew_initialised)
		{
			std::cerr << "Init glew" << std::endl;
			glewInit();
			VRMenu::m_glew_initialised = true;
		}

		if (!m_imgui2D_initialised){
			std::cerr << "Init ImGui" << std::endl;
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
		GLint last_viewport[4];
		glGetIntegerv(GL_VIEWPORT, last_viewport);
		io.DisplaySize = ImVec2((float)last_viewport[2], (float)last_viewport[3]);

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

void VRMenuHandler::setControllerPose(const glm::mat4 &controllerpose, float max_distance)
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
	else if(m_imgui2D_initialised)
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

void VRMenuHandler::setCursorPos(int x, int y)
{
	if (m_is2D && m_imgui2D_initialised) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(x, y);
	}
}

VRMenu* VRMenuHandler::addNewMenu(std::function<void()> callback, int res_x, int res_y, float width, float height, float font_size)
{
	m_font_scale = font_size;
	VRMenu * menu = new VRMenu(res_x, res_y, width, height,m_font_atlas, m_is2D, font_size);
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
