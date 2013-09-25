//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com

#import "FileWrapper.h"
#import <Foundation/Foundation.h>

///
/// \brief Given a fileName, convert into a path that can be used to open from
/// the mainBundle
/// \param fileName Name of file to convert to mainBundle path
/// \return Path that can be used to fopen() from the mainBundle
///
const char *GetBundleFileName( const char *fileName )
{
    NSString* fileNameNS = [NSString stringWithUTF8String:fileName];
    NSString* baseName = [fileNameNS stringByDeletingPathExtension];
    NSString* extension = [fileNameNS pathExtension];
    NSString *path = [[NSBundle mainBundle] pathForResource: baseName ofType: extension ];
    fileName = [path cStringUsingEncoding:1];
    
    return fileName;
}