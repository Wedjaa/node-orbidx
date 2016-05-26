#ifndef __NODE_COMMON_H__
#define __NODE_COMMON_H__

#ifdef WIN
    /*
        This is needed on Windows for Visual Studio to not throw an error in the
        build/include/opencv2/flann/any.h file in OpenCV.
    */
    namespace std{ typedef type_info type_info; }
#endif

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_version.h>
#include <node_buffer.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#if CV_MAJOR_VERSION >= 3
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv_modules.hpp>
#endif
#if ((CV_MAJOR_VERSION == 2) && (CV_MINOR_VERSION >=4) && (CV_SUBMINOR_VERSION>=4))
#define HAVE_OPENCV_FACE
#endif

#include <string.h>
#include <list>
#include <unordered_set>
#include <nan.h>
#include "hit.h"
#include "orbwordindex.h"

using namespace v8;
using namespace node;

#endif
