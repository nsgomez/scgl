#include "cGDriver.h"
#include "GLSupport.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace nSCGL
{
	uint32_t cGDriver::CountVideoModes(void) const {
		return videoModeCount;
	}

	void cGDriver::GetVideoModeInfo(uint32_t dwIndex, sGDMode& gdMode) {
		if (dwIndex == -1 || dwIndex >= (uint32_t)videoModeCount) {
			SetLastError(DriverError::OUT_OF_RANGE);
			return;
		}

		sGDMode tmp = videoModes[dwIndex];
		gdMode = tmp;
	}

	void cGDriver::GetVideoModeInfo(sGDMode& gdMode) {
		return GetVideoModeInfo(currentVideoMode, gdMode);
	}

	void cGDriver::SetVideoMode(int32_t newModeIndex, void* hwndProc, bool showWindow, bool) {
		if (newModeIndex == -1) {
			glfwHideWindow(window);

			currentVideoMode = -1;
			windowWidth = 0;
			windowHeight = 0;

			SetLastError(DriverError::OK);
		}
		else if (newModeIndex < videoModeCount) {
			sGDMode const& newMode = videoModes[newModeIndex];
			GLFWmonitor* monitor = nullptr;

			currentVideoMode = newModeIndex;
			windowWidth = newMode.width;
			windowHeight = newMode.height;

			if (hwndProc == nullptr) {
				hwndProc = DefWindowProcA;
			}

			HWND hwnd = glfwGetWin32Window(window);
			SetWindowLong(hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(hwndProc));

			glfwSetWindowSize(window, windowWidth, windowHeight);
			glfwSwapInterval(0);
			glfwShowWindow(window);

			if (newMode.isFullscreen) {
				monitor = glfwGetPrimaryMonitor();
				glfwSetWindowMonitor(window, monitor, 0, 0, windowWidth, windowHeight, GLFW_DONT_CARE);
			}

			glEnableClientState(GL_VERTEX_ARRAY);
			SetViewport();
			SetLastError(DriverError::OK);
		}
		else {
			SetLastError(DriverError::OUT_OF_RANGE);
		}
	}

	bool cGDriver::IsDeviceReady(void) {
		return true;
	}

	void cGDriver::Flush(void) {
		glFlush();
		if (window != nullptr) {
			glfwSwapBuffers(window);
		}
	}

	void cGDriver::SetViewport(void) {
		glViewport(0, 0, windowWidth, windowHeight);
		glDisable(GL_SCISSOR_TEST);

		viewportX = viewportY = 0;
		viewportWidth = windowWidth;
		viewportHeight = windowHeight;
	}

	void cGDriver::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
		glViewport(x, y, width, height);
		glScissor(x, y, width, height);
		glEnable(GL_SCISSOR_TEST);

		viewportX = x;
		viewportY = y;
		viewportWidth = width;
		viewportHeight = height;
	}

	void cGDriver::GetViewport(int32_t dimensions[4]) {
		dimensions[0] = viewportX;
		dimensions[1] = viewportY;
		dimensions[2] = viewportX + viewportWidth;
		dimensions[3] = viewportY + viewportHeight;
	}
}