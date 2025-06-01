/*
 *  SCGL - a free OpenGL driver for SimCity 4's SimGL interface
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "../cGDriver.h"
#include "../GLSupport.h"

namespace nSCGL
{
	void cGDriver::EnableLighting(bool flag) {
		flag ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
	}

	void cGDriver::EnableLight(uint32_t light, bool flag) {
		flag ? glEnable(GL_LIGHT0 + light) : glDisable(GL_LIGHT0 + light);
	}

	void cGDriver::LightModelAmbient(float r, float g, float b, float a) {
		GLfloat params[] = { r, g, b, a };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, params);
	}

	void cGDriver::LightColor(uint32_t lightIndex, uint32_t gdParam, float const* color) {
		static GLenum colorParamMap[] = { GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR };
		SIZE_CHECK(gdParam, colorParamMap);

		GLenum param = colorParamMap[gdParam];
		glLightfv(GL_LIGHT0 + lightIndex, param, color);
	}

	void cGDriver::LightColor(uint32_t lightIndex, float const* ambient, float const* diffuse, float const* specular) {
		GLenum light = GL_LIGHT0 + lightIndex;
		if (ambient) {
			glLightfv(light, GL_AMBIENT, ambient);
		}

		if (diffuse) {
			glLightfv(light, GL_DIFFUSE, diffuse);
		}

		if (specular) {
			glLightfv(light, GL_SPECULAR, specular);
		}
	}

	void cGDriver::LightPosition(uint32_t lightIndex, float const* position) {
		glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, position);
	}

	void cGDriver::LightDirection(uint32_t lightIndex, float const* direction) {
		NOTIMPL();
	}

	void cGDriver::MaterialColor(uint32_t gdParam, float const* color) {
		static GLenum materialParamMap[] = { GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION, GL_SHININESS };
		SIZE_CHECK(gdParam, materialParamMap);

		GLenum param = materialParamMap[gdParam];
		glMaterialfv(GL_FRONT, param, color);
	}

	void cGDriver::MaterialColor(
		float const* ambient,
		float const* diffuse,
		float const* specular,
		float const* emission,
		float shininess)
	{
		if (ambient) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		}

		if (diffuse) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		}

		if (specular) {
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		}

		if (emission) {
			glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		}

		if (shininess >= 0.0f) {
			glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
		}
	}
}