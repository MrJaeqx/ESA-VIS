# ESA-VIS Assignment 1

Assignments for the Vision course using OpenCV. 

# Edge detection

Lars, doe je ding.

# Blobs

For the blob assignment, we first tried doing it per line pixel and working line by line. This proved to not work very well, as it was impossible to determine the colors of the pixels starting on another line which are offset more than one pixel from the previous lines pixel. 

We then tried to color the first found pixels and the pixels around that, with a recursive function moving to the next pixel each time. This proved to work well, with each blob appearing separately colored.

 
