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
///\file VRMenuHandler.h
///\author Benjamin Knorlein
///\date 10/1/2019

#pragma once

#ifndef VRMENUHANDLER_H
#define VRMENUHANDLER_H
#include <vector>
#include "VRMenu.h"

class VRMenuHandler
{
public:
	VRMenuHandler(bool is2D = false);
	~VRMenuHandler();

	// creation and removal
	VRMenu *addNewMenu(std::function<void()> callback, int res_x, int res_y, float width, float height, float font_size = 2.0);
	void deleteMenu(VRMenu *menu);

	// render
	void renderToTexture();
	void drawMenu(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,int window_width, int window_height, int framebuffer_width, int framebuffer_height);

	// interaction
	void setControllerPose(const glm::mat4 &controllerpose, float max_distance = std::numeric_limits<float>::max());
	void setButtonClick(int button, int state);
	void setAnalogValue(float value);
	void setCursorPos(float x, float y);
	void initGL();

	bool windowIsActive()
	{
		if (!m_is2D)
			return m_active_menu != NULL;
		else
			return m_isHover;
	}

private:
	ImFontAtlas *m_font_atlas;
	std::vector<VRMenu *> m_menus;
	VRMenu *m_active_menu;

	bool m_is2D;
	bool m_imgui2D_initialised;
	bool m_isHover;
	float m_font_scale;

	unsigned int  vshader, fshader, shaderProgram;
	unsigned int VBO, VAO, EBO;
};

#endif // VRMENUHANDLER_H
