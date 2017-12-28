# Surveillance-Enhanced-Collision-Avoidance
# Abstract
Recently, more and more technology companies are interested in developing smart, autonomous cars. Using sensors installed around the car body, an autonomous car can get information from its surroundings and drive itself. This method works well on highways and roads since the car follows the flow of traffic, and generally, the sensor can gather this information under many environmental conditions. However, when crossing the intersection, the situation becomes more complicated. Vehicle sensors might be blocked by other cars and objects and may fail to get information of cars moving in the other direction. These scenarios can be dangerous for autonomous cars and cause accidents. Our group tries to apply a supplementary measure based on the surveillance cameras, which are widely used at intersections, to predict collisions and send information to the cars to compensate for sensors dead zones and avoid accidents. The experiment is applied under a laboratory simulation scenario.   
# Description 
At the beginning, two remote controlled cars will be put into experiment field and run freely. One lab camera will simulate the surveillance camera whose field of view will cover the whole experiment field. The DSP board will receive the video from lab camera and does background subtraction to track two cars. For transmission purposes, these two cars will be classified by different colors and the colors are carefully chosen based on experimentation and MATLAB tests to make them more distinguishable.  

After tracking the cars, their movement will be predicted based on their current location and previous location. In our case, we predicted 4 frames forward which we found works best.   

When there is a high likelihood of a collision, the DSP board will send messages to both cars through XBee communications boards. One car will stop for 650 milliseconds while the other car will take different actions based on each situation. When either of the cars reaches the boundary of the camera’s viewable area, it will make a u-turn to keep itself inside the area, so that we can demonstrate this in a confined area.
