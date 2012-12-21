#ifndef __gl2platform_h_
#define __gl2platform_h_

/* $Revision: #1 $ on $Date: 2012/07/11 $ */

/*
 * This document is licensed under the SGI Free Software B License Version
 * 2.0. For details, see http://oss.sgi.com/projects/FreeB/ .
 */

/* Platform-specific types and definitions for OpenGL ES 2.X  gl2.h
 * Last modified on 2008/12/19
 *
 * Adopters may modify khrplatform.h and this file to suit their platform.
 * You are encouraged to submit all modifications to the Khronos group so that
 * they can be included in future versions of this file.  Please submit changes
 * by sending them to the public Khronos Bugzilla (http://khronos.org/bugzilla)
 * by filing a bug against product "OpenGL-ES" component "Registry".
 */

#include <KHR/khrplatform.h>


#if (defined(_WIN32) || defined(__VC32__)) && !defined(_WIN32_WCE) /* Win32 */
#   if defined (_DLL_EXPORTS)
#       define GL_APICALL __declspec(dllexport)
#   else
#       define GL_APICALL __declspec(dllimport)
#   endif
#elif defined(_WIN32_WCE)             /* WinCE */
#   define GL_APICALL
#elif defined (__ARMCC_VERSION)                      /* ADS */
#   define GL_APICALL
#elif defined (__SYMBIAN32__) && defined (__GCC32__) /* Symbian GCC */
#   define GL_APICALL __declspec(dllexport)
#elif defined (__GNUC__)                             /* GCC dependencies (kludge) */
#   define GL_APICALL
#endif


#if (defined(_WIN32) || defined(__VC32__)) && !defined(_WIN32_WCE) /* Win32 */
#define GL_APIENTRY __stdcall
#else
#define GL_APIENTRY
#endif



#endif /* __gl2platform_h_ */
