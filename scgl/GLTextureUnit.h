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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>
#include "GLShareableState.h"
#include "GLSupport.h"

enum GLTextureStateDiff : unsigned char
{
	GLTextureStateDiff_Clean = 0,
	GLTextureStateDiff_Enabled = (1 << 0),
	GLTextureStateDiff_TextureId = (1 << 1),
	GLTextureStageDiff_TexStageCoord = (1 << 2)
};

class GLTextureUnit
{
public:
	GLTextureUnit(GLuint texUnitId, GLShareableState* sharedState);

public:
	void ApplyStateChanges(void);

public:
	bool Enable(void);
	bool Disable(void);
	bool IsEnabled(void);

	bool SetTexture(GLuint texture);
	void SetTextureImmediately(GLuint texture);
	GLuint GetTexture(void);

	bool TexStageCoord(GLuint gdTexCoordSource);
	bool TexStageMatrix(GLfloat const* matrix, GLuint unknown0, GLuint unknown1, GLuint gdTexMatFlags);

private:
	void ApplyNewEnabledState(void);
	void ApplyNewTexStageCoordState(void);

public:
	bool needsTextureParamRefresh;

private:
	GLuint textureUnitId;
	GLuint textureId;
	GLuint coordSrc;
	void const* textureHandle;
	bool isEnabled;
	bool isIdentityTexMatrix;

private:
	unsigned char stateFlags;

private:
	GLShareableState* sharedState;
};