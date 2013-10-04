//
// Book:      OpenGL(R) ES 3.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Example_6_3
//
//    This example demonstrates using client-side vertex arrays
//    and a constant vertex attribute
//

package com.openglesbook.example6_3;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

public class Example6_3Renderer implements GLSurfaceView.Renderer
{

    ///
    // Constructor
    //
    public Example6_3Renderer(Context context)
    {
        
        mVertices = ByteBuffer.allocateDirect(mVerticesData.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertices.put(mVerticesData).position(0);
    }

    ///
    // Initialize the shader and program object
    //
    public void onSurfaceCreated(GL10 glUnused, EGLConfig config)
    {
    	String vShaderStr =  
    		      "#version 300 es                            \n" +
    		      "layout(location = 0) in vec4 a_color;      \n" +
    		      "layout(location = 1) in vec4 a_position;   \n" +
    		      "out vec4 v_color;                          \n" +
    		      "void main()                                \n" +
    		      "{                                          \n" +
    		      "    v_color = a_color;                     \n" +
    		      "    gl_Position = a_position;              \n" +
    		      "}";

    		   
    	String fShaderStr =
    		      "#version 300 es            \n" +
    		      "precision mediump float;   \n" +
    		      "in vec4 v_color;           \n" +
    		      "out vec4 o_fragColor;      \n" +
    		      "void main()                \n" +
    		      "{                          \n" +
    		      "    o_fragColor = v_color; \n" +
    		      "}" ;
        
        // Load the shaders and get a linked program object
        mProgramObject = ESShader.loadProgram(vShaderStr, fShaderStr);
        
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // /
    // Draw a triangle using the shader pair created in onSurfaceCreated()
    //
    public void onDrawFrame(GL10 glUnused)
    {
        // Set the viewport
        GLES30.glViewport(0, 0, mWidth, mHeight);

        // Clear the color buffer
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);

        // Use the program object
        GLES30.glUseProgram(mProgramObject);
        
        // Set the vertex color to red
        GLES30.glVertexAttrib4f( 0, 1.0f, 0.0f, 0.0f, 1.0f );
        
        // Load the vertex position
        mVertices.position(0);
        GLES30.glVertexAttribPointer ( 1, 3, GLES30.GL_FLOAT, 
                                       false, 
                                       0, mVertices );
        
        GLES30.glEnableVertexAttribArray ( 1 );

        GLES30.glDrawArrays ( GLES30.GL_TRIANGLES, 0, 3 );
        
        GLES30.glDisableVertexAttribArray ( 1 );
    }

    ///
    // Handle surface changes
    //
    public void onSurfaceChanged(GL10 glUnused, int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }

    
    // Handle to a program object
    private int mProgramObject;
   
    // Additional member variables
    private int mWidth;
    private int mHeight;
    private FloatBuffer mVertices;
    
    private final float[] mVerticesData =
    { 
    		0.0f,  0.5f, 0.0f, // v0
    	   -0.5f, -0.5f, 0.0f, // v1
    	    0.5f, -0.5f, 0.0f  // v2
    };
}
