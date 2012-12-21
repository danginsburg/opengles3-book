#ifndef __gl2amdext_h_
#define __gl2amdext_h_
/*
** Copyright (C) 2008 AMD Inc.
*/

#ifndef __gl2ext_h_
#   include <GLES3/gl2ext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

#   include <KHR/khrplatform.h>
/*------------------------------------------------------------------------*
 * AMD extension tokens and functions
 *------------------------------------------------------------------------*/

/* private extension */
/* This gives ES 1.1 ability to turn on/off multisampling using glEnable/glDisable. In ES 2.0 this is always turned on */
#define GL_MULTISAMPLE                                 0x809D

/* private extension */
/* This gives ES 1.1 ability to query state with GL_FIXED type */
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetFixedvAMD (GLenum pname, GLfixed *params);
#endif
typedef void (GL_APIENTRYP PFNGLGETFIXEDVAMDPROC) (GLenum pname, GLfixed *params);

/* AMD_alpha_test */
#ifndef GL_AMD_alpha_test
#define GL_AMD_alpha_test 1
#define GL_ALPHA_TEST_AMD                              0x0BC0
#define GL_ALPHA_TEST_FUNC_AMD                         0x0BC1
#define GL_ALPHA_TEST_REF_AMD                          0x0BC2
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glAlphaFuncAMD (GLenum func, GLclampf ref);
#endif
typedef void (GL_APIENTRYP PFNGLALPHAFUNCAMDPROC) (GLenum func, GLclampf ref);
#endif

/* AMD_logic_op */
#ifndef GL_AMD_logic_op
#define GL_AMD_logic_op 1
#define GL_COLOR_LOGIC_OP_AMD                          0x0BF2
#define GL_LOGIC_OP_MODE_AMD                           0x0BF0
#define GL_CLEAR_AMD                                   0x1500
#define GL_AND_AMD                                     0x1501
#define GL_AND_REVERSE_AMD                             0x1502
#define GL_COPY_AMD                                    0x1503
#define GL_AND_INVERTED_AMD                            0x1504
#define GL_NOOP_AMD                                    0x1505
#define GL_XOR_AMD                                     0x1506
#define GL_OR_AMD                                      0x1507
#define GL_NOR_AMD                                     0x1508
#define GL_EQUIV_AMD                                   0x1509
#define GL_INVERT_AMD                                  0x150A
#define GL_OR_REVERSE_AMD                              0x150B
#define GL_COPY_INVERTED_AMD                           0x150C
#define GL_OR_INVERTED_AMD                             0x150D
#define GL_NAND_AMD                                    0x150E
#define GL_SET_AMD                                     0x150F
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glLogicOpAMD (GLenum op);
#endif
typedef void (GL_APIENTRYP PFNGLLOGICOPAMDPROC) (GLenum op);
#endif

/* HintTarget */
#define GL_TEXTURE_COMPRESSION_HINT                     0x84EF    // USED FOR AUTOMATIC ONLINE TEXTURE COMPRESSION

/* AMD_fog_blend */
#ifndef GL_AMD_fog_blend
#define GL_AMD_fog_blend 1
#define GL_FOG_COLOR_AMD                                0x0B66
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFogfvAMD (GLenum pname, const GLfloat *params);
#endif
typedef void (GL_APIENTRYP PFNGLFOGFVAMDPROC) (GLenum pname, const GLfloat *params);
#endif

/* AMD_EGL_image_external_layout_specifier */
#ifndef GL_AMD_EGL_image_external_layout_specifier
#define GL_AMD_EGL_image_external_layout_specifier 1
// YUV format
#define GL_YUV_AMD                                      0x8BC7
// YUV types
#define GL_YUYV_AMD                                     0x8BC8
#define GL_UYVY_AMD                                     0x8BC9
#define GL_NV21_AMD                                     0x8BCA
#define GL_YV12_AMD                                     0x8BCB
#define GL_NV12_TILED_AMD                               0x8BCC
// YUV samplers
#define GL_SAMPLER_EXTERNAL_YUYV_AMD                    0x8BCD
#define GL_SAMPLER_EXTERNAL_UYVY_AMD                    0x8BCE
#define GL_SAMPLER_EXTERNAL_NV21_AMD                    0x8BCF
#define GL_SAMPLER_EXTERNAL_YV12_AMD                    0x8BD0
#define GL_SAMPLER_EXTERNAL_NV12_TILED_AMD              0x8BD1
#endif

/* EXT_blend_minmax */
#ifndef GL_EXT_blend_minmax
#define GL_EXT_blend_minmax 1
#define GL_MIN_EXT                                      0x8007
#define GL_MAX_EXT                                      0x8008
#endif

/* QCOM_tiled_rendering */
#ifndef GL_QCOM_tiled_rendering
#define GL_QCOM_tiled_rendering 1
#define GL_COLOR_BUFFER_BIT0_QCOM                       0x00000001
#define GL_COLOR_BUFFER_BIT1_QCOM                       0x00000002
#define GL_COLOR_BUFFER_BIT2_QCOM                       0x00000004
#define GL_COLOR_BUFFER_BIT3_QCOM                       0x00000008
#define GL_COLOR_BUFFER_BIT4_QCOM                       0x00000010
#define GL_COLOR_BUFFER_BIT5_QCOM                       0x00000020
#define GL_COLOR_BUFFER_BIT6_QCOM                       0x00000040
#define GL_COLOR_BUFFER_BIT7_QCOM                       0x00000080
#define GL_DEPTH_BUFFER_BIT0_QCOM                       0x00000100
#define GL_DEPTH_BUFFER_BIT1_QCOM                       0x00000200
#define GL_DEPTH_BUFFER_BIT2_QCOM                       0x00000400
#define GL_DEPTH_BUFFER_BIT3_QCOM                       0x00000800
#define GL_DEPTH_BUFFER_BIT4_QCOM                       0x00001000
#define GL_DEPTH_BUFFER_BIT5_QCOM                       0x00002000
#define GL_DEPTH_BUFFER_BIT6_QCOM                       0x00004000
#define GL_DEPTH_BUFFER_BIT7_QCOM                       0x00008000
#define GL_STENCIL_BUFFER_BIT0_QCOM                     0x00010000
#define GL_STENCIL_BUFFER_BIT1_QCOM                     0x00020000
#define GL_STENCIL_BUFFER_BIT2_QCOM                     0x00040000
#define GL_STENCIL_BUFFER_BIT3_QCOM                     0x00080000
#define GL_STENCIL_BUFFER_BIT4_QCOM                     0x00100000
#define GL_STENCIL_BUFFER_BIT5_QCOM                     0x00200000
#define GL_STENCIL_BUFFER_BIT6_QCOM                     0x00400000
#define GL_STENCIL_BUFFER_BIT7_QCOM                     0x00800000
#define GL_MULTISAMPLE_BUFFER_BIT0_QCOM                 0x01000000
#define GL_MULTISAMPLE_BUFFER_BIT1_QCOM                 0x02000000
#define GL_MULTISAMPLE_BUFFER_BIT2_QCOM                 0x04000000
#define GL_MULTISAMPLE_BUFFER_BIT3_QCOM                 0x08000000
#define GL_MULTISAMPLE_BUFFER_BIT4_QCOM                 0x10000000
#define GL_MULTISAMPLE_BUFFER_BIT5_QCOM                 0x20000000
#define GL_MULTISAMPLE_BUFFER_BIT6_QCOM                 0x40000000
#define GL_MULTISAMPLE_BUFFER_BIT7_QCOM                 0x80000000
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glStartTilingQCOM(GLuint x, GLuint y, GLuint width, GLuint height,
                                              GLbitfield preserveMask);
GL_APICALL void GL_APIENTRY glEndTilingQCOM(GLbitfield preserveMask);
#endif
typedef void (GL_APIENTRYP PFNGLSTARTTILINGQCOM) (GLuint x, GLuint y, GLuint width, GLuint height,
                                                  GLbitfield preserveMask);
typedef void (GL_APIENTRYP PFNGLENDTILINGQCOM) (GLbitfield preserveMask);
#endif

/* AMD_panel_settings */
#ifndef GL_AMD_panel_settings
#define GL_AMD_panel_settings 1
#define GL_PANEL_MODE_IFD_AMD                   0x8880 //TEMP TOKEN
#define GL_PANEL_MODE_IFH_AMD            	    0x8881 //TEMP TOKEN
#define GL_PANEL_POWERFLAGS_OVERRIDE_AMD        0x8882 //TEMP TOKEN
#define GL_PANEL_AUTO_TEX_COMPRESSION_AMD       0x8883 //TEMP TOKEN
#endif

/* GL_OES_egl_image_external (DRAFT) */
#ifndef GL_OES_egl_image_external
#define GL_OES_egl_image_external 1
#define GL_TEXTURE_EXTERNAL_OES                         0x8D65
#define GL_SAMPLER_EXTERNAL_OES                         0x8D66
#define GL_TEXTURE_BINDING_EXTERNAL_OES                 0x8D67
#define GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES             0x8D68
#endif

/* QCOM_texture_format_ARGB */
#ifndef GL_QCOM_texture_format_ARGB
#define GL_QCOM_texture_format_ARGB 1
#define GL_ARGB_QCOM                                    0x8FA3
#define GL_UNSIGNED_SHORT_1_5_5_5_QCOM                  0x8FA2
#endif

/* QCOM_texture_format_XRGB */
#ifndef GL_QCOM_texture_format_XRGB
#define GL_QCOM_texture_format_XRGB 1
#define GL_XRGB_QCOM                                    0x8FA1
#define GL_UNSIGNED_SHORT_1_5_5_5_QCOM                  0x8FA2
#endif

/* QCOM_memory_monitor */
#ifndef GL_QCOM_memory_extension
#define GL_QCOM_memory_extension

typedef khronos_int64_t     GLint64QCOM;

#ifdef  GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY
glGetMemoryStatsQCOM(GLenum pname, GLenum usage, GLint64QCOM* param);
GL_APICALL void GL_APIENTRY
glGetSizedMemoryStatsQCOM(GLsizei maxcount, GLsizei* count, GLint64QCOM *buf);
#endif

#define GL_VMEM_TOTAL_AVAILABLE_QCOM                     0x8FA4
#define GL_VMEM_USED_THIS_PROCESS_QCOM                   0x8FA5
#define GL_VMEM_USED_ALL_PROCESSES_QCOM                  0x8FA6
#define GL_VMEM_LARGEST_CONTIGUOUS_BLOCK_REMAINING_QCOM  0x8FA7
#define GL_HEAPMEM_TOTAL_AVAILABLE_QCOM                  0x8FA8
#define GL_HEAPMEM_USED_THIS_PROCESS_QCOM                0x8FA9

#define GL_CLIENT_VERTEX_ARRAY_QCOM                      0x8FAA
#define GL_OTHER_GL_USAGE_QCOM                           0x8FAB
#define GL_EGL_USAGE_QCOM                                0x8FAC
#define GL_2D_USAGE_QCOM                                 0x8FAD
#define GL_OTHER_USAGE_QCOM                              0x8FAE
#define GL_ALL_USAGE_QCOM                                0x8FAF

typedef void (GL_APIENTRYP PFNGLGETMEMORYSTATSQCOM) (GLenum pname, GLenum usage, GLint64QCOM* param);
typedef void (GL_APIENTRYP PFNGLGETSIZEDMEMORYSTATSQCOM) (GLsizei maxcount, GLsizei* count, GLint64QCOM* param);
#endif  //GL_QCOM_memory_extension


/* EXT_texture_compression_s3tc */
#ifndef GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                  0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                 0x83F3
#endif

#ifdef __cplusplus
}
#endif

#endif /* __gl2amdext_h_ */
