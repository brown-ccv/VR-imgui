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
///\file VRMenu.h
///\author Benjamin Knorlein
///\date 9/26/2019

#pragma once

#ifndef VRMENU_H
#define VRMENU_H

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

class VRMenu
{
public:
	VRMenu(int res_x, int res_y, float width, float height, ImFontAtlas *font_atlas = NULL, bool is2D = false, float font_scale = 2.0f);
	~VRMenu();

	// renders the UI to texture - only call once per draw loop, e.g. in context
	void renderToTexture();
	void drawMenu();

	// interaction
	float menu_controller_distance(const glm::mat4 &controllerpose, float max_distance = std::numeric_limits<float>::max());
	void setControllerPose(const glm::mat4 &controllerpose);
	void setButtonClick(int button, int state);
	void setAnalogValue(float value);
	void set_callback(std::function<void()> callback);
	void call_callback();
	void setMenuPose(const glm::mat4 pose);
	ImGuiIO &getIO();
	bool isGrabbed()
	{
		return m_grabbing_active;
	};
	void newFrame(bool setContext = true);
	static bool m_glew_initialised;

private:
	// render resolution
	int m_res_x;
	int m_res_y;

	// draw size
	float m_width;
	float m_height;

	// curent menu pose
	glm::mat4 m_pose;

	// callback function
	std::function<void()> m_callback;

	// fbo related
	bool m_fbo_initialised;
	void initFBO();
	unsigned int m_nRenderFramebufferId;
	unsigned int m_nDepthBufferId;
	unsigned int m_nRenderTextureId;
	unsigned int m_nResolveFramebufferId;

	// imgui related
	bool m_imgui_initialised;
	void initImgui();
	ImGuiContext *m_imgui_context;
	ImFontAtlas *m_font_atlas; // shared handled through main

	// new frame timing
	std::chrono::steady_clock::time_point m_lastTime;

	bool m_grabbing_active;
	glm::mat4 m_pose_grab_relative;
	ImVec2 grab_mouse_pos;

	int m_MSAA_buffers;
	float m_font_scale;
	bool m_is2D;
};

#endif // VRMENU_H
