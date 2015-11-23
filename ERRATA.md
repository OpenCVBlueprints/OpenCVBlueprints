# OpenCV 3 Blueprints Errata Page
-------------------------------

## Page 208: erratum in the visual of a haar feature

In the image contained inside page 208, visualising the calculation of a HAAR wavelet like feature, there is a mistake in the formula. 

The current formula is stating 
<pre>feature = area(A) + area(C) - area(C)</pre>
while it should be
<pre>feature = area(A) + area(C) - area(<b>B</b>)</pre>
to correctly represent the image above.

Erratum reported by *Can Ergun* on *November 22nd, 2015*.
