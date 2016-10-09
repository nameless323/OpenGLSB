#include <sb6.h>

#include "DeferredShading.cpp" // Yep, looks like shit, but framework force to do like this.

class SceneStarter : public sb6::application
{    
};
DECLARE_MAIN(OpenGlSB::DeferredShading);