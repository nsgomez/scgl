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