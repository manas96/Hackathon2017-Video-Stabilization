# Hackathon2017-Video-Stabilization

# To compile source:

OpenCV and g++ is needed (on Ubuntu).

Run script file "run.sh" with filename as argument(bash run.sh xyz.cpp). It will produce binary named "app".

Run binary with sample video filename as argument(./app sample.mp4).

Executable binaries are in /bin.

"stabilize.cpp" processes input video, then displays input and output side by side.

"realTime.cpp" displays video as soon as a frame is processed.

# To run realTimeStabilize.cpp:

This file displays 1)Real time video feed from an android device and 2)Stabilized video in real time.

To run, first download IPWebcam (https://play.google.com/store/apps/details?id=com.pas.webcam&hl=en) on your Android device.

Ensure your PC and phone are on the same wifi network.

Open IPWebcam, scroll down and press "Start server". You will see video on your device along with a URL.

Open realTimeStabilize.cpp, goto line 92 and replace the IP/port with the URL displayed in IPWebcam. 

Build the cpp file and run. Takes some time to startup.
