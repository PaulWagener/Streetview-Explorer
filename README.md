Streetview-Explorer
===================

Instead of just viewing the StreetView images you can walk around in them! Image and depth data is automatically downloaded from the official Google Street View servers.

If you've got any feedback, questions or thanks you can leave them [http://code.google.com/p/streetview-explorer/wiki/Feedback here].

If you want to dive right in [http://code.google.com/p/streetview-explorer/downloads/list download the software]

Or if you want to see what this is at all [http://www.youtube.com/watch?v=EEN4DxrkXrM watch the video]

If you have been getting DLL errors in Windows please redownload the file, this problem has been fixed.

The mac & linux version are broken unless you have a very specific set of libraries installed. I'm working on it, please stand by. If you have experience with static linking against libcurl and other libraries don't hesitate to contact me. *Update*: Problem has been fixed, please redownload if you had this problem.

*Version 1.0.1* Fixes some bugs, it should now also work on older graphics cards. If you are getting all white textures you don't have enough texture memory, get with the times!

==Options==

All settings are fastest/worst to slowest/best in the left-to-right direction.

*Vertical accuracy*
Option to eliminate the banding effect when textures do not line up. Increases or decreases the number of polygons in the vertical direction. Default: 5

*Horizontal accuracy*
Increases the number of polygons in the horizontal direction. Lower settings can increase speed but can also make the depth of smaller objects indistinguishable from the background. Default: 2

*Detail level*
Sets the resolution at which to download images from the Google server. Each additional level takes 4 times as long to download and takes up 4 times as much disk space in cache. Level 1 is ideal for just racing around town while Level 3 can be used to study all the details. Default: 2

*Mipmapping*
Enables texture smoothing of far away polygons or polygons looked at from a grazing angle but also dramatically increases texture load times resulting in an annoying wait every time a new panorama comes into view. Default: off

*Wireframe*
Does exactly what you think it does. Can be used to get an estimate of the amount of polygons being pushed, great way to visualize the effects of the accuracy settings. Default: off

==Compilation==
The source can be obtained by following the instructions in the 'checkout' tab. The 'debug' compilation mode might not work, however 'Release' should result in a working executable.

===Windows===
Just open up the Visual Studio project in the windows folder and hit that play button! All libraries are already included.
(only tested on Visual Studio Express 2010 on Windows 7).

===OS X / Linux===
In the mac / linux folder there are NetBeans projects included. In order for the compile to work you need the following libraries installed: wxwidgets 2.8 or 2.9, libjpeg, zlib, libcurl and for linux also glew.

==License==
All the code is licensed in the most free way possible and can be used / adjusted / played around with / redistributed to your hearts contents. For the specifics please refer to the [http://sam.zoy.org/wtfpl/ WTFPL].

==Author==
My name is Paul and I'm a college student in the Netherlands. I am not affiliated with Google in any way. This application is a tech demo demonstrating that Google's Street View data contains enough depth information to create a virtual environment. I hope this application pressures Google into integrating 3d features like this into their Maps products, especially since they are sitting on depth data that is much better and higher resolution than my application has access to.

== To Google's crackteam of lawyers: ==

I am aware that this is very much in violation of the Street View Terms of Service. But before you crack down on me please consider the following:

  * This is all fun and games and will always remain the toy that it is. In no way or shape will I ever make it commercial or use it to generate money.
  * It is very conservative of your bandwidth, all data and images are downloaded only once. It only downloads images that are relevant.
  * Data and images are cached in a non-standard format and it would be very impractical for somebody to adjust it for mass downloading of your imagery.
  * It is hosted only on your own Google Code service. If you feel the need to take this off the internet I hereby give you the right to do so. I shall make no attempt to re-upload it. Existing copies can be blocked by banning the user-agent 'StreetView Explorer'.
