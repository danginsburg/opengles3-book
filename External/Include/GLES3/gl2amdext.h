
#ifndef __gl2atiext_h_
#define __gl2atiext_h_

#ifndef __gl2ext_h_
	#include <GLES3/gl2ext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

/*------------------------------------------------------------------------*
 * AMD extension tokens and functions
 *------------------------------------------------------------------------*/

/* private extension */
/* This gives ES 1.1 ability to turn on/off multisampling using glEnable/glDisable. In ES 2.0 this is always turned on */
#define GL_MULTISAMPLE                                 0x809D

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


/* AMD_performance_counters */
#ifndef GL_AMD_performance_counters
#define GL_AMD_performance_counters 1
#define GL_COUNTER_TYPE_AMD                             0x8BC0
#define GL_COUNTER_RANGE_AMD                            0x8BC1
#define GL_UNSIGNED_INT64_AMD                           0x8BC2
#define GL_PERCENTAGE_AMD                               0x8BC3
#define GL_PERFMON_RESULT_AVAILABLE_AMD                 0x8BC4
#define GL_PERFMON_RESULT_SIZE_AMD                      0x8BC5
#define GL_PERFMON_RESULT_AMD                           0x8BC6
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetPerfMonitorGroupsAMD (GLint *numGroups, GLsizei groupsSize, GLuint *groups);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCountersAMD (GLuint group, GLint *numCounters, GLint *maxActiveCounters, GLsizei counterSize, GLuint *counters);
GL_APICALL void GL_APIENTRY glGetPerfMonitorGroupStringAMD (GLuint group, GLsizei bufSize, GLsizei *length, char *groupString);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterStringAMD (GLuint group, GLuint counter, GLsizei bufSize, GLsizei *length, char *counterString);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterInfoAMD (GLuint group, GLuint counter, GLenum pname, void *data);
GL_APICALL void GL_APIENTRY glGenPerfMonitorsAMD (GLsizei n, GLuint *monitors);
GL_APICALL void GL_APIENTRY glDeletePerfMonitorsAMD (GLsizei n, GLuint *monitors);
GL_APICALL void GL_APIENTRY glSelectPerfMonitorCountersAMD (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint *countersList);
GL_APICALL void GL_APIENTRY glBeginPerfMonitorAMD (GLuint monitor);
GL_APICALL void GL_APIENTRY glEndPerfMonitorAMD (GLuint monitor);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterDataAMD (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint *data, GLint *bytesWritten);
#endif
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORGROUPSAMDPROC) (GLint *numGroups, GLsizei groupsSize, GLuint *groups);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERSAMDPROC) (GLuint group, GLint *numCounters, GLint *maxActiveCounters, GLsizei counterSize, GLuint *counters);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORGROUPSTRINGAMDPROC) (GLuint group, GLsizei bufSize, GLsizei *length, char *groupString);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC) (GLuint group, GLuint counter, GLsizei bufSize, GLsizei *length, char *counterString);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERINFOAMDPROC) (GLuint group, GLuint counter, GLenum pname, void *data);
typedef void (GL_APIENTRYP PFNGLGENPERFMONITORSAMDPROC) (GLsizei n, GLuint *monitors);
typedef void (GL_APIENTRYP PFNGLDELETEPERFMONITORSAMDPROC) (GLsizei n, GLuint *monitors);
typedef void (GL_APIENTRYP PFNGLSELECTPERFMONITORCOUNTERSAMDPROC) (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint *countersList);
typedef void (GL_APIENTRYP PFNGLBEGINPERFMONITORAMDPROC) (GLuint monitor);
typedef void (GL_APIENTRYP PFNGLENDPERFMONITORAMDPROC) (GLuint monitor);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERDATAAMDPROC) (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint *data, GLint *bytesWritten);
#endif

/* OES_vertex_type_10_10_10_2 */
#ifndef GL_OES_vertex_type_10_10_10_2
#define GL_OES_vertex_type_10_10_10_2 1
#define GL_UNSIGNED_INT_10_10_10_2_OES                  0x8DF6    // REMOVE ME ONCE RATIFIED AND PUSHED INTO GL2EXT.H
#define GL_INT_10_10_10_2_OES                           0x8DF7    // REMOVE ME ONCE RATIFIED AND PUSHED INTO GL2EXT.H
#endif

/* EXT_texture_type_2_10_10_10_REV */
#ifndef GL_EXT_texture_type_2_10_10_10_REV
#define GL_EXT_texture_type_2_10_10_10_REV 1
#define GL_UNSIGNED_INT_2_10_10_10_REV_EXT              0x8368    // REMOVE ME ONCE PUSHED INTO GL2EXT.H
#endif

/* HintTarget */
#define GL_TEXTURE_COMPRESSION_HINT                     0x84EF    // USED FOR AUTOMATIC ONLINE TEXTURE COMPRESSION

/* OES_packed_depth_stencil */
#ifndef GL_OES_packed_depth_stencil
#define GL_OES_packed_depth_stencil 1
#define GL_DEPTH_STENCIL_OES                            0x84F9    // REMOVE ME ONCE RATIFIED AND PUSHED INTO GL2EXT.H
#define GL_UNSIGNED_INT_24_8_OES                        0x84FA    // REMOVE ME ONCE RATIFIED AND PUSHED INTO GL2EXT.H
#define GL_DEPTH24_STENCIL8_OES                         0x88F0    // REMOVE ME ONCE RATIFIED AND PUSHED INTO GL2EXT.H
#endif

/* AMD_fog_blend */
#ifndef GL_AMD_fog_blend
#define GL_AMD_fog_blend 1
#define GL_FOG_COLOR_AMD                                0x0B66
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glFogfvAMD (GLenum pname, const GLfloat *params);
#endif
typedef void (GL_APIENTRYP PFNGLFOGFVAMDPROC) (GLenum pname, const GLfloat *params);
#endif

/* AMD_shader_binary_Z400 */
#ifndef GL_AMD_shader_binary_Z400
#define GL_AMD_shader_binary_Z400 1
#define GL_Z400_BINARY_AMD                              0x8740    // REMOVE ME ONCE PUSHED INTO GL2EXT.H
#define GL_SHADER_BINARY_LENGTH_AMD                     0x8741    // REMOVE ME ONCE PUSHED INTO GL2EXT.H
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetProgramBinaryAMD (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryformat, void *binary);
#endif
typedef void (GL_APIENTRYP PFNGLGETPROGRAMBINARYAMDPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryformat, void *binary);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __gl2atiext_h_ */
