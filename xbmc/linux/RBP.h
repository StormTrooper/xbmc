#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef USE_VCHIQ_ARM
#define USE_VCHIQ_ARM
#endif
#ifndef __VIDEOCORE4__
#define __VIDEOCORE4__
#endif
#ifndef HAVE_VMCS_CONFIG
#define HAVE_VMCS_CONFIG
#endif

#if defined(HAVE_CONFIG_H) && !defined(TARGET_WINDOWS)
#include "config.h"
#define DECLARE_UNUSED(a,b) a __attribute__((unused)) b;
#endif

#if defined(TARGET_RASPBERRY_PI)
#include "DllBCM.h"
#include "OMXCore.h"
#include "cores/omxplayer/OMXImage.h"

#include "system_gl.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "xbmc/filesystem/File.h"
#include <semaphore.h>
#include "threads/Thread.h"

enum TextureAction {TEXTURE_ALLOC, TEXTURE_DELETE };

struct textureinfo {
  TextureAction action;
  int width, height;
  GLuint texture;
  EGLImageKHR egl_image;
  sem_t sync;
  void *parent;
  const char *filename;
};

class CRBP : public CThread
{
protected:
  virtual void OnStartup();
  virtual void OnExit();
  virtual void Process();
public:
  CRBP();
  ~CRBP();

  bool Initialize();
  void LogFirmwareVerison();
  void Deinitialize();
  int GetArmMem() { return m_arm_mem; }
  int GetGpuMem() { return m_gpu_mem; }
  void GetDisplaySize(int &width, int &height);
  // stride can be null for packed output
  unsigned char *CaptureDisplay(int width, int height, int *stride, bool swap_red_blue, bool video_only = true);
  DllOMX *GetDllOMX() { return m_OMX ? m_OMX->GetDll() : NULL; }
  COMXImageFile *LoadJpeg(const CStdString& texturePath);
  void CloseJpeg(COMXImageFile *file);
  bool DecodeJpegToTexture(COMXImageFile *file, unsigned int width, unsigned int height, void **userdata);
  void DestroyTexture(void *userdata);
  void GetTexture(void *userdata, GLuint *texture);

  bool DecodeJpeg(COMXImageFile *file, unsigned int maxWidth, unsigned int maxHeight, unsigned int stride, void *pixels);
  bool ClampLimits(unsigned int &width, unsigned int &height, unsigned int m_width, unsigned int m_height, bool transposed = false);
  bool CreateThumbnailFromSurface(unsigned char* buffer, unsigned int width, unsigned int height,
      unsigned int format, unsigned int pitch, const CStdString& destFile);
  bool CreateThumb(const CStdString& srcFile, unsigned int width, unsigned int height, std::string &additional_info, const CStdString& destFile);
  void CreateContext();
private:
  DllBcmHost *m_DllBcmHost;
  bool       m_initialized;
  bool       m_omx_initialized;
  int        m_arm_mem;
  int        m_gpu_mem;
  EGLDisplay m_egl_display;
  EGLContext m_egl_context;
  COMXCore   *m_OMX;
  pthread_mutex_t   m_texqueue_mutex;
  pthread_cond_t    m_texqueue_cond;
  std::queue <struct textureinfo *> m_texqueue;
  void AllocTextureInternal(struct textureinfo *tex);
  void DestroyTextureInternal(struct textureinfo *tex);
};

extern CRBP g_RBP;
#endif
