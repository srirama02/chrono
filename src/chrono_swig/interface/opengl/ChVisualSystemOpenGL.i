%{
#include "chrono_opengl/ChVisualSystemOpenGL.h"

using namespace chrono;
using namespace chrono::opengl;
%}

%shared_ptr(chrono::opengl::ChVisualSystemOpenGL)

%import(module = "pychrono.core") "chrono_swig/interface/core/ChVisualSystem.i"
   
%include "../../../chrono_opengl/ChVisualSystemOpenGL.h"

%DefSharedPtrDynamicDowncast2NS(chrono, chrono::opengl, ChVisualSystem, ChVisualSystemOpenGL)
