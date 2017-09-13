# ESA-VIS Assignment 1

Assignments for the Vision course using OpenCV. 

# Edge detection

To do some edge detection on the provided image we used different methods. We first converted the image to grayscale. The we used three different filters, two Laplacian filters and one gradient filter.  
  
For the Laplacian filters we used the following kernels:
 0|-1| 0
-1| 4|-1
 0|-1| 0
  
 0|-1| 0
-1| 8|-1
 0|-1| 0
  
The gradient filter used the following kernel:
-1| 0| 1
-1| 0| 1
-1| 0| 1

As extra we processed the image with the OpenCV Laplacian filter, just to see the differences with the custom kernels we made and to do some experiments with the parametes.

# Blobs

For the blob assignment, we first tried doing it per line pixel and working line by line. This proved to not work very well, as it was impossible to determine the colors of the pixels starting on another line which are offset more than one pixel from the previous lines pixel. 

We then tried to color the first found pixels and the pixels around that, with a recursive function moving to the next pixel each time. This proved to work well, with each blob appearing separately colored.