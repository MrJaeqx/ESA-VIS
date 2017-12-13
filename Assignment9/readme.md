# Assignment 9

Lars Jaeqx, Minh-Triet Diep

## Undistort image

To use the camera for measuring in OpenCV we have to calibrate the camera. We can do this using the chessboard image. In order to achieve this we took 20 photos of the chessboard using the webcam.

We use the OpenCV tutorials to create a camera matrix. To make use of this tutorials we have to compile OpenCV with the tutorials included. So we have to set the `BUILD_SAMPLES=ON` parameter in `CMakeLists.txt`.

Now we can compile OpenCV:
```sh
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
```

Next we create a XML file of the images we are using for the calibration. In the folder where you stored the images you run the next command.
```sh
~/opencv-3.3.0/build/bin/cpp-example-imagelist_creator images.xml *.jpg
```

To create the camera matrix we run the next command. This will create a YAML file where the camera specific information is stored.
```sh
~/opencv-3.3.0/build/bin/cpp-example-calibration -w 9 -h 6 images.xml

```

Now we have all the information to undistort the image which will remove the lens distortion. 

## Measure distance

We can use the generated camera information from the last step to calculate distance of the checkerboard pattern. We load the data from the YAML file which we'll use later. Additionally we define the distance between some corners to use later. In this case we used 8 boxes on the pattern, which are 210mm. We also find the focal length of the camera, which is given by documentation of the camera. In our case with the Logitech C920, it's 3.67mm.

First, the frame we want to measure is undistorted. Then using the `cv::findChessboardCorners` function we can get the points of the chessboard. With `cv::minAreaRect` we can find a rectangle, and with that we can get the width. Using the camera resolution, focal length and real size we can figure out the distance.

We have two parts, one which just resolves the distance for one single image and one which takes frames from a camera. 

Here's the result ([YouTube](https://youtu.be/FZD16QykA6w)).

