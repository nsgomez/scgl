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

#include <cassert>
#include "GLTextureUnit.h"
#include "VertexFormatUtils.h"

extern GLenum matrixModeMap[2];

GLTextureUnit::GLTextureUnit(GLuint texUnitId, GLShareableState* sharedState) :
	textureUnitId(texUnitId),
	textureId(0),
	coordSrc(0),
	textureHandle(nullptr),
	isEnabled(texUnitId == 0),
	stateFlags(GLTextureStateDiff_Clean),
	sharedState(sharedState),
	isIdentityTexMatrix(true),
	needsTextureParamRefresh(true)
{
}

void GLTextureUnit::ApplyStateChanges(void) {
	int texCoordOffset = RZVertexFormatElementOffset(sharedState->interleavedFormat, kGDElementType_TexCoord, coordSrc);
	void const* localTextureHandle = reinterpret_cast<uint8_t const*>(sharedState->interleavedPointer) + texCoordOffset;
	bool isTextureHandleDirty = isEnabled && textureHandle != localTextureHandle;

	if (stateFlags || isTextureHandleDirty) {
		if (sharedState->glActiveTextureUnit != textureUnitId) {
			glClientActiveTexture(GL_TEXTURE0 + textureUnitId);
			glActiveTexture(GL_TEXTURE0 + textureUnitId);

			sharedState->glActiveTextureUnit = textureUnitId;
		}

		if (stateFlags & GLTextureStateDiff_Enabled) {
			ApplyNewEnabledState();
		}

		if (stateFlags & GLTextureStateDiff_TextureId) {
			glBindTexture(GL_TEXTURE_2D, textureId);
		}

		if (isTextureHandleDirty) {
			glTexCoordPointer(2, GL_FLOAT, sharedState->interleavedStride, localTextureHandle);
			textureHandle = localTextureHandle;
		}

		if (stateFlags & GLTextureStageDiff_TexStageCoord) {
			ApplyNewTexStageCoordState();
		}

		stateFlags = GLTextureStateDiff_Clean;
	}
}

void GLTextureUnit::ApplyNewEnabledState(void) {
	if (isEnabled) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
	}
	else {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);

		textureHandle = nullptr;
	}
}

void GLTextureUnit::ApplyNewTexStageCoordState(void) {
	static float sCoord[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	static float tCoord[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	static float rCoord[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	if ((coordSrc & 0xfffffff8) == 0x10) { // mimics D3DTSS_TCI_CAMERASPACEPOSITION
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTexGenfv(GL_S, GL_EYE_PLANE, sCoord);
		glTexGenfv(GL_T, GL_EYE_PLANE, tCoord);
		glTexGenfv(GL_R, GL_EYE_PLANE, rCoord);

		glPopMatrix();
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glMatrixMode(matrixModeMap[sharedState->activeMatrixMode]);
	}
	// There are technically TexGen modes for 0x20 (D3DTSS_TCI_CAMERASPACENORMAL) and
	// 0x30 (D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR), but SimCity 4 doesn't seem to
	// use them, so they're left unimplemented.
	else {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
	}
}

bool GLTextureUnit::Enable(void) {
	if (!isEnabled) {
		stateFlags |= (!isEnabled) ? GLTextureStateDiff_Enabled : 0;
		isEnabled = true;

		return true;
	}

	return false;
}

bool GLTextureUnit::Disable(void) {
	if (isEnabled) {
		stateFlags |= (isEnabled) ? GLTextureStateDiff_Enabled : 0;
		isEnabled = false;

		return true;
	}

	return false;
}

bool GLTextureUnit::IsEnabled(void) {
	return isEnabled;
}

bool GLTextureUnit::SetTexture(GLuint texture) {
	if (texture != this->textureId) {
		stateFlags |= GLTextureStateDiff_TextureId;
		SetTextureImmediately(texture);

		return true;
	}

	return false;
}

void GLTextureUnit::SetTextureImmediately(GLuint texture) {
	this->textureId = texture;
	this->needsTextureParamRefresh = true;
}

GLuint GLTextureUnit::GetTexture(void) {
	return textureId;
}

bool GLTextureUnit::TexStageCoord(GLuint gdTexCoordSource) {
	if (gdTexCoordSource != this->coordSrc) {
		stateFlags |= GLTextureStageDiff_TexStageCoord;
		this->coordSrc = gdTexCoordSource;

		return true;
	}

	return false;
}

bool GLTextureUnit::TexStageMatrix(GLfloat const* matrix, GLuint unknown0, GLuint unknown1, GLuint gdTexMatFlags) {
	if (matrix == nullptr) {
		if (!isIdentityTexMatrix) {
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(matrixModeMap[sharedState->activeMatrixMode]);

			isIdentityTexMatrix = true;
			return true;
		}

		return false;
	}

	glMatrixMode(GL_TEXTURE);
	if ((gdTexMatFlags & 3) == 1 && unknown0 == 4 && unknown1 == 2) {
		GLfloat replacementMatrix[16];
		GLfloat* replacementPtr = replacementMatrix;
		for (int i = 0; i < 16; i++) {
			replacementPtr[i] = matrix[i];
		}

		replacementMatrix[2] = 0.0f;
		replacementMatrix[6] = 0.0f;
		replacementMatrix[10] = 1.0f;
		replacementMatrix[14] = 0.0f;

		replacementMatrix[3] = 0.0f;
		replacementMatrix[7] = 0.0f;
		replacementMatrix[11] = 0.0f;
		replacementMatrix[15] = 1.0f;

		glLoadMatrixf(replacementMatrix);
	}
	else if ((gdTexMatFlags & 1) == 0 || (unknown0 > 3 && (unknown1 > 3 || (gdTexMatFlags & 2) == 0))) {
		glLoadMatrixf(matrix);
	}
	else {
		assert(false);
	}

	isIdentityTexMatrix = false;
	glMatrixMode(matrixModeMap[sharedState->activeMatrixMode]);

	return true;
}