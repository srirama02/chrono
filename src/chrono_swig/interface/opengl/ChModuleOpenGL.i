//////////////////////////////////////////////////
//  
//   ChModuleOpenGL.i
//
//   SWIG configuration file.
//   This is processed by SWIG to create the C::E
//   wrapper for Python.
//
///////////////////////////////////////////////////

// Define the module to be used in Python when typing 
//  'import pychrono.opengl'
%module(directors="1") opengl

// Turn on the documentation of members, for more intuitive IDE typing
%feature("autodoc", "1");
%feature("flatnested", "1");

// Turn on the exception handling to intercept C++ exceptions
%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

// For optional downcasting of polimorphic objects:
%include "../chrono_downcast.i" 

// For supporting shared pointers:
%include <std_shared_ptr.i>


// Include C++ headers
%{
#include "chrono_opengl/ChOpenGLViewer.h"
#include "chrono_opengl/ChVisualSystemOpenGL.h"
using namespace chrono;
using namespace chrono::opengl;
%}

// Undefine ChApi and other macros that otherwise SWIG gives a syntax error
#define CH_OPENGL_API
#define ChApi 
#define CH_DEPRECATED(msg)

// Include other .i configuration files for SWIG. 
%include "std_string.i"
%include "std_wstring.i"
%include "std_vector.i"
%include "typemaps.i"
%include "wchar.i"
%include "python/cwstring.i"
%include "cstring.i"
%include "cpointer.i"

// This is to enable references to double,int,etc. types in function parameters
%pointer_class(int,int_ptr);
%pointer_class(double,double_ptr);
%pointer_class(float,float_ptr);
%pointer_class(char,char_ptr);


// A- ENABLE SHARED POINTERS

%shared_ptr(chrono::opengl::ChVisualSystemOpenGL)

// B- INCLUDE HEADERS

%import(module = "pychrono.core")  "chrono_swig/interface/core/ChClassFactory.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChSystem.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChVector.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChCoordsys.i"

%import(module = "pychrono.core") "chrono_swig/interface/core/ChVisualSystem.i"
%import(module = "pychrono.core") "chrono_swig/interface/core/ChColor.i"

%include "../../../chrono_opengl/ChOpenGLViewer.h"
%include "../../../chrono_opengl/ChVisualSystemOpenGL.h"

// C- DOWNCASTING OF SHARED POINTERS

%DefSharedPtrDynamicDowncast2NS(chrono, chrono::opengl, ChVisualSystem, ChVisualSystemOpenGL)

