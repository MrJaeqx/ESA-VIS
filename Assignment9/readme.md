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

text