/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "devicecaps.h"

namespace FIFE {

	ScreenMode::ScreenMode() :
		           m_width(0), m_height(0), m_bpp(0), m_SDLFlags(0){
	}

	ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags) :
		           m_width(width), m_height(height), m_bpp(bpp), m_SDLFlags(SDLFlags){
	}

	ScreenMode::ScreenMode(const ScreenMode& rhs){
		m_width = rhs.getWidth();
		m_height = rhs.getHeight();
		m_bpp = rhs.getBPP();
		m_SDLFlags = rhs.getSDLFlags();
	}

	DeviceCaps::DeviceCaps() :
	    m_driverName("Invalid"),
		m_hwAvailable(false),
		m_wmAvailable(false),
		m_hwBlitAccel(false),
		m_hwCCBlitAccel(false),
		m_hwToHwAlphaBlitAccel(false),
		m_swToHwBlitAccel(false),
		m_swToHwCCBlistAccel(false),
		m_swToHwAlphaBlitAccel(false),
		m_BlitFillAccel(false),
		m_videoMem(0) {

		fillAvailableDrivers();
	}


	DeviceCaps::~DeviceCaps() {
	}

	void DeviceCaps::reset() {
		m_screenModes.clear();
		m_driverName = "Invalid";
		m_hwAvailable = false;
		m_wmAvailable = false;
		m_hwBlitAccel = false;
		m_hwCCBlitAccel = false;
		m_hwToHwAlphaBlitAccel = false;
		m_swToHwBlitAccel = false;
		m_swToHwCCBlistAccel = false;
		m_swToHwAlphaBlitAccel = false;
		m_BlitFillAccel = false;
		m_videoMem = 0;
	}


	void DeviceCaps::fillAvailableDrivers() {
		m_availableDrivers.clear();
#if defined( __unix__ )
		m_availableDrivers.push_back("x11");
		m_availableDrivers.push_back("nanox");
		m_availableDrivers.push_back("qtopia");
		m_availableDrivers.push_back("fbcon");
		m_availableDrivers.push_back("directfb");
		m_availableDrivers.push_back("svgalib");
#endif

// Win32
#if defined( WIN32 )
		m_availableDrivers.push_back("directx");
		m_availableDrivers.push_back("windib");
#endif

// Macintosh
#if defined( __APPLE_CC__ )
		m_availableDrivers.push_back("Quartz");
		m_availableDrivers.push_back("x11");
#endif
	}

	void DeviceCaps::fillDeviceCaps() {
		int bufferSize = 256;
		char buffer[bufferSize];

		int numBPP = 1;
		int bpps[numBPP];

		//clear in case this is called twice
		reset();

		//FLAGS
#ifdef HAVE_OPENGL
		int numFlags = 4;
		Uint32 flags[numFlags];

		//OpenGL, windowed, hw accel
		flags[0] = ScreenMode::HW_WINDOWED_OPENGL;
		//OpenGL, fullscree, hw accel
		flags[1] = ScreenMode::HW_FULLSCREEN_OPENGL;
		//SDL, windowed
		flags[2] = ScreenMode::WINDOWED_SDL;
		//SDL, fullscreen
		flags[3] = ScreenMode::FULLSCREEN_SDL;
#else
		int numFlags = 2;
		Uint32 flags[numFlags];

		//SDL, windowed
		flags[0] = ScreenMode::WINDOWED_SDL;
		//SDL, fullscreen
		flags[1] = ScreenMode::FULLSCREEN_SDL;
#endif
		//BITS PER PIXEL

		bpps[0] = 32;
		
		//COMMON FS RESOLUTIONS
		int resolutions[15][2] = {
			{640, 480},
			{800, 600},
			{1024, 768},
			{1152, 864},
			{1280, 768},
			{1280, 800},
			{1280, 960},
			{1280, 1024},
			{1366, 768},
			{1440, 900},
			{1600, 900},
			{1600, 1200},
			{1680, 1050},
			{1920, 1080},
			{1920, 1200}
		};
		int numRes = 15;


		for (int i = 0; i < numBPP; ++i){
			for (int j = 0; j < numFlags; ++j) {
				for ( int k = 0; k < numRes; ++k) {
					int bpp;
					if (flags[j] & SDL_FULLSCREEN) {
						bpp = SDL_VideoModeOK(resolutions[k][0],resolutions[k][1], bpps[i], flags[j]);
						
						if (bpp > 0) {
							ScreenMode mode = ScreenMode(resolutions[k][0],resolutions[k][1], bpps[i], flags[j]);
							m_screenModes.push_back(mode);
						}
					}
					else {  //windowed mode
						//check an arbitrary value as we know all resolutions are supported in windowed mode.
						//we are checking to make sure the bpp is supported here.
						bpp = SDL_VideoModeOK(resolutions[k][0],resolutions[k][1], bpps[i], flags[j]);
						if (bpp > 0) {
							ScreenMode mode = ScreenMode(0,0, bpps[i], flags[j]);
							m_screenModes.push_back(mode);
							break; //insert windowed mode once as all resolutions are supported.
						}
					}

				}
			}
		}

		if(SDL_VideoDriverName(buffer, bufferSize) != NULL) {
			m_driverName = std::string(buffer);
		}
		else {
			m_driverName = "Unknown";
		}

		const SDL_VideoInfo* vInfo = SDL_GetVideoInfo();

		m_hwAvailable = vInfo->hw_available;
		m_wmAvailable = vInfo->wm_available;
		m_hwBlitAccel = vInfo->blit_hw;
		m_hwCCBlitAccel = vInfo->blit_hw_CC;
		m_hwToHwAlphaBlitAccel = vInfo->blit_hw_A;
		m_swToHwBlitAccel = vInfo->blit_sw;
		m_swToHwCCBlistAccel = vInfo->blit_sw_CC;
		m_swToHwAlphaBlitAccel = vInfo->blit_sw_A;
		m_BlitFillAccel = vInfo->blit_fill;
		m_videoMem = vInfo->video_mem;
	}

	ScreenMode DeviceCaps::getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const {
		ScreenMode mode;
		bool foundMode = false;

		bool widthCheck = false;
		bool heightCheck = false;
		bool bppCheck = false;
		bool rendCheck = false;
		bool fsCheck = false;


		for (uint32_t i = 0; i < m_screenModes.size(); i++) {
			if (m_screenModes[i].getWidth() == width) {
				widthCheck = true;
			}
			if (m_screenModes[i].getHeight() == height) {
				heightCheck = true;
			}
			if (m_screenModes[i].getBPP() == bpp) {
				bppCheck = true;
			}
			if (m_screenModes[i].isFullScreen() == fs) {
				fsCheck = true;
			}

			if ((m_screenModes[i].isOpenGL() && renderer == "OpenGL") || (!m_screenModes[i].isOpenGL() && renderer == "SDL")){
				rendCheck = true;
			}

			//check for exact match
			if (widthCheck && heightCheck && bppCheck && fsCheck && rendCheck) {
				mode = m_screenModes[i];
				foundMode = true;
				break;
			}

			//@note When the width and height to 0 that means that all
			//resolutions are supported
			if (m_screenModes[i].getWidth() == 0 && m_screenModes[i].getHeight() == 0 && bppCheck && fsCheck && rendCheck) {
				mode = ScreenMode(width, height, bpp, m_screenModes[i].getSDLFlags());
				foundMode = true;
				break;
			}

			widthCheck = false;
			heightCheck = false;
			bppCheck = false;
			rendCheck = false;
			fsCheck = false;
		}

		if (!foundMode) {
			throw NotSupported("Could not find a maching screen mode for the values given!");
		}

		return mode;
	}

} //FIFE