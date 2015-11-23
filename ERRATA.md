# OpenCV 3 Blueprints Errata Page
-------------------------------

## Page 208: erratum in the visual of a haar feature

In the image contained inside page 208, visualising the calculation of a HAAR wavelet like feature, there is a mistake in the formula. The current formula is stating `feature = area(A) + area(C) - area(C)` while it should `be feature = area(A) + area(C) - area(**B**)`.

Erratum reported by *Can Ergun* on *November 22nd, 2015*.
