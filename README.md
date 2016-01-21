Description
===========
My go at the interesting Samsung interview task I have found on the internet recently

The task is desribed in the task.png (in Russian)

### TL;DR; & for non Russian speakers:

Given the image of vinyl plate write the program which decodes it into the sound.


Compillation and running
========================

1. install libsdl2 dependencies:  
        `apt-get install libsdl2-dev libsdl2-image-dev`
2. compile and run:  
        `make && ./vinyldc vinyl.png`
3. play decoded file (command line vlc player is used in this example):  
        `cvlc vinyl_decoded.wav`

Algorithm description
=====================

*Mind that we are using display coordinates which are always positive and start at (0,0) in the top left screen corner*

1. Start in the image top center `(x,y) = (Screen width / 2, 0)`
2. Move down by incrementing Y untill we find pixel with the value 'brighter' than certain grey shade threshold value
  1. If we have reached the center without finding any bright pixel, stop processing the image else, save the position of the first 'bright' pixel we have found into the `track_start`
3. Rotate `track_start` counterclockwise and save rotation result into `pnew`. Whenever it is light enough, write the sample into the wav file and proceed with rotation. If `pnew` is not light enough, there is a couple of possibilities of us having:  
  1. strayed off the track into the dark zone between the tracks (most frequent case):  
    1. in this case we twiggle start point 2 pixels up and rotate -> this will result into rotated point to be a bit more far away form the center and might move us back on track. If it didn't and we are still on the dark spot:
    2. twiggle start point 2 pixels down and rotate -> this will move our pixel a bit closer to the center. If it didn't, proceed to the next case:  
  2. have reached the end of the track (dark circle in the middle of the plate):  
    1. move 30 pixels towards the plate center - if all of the 30 sampled pixels are dark, assume that we have reached the end of the plate and stop processing. If it is not the end, proceed to the next case:  
  3. encountered a very dark bump on the track and the algoritm considered it to be 'off track' value. Add the value  
to the sample file as if it was light enough  
4. After we have reached the end of the plate, prepend the wav header with the amount of samples we have written to the beginning of the sample file.

