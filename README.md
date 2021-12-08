# ARGoS3 Pi-Puck Extended

## Summary
This repository demonstrates how to create an alternative configuration of the Pi-Puck in the ARGoS simulator. The alternative configuration is achieved by abusing the ARGoS loop functions and Qt-OpenGL user functions to register an new robot called `pipuck_ext`. The configuration includes an alternative dynamics3d model, an alternative visualisation, and an alternative entity that contains additional components (a tag mounted on the top in this case).

### Compilation
As with the rest of ARGoS, compilation uses CMake. The following files are created during configuration and compilation:
| Output                                             | Description                                                                                                                                                               |
|----------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `libmy_extensions.so` (or dylib on MacOS)          | Contains the loop functions, pipuck_ext entity, [debug actuator/entity](#the-debug-actuator-and-entity), and dynamics3d model. Should be loaded in the `<loop_functions>` section of the configuration file |
| `libmy_qtopengl_extensions.so` (or dylib on MacOS) | Contains the qtopengl user functions and the visualisation of the pipuck_ext entity. Should be loaded in the `<user_functions>` section under the `<qt-opengl>` tag.      |
| `controller.lua`                                   | Example Lua controller demonstrating the driving forwards, LEDs, and the debug actuator                                                                                   |
| `configuration.argos`                                | Example ARGoS configuration file that is populated with absolute paths to the files above (can be run with `argos3 -c configuration.argos`)                               |

The following commands clone this repository and build/run the plugins/example configuration.
```sh
# clone this repository
git clone https://github.com/iridia-ulb/argos3-pipuck-ext.git
# compile
cd argos3-pipuck-ext
mkdir build
cd build
cmake ../src
make
# run
argos3 -c configuration.argos
```
## The debug actuator and entity
When loaded, these plugins provide a debug actuator that can be added to any robot via the `<actuators>` sections in a robot controller's configuration. By adding this actuator, a debug entity is automatically created and added into the robot. Using the Lua interface of a robot, it is possible to draw arrows, rings, and write messages that will be appended to the loop function output for each entity (see controller.lua)

## Compatibility with ARGoS
As there is an alternative OBJ/MLT model embedded into `libmy_qtopengl_extensions.so`, it is necessary to use a recent version of ARGoS. As of writing, this means at least revision [2ca77a3](https://github.com/ilpincy/argos3/tree/2ca77a3eed70d815142ab8efbe87e5bd39e0851f).

