//
// Book:      OpenGL(R) ES 3.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Example_6_6
//
//    This example demonstrates drawing a primitive with
//    a separate VBO per attribute 
//    
//

package com.openglesbook.example6_6;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

public class Example6_6Renderer implements GLSurfaceView.Renderer
{

    ///
    // Constructor
    //
    public Example6_6Renderer(Context context)
    {        
        mVertices = ByteBuffer.allocateDirect(mVerticesData.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertices.put(mVerticesData).position(0);
        
        mColors = ByteBuffer.allocateDirect(mColorData.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mColors.put(mColorData).position(0);
        
        mIndices = ByteBuffer.allocateDirect(mIndicesData.length * 2)
                .order(ByteOrder.nativeOrder()).asShortBuffer();
        mIndices.put(mIndicesData).position(0);
    }

    ///
    // Initialize the shader and program object
    //
    public void onSurfaceCreated(GL10 glUnused, EGLConfig config)
    {
    	String vShaderStr =  
    			  "#version 300 es                            \n" +
    		      "layout(location = 0) in vec4 a_position;   \n" +
    		      "layout(location = 1) in vec4 a_color;      \n" +
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
        
        mVBOIds[0] = 0;
        mVBOIds[1] = 0;
        mVBOIds[2] = 0;
        
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
        
        drawPrimitiveWithVBOs();
    }
    
    private void drawPrimitiveWithVBOs() 
    {
    	int numVertices = 3;
    	int numIndices = 3;
    	
    	// mVBOIds[0] - used to store vertex position
    	// mVBOIds[1] - used to store vertex color
        // mVBOIds[2] - used to store element indices 
    	if ( mVBOIds[0] == 0 && mVBOIds[1] == 0 && mVBOIds[2] == 0)
    	{
    		// Only allocate on the first draw
    		GLES30.glGenBuffers(3, mVBOIds, 0);
	    
    		mVertices.position(0);
    		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mVBOIds[0]);
    		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, vtxStrides[0] * numVertices,
    				 	        mVertices, GLES30.GL_STATIC_DRAW);
    		
    		mColors.position(0);
    		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mVBOIds[1]); 
    		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, vtxStrides[1] * numVertices,
	                   	        mColors, GLES30.GL_STATIC_DRAW); 
    		
    		mIndices.position(0);
    		GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, mVBOIds[2]); 
    		GLES30.glBufferData(GLES30.GL_ELEMENT_ARRAY_BUFFER, 2 * numIndices,
	                   	        mIndices, GLES30.GL_STATIC_DRAW);
    	}
    	
    	GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mVBOIds[0]);
    	
    	GLES30.glEnableVertexAttribArray(VERTEX_POS_INDX); 
    	GLES30.glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
    			                     GLES30.GL_FLOAT, false, vtxStrides[0], 0);

    	GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mVBOIds[1]);
    	   
    	GLES30.glEnableVertexAttribArray(VERTEX_COLOR_INDX);    
    	GLES30.glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
    	                             GLES30.GL_FLOAT, false, vtxStrides[1], 0);

    	GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, mVBOIds[2]); 
    	      
    	GLES30.glDrawElements(GLES30.GL_TRIANGLES, numIndices, 
    	                      GLES30.GL_UNSIGNED_SHORT, 0);

    	GLES30.glDisableVertexAttribArray(VERTEX_POS_INDX); 
    	GLES30.glDisableVertexAttribArray(VERTEX_COLOR_INDX); 

    	GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0);
    	GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, 0);
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
    private FloatBuffer mColors;
    private ShortBuffer mIndices;
    
    // VertexBufferObject Ids
    private int [] mVBOIds = new int[3];
    
    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    private final float[] mVerticesData =
    { 
    		0.0f,  0.5f, 0.0f, // v0
    	   -0.5f, -0.5f, 0.0f, // v1
    	    0.5f, -0.5f, 0.0f  // v2
    };
    
    private final short[] mIndicesData =
    { 
    		0, 1, 2 
    };
     
    private final float [] mColorData = 
    {  
    		1.0f, 0.0f, 0.0f, 1.0f,   // c0
    		0.0f, 1.0f, 0.0f, 1.0f,   // c1
    		0.0f, 0.0f, 1.0f, 1.0f    // c2
    };
    
    final int VERTEX_POS_SIZE   = 3; // x, y and z
    final int VERTEX_COLOR_SIZE = 4; // r, g, b, and a

    final int VERTEX_POS_INDX   = 0;
    final int VERTEX_COLOR_INDX = 1;
    
    private int vtxStrides[] = 
    { 
    		VERTEX_POS_SIZE * 4, 
    		VERTEX_COLOR_SIZE * 4
    };
}
