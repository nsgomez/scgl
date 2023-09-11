#pragma once
#include "GLSupport.h"

enum class GLTextureState
{
	Clean = 0,
};

class GLTextureUnit
{
public:
	void BindTexture(GLenum target, GLuint texture);

private:
	void ApplyStateChanges();

private:
};