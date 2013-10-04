//
// Book:      OpenGL(R) ES 3.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// MultiTexture
//
//    This is an example that draws a quad with a basemap and
//    lightmap to demonstrate multitexturing.
//

package com.openglesbook.multitexture;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;


public class MultiTextureRenderer implements GLSurfaceView.Renderer
{

    ///
    // Constructor
    //
    public MultiTextureRenderer(Context context)
    {
        mContext = context;
        mVertices = ByteBuffer.allocateDirect(mVerticesData.length * 4)
                .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertices.put(mVerticesData).position(0);
        mIndices = ByteBuffer.allocateDirect(mIndicesData.length * 2)
                .order(ByteOrder.nativeOrder()).asShortBuffer();
        mIndices.put(mIndicesData).position(0);
    }

    ///
    //  Load texture from asset
    //
    private int loadTextureFromAsset ( String fileName )
    {
        int[] textureId = new int[1];
        Bitmap bitmap = null;
        InputStream is = null;
        
        try {
        	is = mContext.getAssets().open(fileName);
        } catch (IOException ioe) {
        	is = null;
        }
        
        if (is == null)
        	return 0;
        
        bitmap = BitmapFactory.decodeStream(is);
           
        GLES30.glGenTextures ( 1, textureId, 0 );
        GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, textureId[0] );

        GLUtils.texImage2D(GLES30.GL_TEXTURE_2D, 0, bitmap, 0);
        
        GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR );
        GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR );
        GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE );
        GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE );
        
        return textureId[0];
    }
    
    ///
    // Initialize the shader and program object
    //
    public void onSurfaceCreated(GL10 glUnused, EGLConfig config)
    {
        // Load shaders from 'assets' and get a linked program object
        mProgramObject = ESShader.loadProgramFromAsset(mContext, 
        		                                       "shaders/vertexShader.vert", 
        		                                       "shaders/fragmentShader.frag");

        // Get the sampler locations
        mBaseMapLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_baseMap" );
        mLightMapLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_lightMap" );

        // Load the texture images from 'assets'
        mBaseMapTexId = loadTextureFromAsset("textures/basemap.png");
        mLightMapTexId = loadTextureFromAsset("textures/lightmap.png");

        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // /
    // Draw a triangle using the shader pair created in onSurfaceCreated()
    //
    public void onDrawFrame(GL10 glUnused)
    {
        // Set the view-port
        GLES30.glViewport(0, 0, mWidth, mHeight);

        // Clear the color buffer
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);

        // Use the program object
        GLES30.glUseProgram(mProgramObject);

        // Load the vertex position
        mVertices.position(0);
        GLES30.glVertexAttribPointer ( 0, 3, GLES30.GL_FLOAT, 
                                       false, 
                                       5 * 4, mVertices );
        // Load the texture coordinate
        mVertices.position(3);
        GLES30.glVertexAttribPointer ( 1, 2, GLES30.GL_FLOAT,
                                       false, 
                                       5 * 4, 
                                       mVertices );

        GLES30.glEnableVertexAttribArray ( 0 );
        GLES30.glEnableVertexAttribArray ( 1 );


        // Bind the base map
        GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
        GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mBaseMapTexId );

        // Set the base map sampler to texture unit to 0
        GLES30.glUniform1i ( mBaseMapLoc, 0 );

        // Bind the light map
        GLES30.glActiveTexture ( GLES30.GL_TEXTURE1 );
        GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mLightMapTexId );
        
        // Set the light map sampler to texture unit 1
        GLES30.glUniform1i ( mLightMapLoc, 1 );

        GLES30.glDrawElements ( GLES30.GL_TRIANGLES, 6, GLES30.GL_UNSIGNED_SHORT, mIndices );
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
    
    // Sampler location
    private int mBaseMapLoc;
    private int mLightMapLoc;
    
    // Texture handle
    private int mBaseMapTexId;
    private int mLightMapTexId;
    
    // Additional member variables
    private int mWidth;
    private int mHeight;
    private FloatBuffer mVertices;
    private ShortBuffer mIndices;
    private Context mContext;
    
    private final float[] mVerticesData =
    { 
            -0.5f, 0.5f, 0.0f, // Position 0
            0.0f, 0.0f, // TexCoord 0
            -0.5f, -0.5f, 0.0f, // Position 1
            0.0f, 1.0f, // TexCoord 1
            0.5f, -0.5f, 0.0f, // Position 2
            1.0f, 1.0f, // TexCoord 2
            0.5f, 0.5f, 0.0f, // Position 3
            1.0f, 0.0f // TexCoord 3
    };

    private final short[] mIndicesData =
    { 
            0, 1, 2, 0, 2, 3 
    };
    
}
