//////////////////////////////////////////////////
//  
//   ChModuleMulticore.i
//
//   SWIG configuration file.
//   This is processed by SWIG to create the C::E
//   wrapper for Python.
//
///////////////////////////////////////////////////

// Define the module to be used in Python when typing 
//  'import pychrono.multicore'
%module(directors="1") mcore

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
#include "chrono_multicore/physics/ChSystemMulticore.h"
#include "chrono_multicore/ChSettings.h"
using namespace chrono;
%}

// Undefine ChApi and other macros that otherwise SWIG gives a syntax error
#define CH_MULTICORE_API
#define ChApi 
#define CH_DEPRECATED(msg)

// Include other .i configuration files for SWIG. 
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "cstring.i"
%include "cpointer.i"

#ifdef SWIGPYTHON
%include "std_wstring.i"
%include "wchar.i"
%include "python/cwstring.i"
#endif

// This is to enable references to double,int,etc. types in function parameters
%pointer_class(int,int_ptr);
%pointer_class(double,double_ptr);
%pointer_class(float,float_ptr);
%pointer_class(char,char_ptr);

// A- ENABLE SHARED POINTERS

%shared_ptr(chrono::ChSystemMulticore)
%shared_ptr(chrono::ChSystemMulticoreNSC)
%shared_ptr(chrono::ChSystemMulticoreSMC)

// B- INCLUDE HEADERS

#ifdef SWIGCSHARP
%import "chrono_swig/interface/core/ChClassFactory.i"
%import "chrono_swig/interface/core/ChSystem.i"
%import "chrono_swig/interface/core/ChVector.i"
%import "chrono_swig/interface/core/ChCoordsys.i"
#endif

#ifdef SWIGPYTHON
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChClassFactory.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChSystem.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChVector.i"
%import(module = "pychrono.core")  "chrono_swig/interface/core/ChCoordsys.i"
#endif

%include "../../../chrono_multicore/physics/ChSystemMulticore.h"
%include "../../../chrono_multicore/ChSettings.h"

// C- DOWNCASTING OF SHARED POINTERS

%DefSharedPtrDynamicDowncast(chrono, ChSystem, ChSystemMulticore)
%DefSharedPtrDynamicDowncast(chrono, ChSystemMulticore, ChSystemMulticoreNSC)
%DefSharedPtrDynamicDowncast(chrono, ChSystemMulticore, ChSystemMulticoreSMC)

