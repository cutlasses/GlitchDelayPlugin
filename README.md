# GlitchDelayPlugin

This is the Plugin version of an effect orginally written to run on Teensy hardward. It uses the TeensyJuce API, also in the repository.

The effect consists of a standard delay line, or delay buffer, with multiple read heads that each read the audio in a different way.

**Loop heads**
These heads loop small sections of audio. There are 3 of these. One that plays the audio an octave lower, one at the original octave, and one an octave higher. The size of each of these loops can be adjusted (size dial), as can the amount the loops move each time they loop (jitter dial)

**Reverse head** 
This head plays the buffer in reverese at the original octave.

![alt text](https://user-images.githubusercontent.com/18176564/32569049-e84dc710-c4b7-11e7-9e0f-059c36c37f17.png "Plugin on MacOs")



