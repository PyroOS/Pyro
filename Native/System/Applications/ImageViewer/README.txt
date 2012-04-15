ImageViewer
========================================
ImageViewer is a fast and slim image viewer for Syllable.
It includes several features to support image viewing such 
as rotating, fullscreen and slideshow.

When ImageViewer is first started it registers itself as a
viewer for all supported image formats so that you can 
click on images in the filebrowser to automatically 
launch ImageViewer. Note that if you already have installed
an imageviewer such as AView this will not work. 

You can start ImageViewer from prompt by typing 
"ImageViewer [filename]" and press enter. You can also 
provide wildcards in the filename, e.g. "ImageViewer *.jpg" 
to view all JPEG-images in the current directory.

You quite the viewer either by closing the window or pressing
Esc.

Icons
-----------------------------------------
When ImageViewer is started you have the image in the center
of the window and at the bottom an iconbar. Horizontal and 
vertical scrollbars will be added automatically if the image 
is too big for the window. The window title shows the name 
of the current image.

The following icons are available (from left to right and 
shortcuts between the brackets)

Open Image (Ctrl+O): Opens a file requester and lets you choose
  which files to open. You can select multiple images
  by keeping shift pressed while clicking on the files.

Previous Image (left arrow or page up): View the previous 
  image. If you are already looking at the first image,
  previous image will show the last image.

Next image (right arrow, space or page down): View the next
  image. If you are at the last image, next image will show
  the first image.

Rotate CCW (q): Rotate the current image 90 degress 
  counterclockwise.

Rotate CW (w): Rotate the current image 90 degress 
  clockwise.

Zoom in (+): Zoom in the image (make the image bigger).

Zoom out (-): Zoom out the image (make the image smaller).

1:1 : Show the image in natural size (ie. one pixel in the 
  image is equal to one pixel on screen)

Fit to window: Make the image always fit the window even if 
  the window is resized.

Zoom slider: Slider to change the zoom scale.

Fullscreen (Backspace): Toggles between fullscreen and 
  windowed view. Note that when in fullscreen the iconbar
  is automatically removed. To view it move the mouse cursor 
  to the bottom of the screen.

Play (p): Start the slideshow

Stop (s): Stop the slideshow

Slideshow slider: Set the time between images, 1-10 seconds.
  Default is 5 seconds.

Set wallpaper: Set the current image as wallpaper for the
  desktop

Delete (del): Deletes the current image.

Mouse
-----------------------------------------
There are some tricks you can do with the mouse if the mouse
cursor is on the image:

- Use the wheelbutton to zoom in or out in the image.
- Press left mousebutton and keep it pressed to move (pan) 
  the image (only works if scrollbars are visible).

Bugs
-----------------------------------------
Probably, send a report to me when you find them!

Changes
-----------------------------------------
< 2008-08-10 
* First release

2008-08-10
* Show a hand-cursor when panning
* Added icon for setting the wallpaper

2008-08-18
* Bug fixed: It was not possible to scroll between images 
  on disk if you selected only one file in the fileselector.

Enjoy!

Copyright 2008 Jonas Jarvoll
Released under GPL
