# LIRS World Construction Tool - Automatic tool for gazebo world construction: from a grayscale image to a 3d solid model
This utility provides automatic converting to DAE file format from PNG/JPG grayscale image

Full information about the utility with test results is published in the article:

B. Abbyasov, R. Lavrenov, A. Zakiev, K. Yakovlev, M. Svinin, and E. Magid, “Automatic tool for gazebo world construction: from a grayscale image to a 3d solid model”, International Conference on Robotics and Automation (ICRA), 2020, pp. 7226-7232.

Cite this article if you use the LIRS-WCT utility

This project fork from [gitlab](https://gitlab.com/LIRS_Projects/LIRS-WCT).

# Installation
Clone the repository:
```sh
git clone https://gitlab.com/LIRS_Projects/LIRS-WCT lirs-wct
```
Run the script:
```sh
./deploy.sh
```
Compilation is done.
Enter to a ``` release ``` directory:
```sh
cd release
```
It opens the ``` release ``` directory.
The ``` release ``` folder contains both the console (```lirs_wct_console```) and the GUI (```lirs_wct_gui```) versions of the utility.  
Running of LIRS-WCT is straightforward and can be accomplished with two commands depending on the type of interaction interface:
#### GUI version:
```sh
./lirs_wct_gui
```
-----------------
#### Command-Line version:
```sh
./lirs_wct_console
```
# Usage
## GUI version
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/LIRS-WCTv2.png?raw=true?raw=true "LIRS-WCT GUI")

Run the GUI version:
```sh
./lirs_wct_gui
```

## Command-Line version
### Parameters

| Parameter  | Description |
| ------------- | ------------- |
| -h, –help  | Help screen  |
| -i, –input-filepath=  | Set an absolute path to a source grayscale image (required)  |
| -o, –output-filename=  | Set a filename for a generated mesh model (required)  |
| -f, –output-folder=    | Set a folder for output files (required)  |
| -t, –texture  | Set texture for generated mesh model (required)  |
| -p, –png-to-stl  | Convert a png file to stl model (optional)  |
| -k, –png-to-dae  | Convert a grayscale png image to dae model (optional-default)  |
| -s, –stl-to-dae  | Convert a stl file model to dae model (optional)  |
| -c, –color-inverse  |Using a preliminary image color inverse (optional)  |
| -z, –z-scale=  | Set z-axis scale factor (affects output model height) (optional)  |
| -g, –grayscale=  |  Convert an arbitrary image to grayscale (optional)  |
| -l, –scale= |  Set xyz-axis scale factor (affects output model dimensions) (optional)  |
| -x, –x-rotate=  | Set x-axis rotate angle (default value is -90 degrees)  |

## Testing

In the ```example``` folder you can find some materials for the model making.

#### PNG to DAE
##### Convert the PNG file to the DAE 3D-model
```
./lirs_wct_console -i ../example/map4.png -o map4 -f ../test4 -t ../example/texture3.jpg -k -z 0.15 -l 0.05
```
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/game.png?raw=true "The game")

#### PNG to STL
##### Convert the PNG file to the STL 3D-model. Note that the STL file does not support texture applying. Instead you could use the DAE model
```sh
./lirs_wct_console -i ../example/map1.png -o map1 -f ../test2 -p -z 0.15
```
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/labyrinth_stl.png?raw=true "The naked labyrinth")

#### STL to DAE
##### Convert the the STL 3D-model to the DAE 3D-model
```sh
./lirs_wct_console -i ../test2/map1.stl -o map3 -f ../test3 -t ../example/texture8.jpg -s -z 0.15 -l 0.05
```
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/textured_labyrinth_dae.png?raw=true "The textured labyrinth")

The generated WORLD file can be loaded into Gazebo by the following command:
```sh
gazebo generated_world.world
```
# Examples
## The maze
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/maze.png?raw=true "The maze")
## The arctic terrain
![Alt text](https://gitlab.com/LIRS_Projects/LIRS-WCT/raw/master/example/screenshots/arctic_terrain.png?raw=true "The arctic terrain")

# [Wiki](https://gitlab.com/LIRS_Projects/LIRS-WCT/-/wikis/home)