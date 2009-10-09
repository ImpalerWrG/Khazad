# main project file. use it from a build sub-folder, see COMPILE for details
PROJECT (khazad) 
cmake_minimum_required(VERSION 2.6)

# disable warning, autosearch
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

IF(NOT DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

SET(SO_MAJOR_VERSION "0")
SET(SO_MINOR_VERSION "0")
SET(SO_BUILD_VERSION "1")
SET(SO_VERSION "${SO_MAJOR_VERSION}.${SO_MINOR_VERSION}.${SO_BUILD_VERSION}")

#SET( EXECUTABLE_OUTPUT_PATH ${khazad_SOURCE_DIR}/output CACHE PATH "Output directory for khazad" )
SET( EXECUTABLE_OUTPUT_PATH ${khazad_SOURCE_DIR}/output CACHE PATH "Output directory for khazad" )

include_directories (
${CMAKE_SOURCE_DIR}/include/
${CMAKE_SOURCE_DIR}/dfhack/library/
${CMAKE_SOURCE_DIR}/dfhack/library/md5/
${CMAKE_SOURCE_DIR}/src/Core
${CMAKE_SOURCE_DIR}/src/Data
${CMAKE_SOURCE_DIR}/src/Game
${CMAKE_SOURCE_DIR}/src/Geometry
${CMAKE_SOURCE_DIR}/src/Gui
${CMAKE_SOURCE_DIR}/src/Gui/CustomWidgets
${CMAKE_SOURCE_DIR}/src/Map
${CMAKE_SOURCE_DIR}/src/Render
${CMAKE_SOURCE_DIR}/guichan/include
# dynamic detect path to those two?
/usr/include/IL/
/usr/include/SDL/
${CMAKE_SOURCE_DIR}/TinyXML

)

SET(PROJECT_SRCS
TinyXML/tinystr.cpp
TinyXML/tinyxml.cpp
TinyXML/tinyxmlerror.cpp
TinyXML/tinyxmlparser.cpp

dfhack/library/DFDataModel.cpp
dfhack/library/DFHackAPI.cpp
dfhack/library/DFMemInfo.cpp
dfhack/library/DFProcess.cpp
dfhack/library/DFProcessManager.cpp
dfhack/library/DFTileTypes.cpp
dfhack/library/md5/md5.cpp
dfhack/library/md5/md5wrapper.cpp

guichan/src/actionevent.cpp
guichan/src/basiccontainer.cpp
guichan/src/cliprectangle.cpp
guichan/src/color.cpp
guichan/src/defaultfont.cpp
guichan/src/event.cpp
guichan/src/exception.cpp
guichan/src/focushandler.cpp
guichan/src/font.cpp
guichan/src/genericinput.cpp
guichan/src/graphics.cpp
guichan/src/gui.cpp
guichan/src/guichan.cpp
guichan/src/image.cpp
guichan/src/imagefont.cpp
guichan/src/inputevent.cpp
guichan/src/key.cpp
guichan/src/keyevent.cpp
guichan/src/keyinput.cpp
guichan/src/mouseevent.cpp
guichan/src/mouseinput.cpp
guichan/src/opengl/opengl.cpp
guichan/src/opengl/openglgraphics.cpp
guichan/src/opengl/openglimage.cpp
guichan/src/rectangle.cpp
guichan/src/sdl/sdl.cpp
guichan/src/sdl/sdlgraphics.cpp
guichan/src/sdl/sdlimage.cpp
guichan/src/sdl/sdlimageloader.cpp
guichan/src/sdl/sdlinput.cpp
guichan/src/selectionevent.cpp
guichan/src/widget.cpp
guichan/src/widgets/button.cpp
guichan/src/widgets/checkbox.cpp
guichan/src/widgets/container.cpp
guichan/src/widgets/dropdown.cpp
guichan/src/widgets/icon.cpp
guichan/src/widgets/imagebutton.cpp
guichan/src/widgets/label.cpp
guichan/src/widgets/listbox.cpp
guichan/src/widgets/radiobutton.cpp
guichan/src/widgets/scrollarea.cpp
guichan/src/widgets/slider.cpp
guichan/src/widgets/tab.cpp
guichan/src/widgets/tabbedarea.cpp
guichan/src/widgets/textbox.cpp
guichan/src/widgets/textfield.cpp
guichan/src/widgets/window.cpp

src/Core/Random.cpp
src/Core/Timer.cpp
src/Core/main.cpp

src/Data/ConfigManager.cpp
src/Data/DataManager.cpp
src/Data/DataTypes.cpp
src/Data/Paths.cpp
src/Data/XMLManager.cpp

src/Game/Actor.cpp
src/Game/Game.cpp
src/Game/Pawn.cpp

src/Geometry/Plane.cpp
src/Geometry/Vector2.cpp
src/Geometry/Vector3.cpp

src/Gui/FontManager.cpp
src/Gui/Gui.cpp
src/Gui/InputManager.cpp
src/Gui/CustomWidgets/KhazSlider.cpp
src/Gui/CustomWidgets/toggleimagebutton.cpp
src/Gui/CustomWidgets/KhazWindow.cpp
src/Gui/CustomWidgets/KhazImageButton.cpp

src/Map/Building.cpp
src/Map/Cell.cpp
src/Map/Column.cpp
src/Map/Cube.cpp
src/Map/Map.cpp
src/Map/Tree.cpp

src/Render/Camera.cpp
src/Render/ClipImage.cpp
src/Render/ColorManager.cpp
src/Render/ImageManager.cpp
src/Render/ImagePage.cpp
src/Render/ModelManager.cpp
src/Render/Renderer.cpp
src/Render/TextureManager.cpp
src/Render/TreeManager.cpp
)

IF(UNIX)
add_definitions(-DLINUX_BUILD)
add_definitions(-DILUT_USE_OPENGL)
add_definitions(-DBUILD_DFHACK_LIB)
SET(PROJECT_LIBS SDL SDLmain SDL_ttf SDL_image SDL_mixer SDL_net GL GLU glut IL ILU ILUT z )
ELSE(UNIX)
add_definitions(-DBUILD_DFHACK_LIB)
add_definitions(-DILUT_USE_OPENGL)
SET(PROJECT_LIBS
psapi
)
ENDIF(UNIX)

ADD_EXECUTABLE(khazad ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(khazad ${PROJECT_LIBS})