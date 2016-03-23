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

## Page 189: multiple errata describing the annotation tool

As reported to me by a buyer of the book page 189 contains several errata which should be changed, in order to make the explanation more cohesive.

* Before the line `Using the software is quite straightforward:`, there is the mentioning of the opencv integrated version of the tool. However if OpenCV is installed correctly it is a system tool and thus there needs to be no slash before the executable name. The line `/opencv_annotation -images <folder location> -annotations <output file>` should thus be replaced by `opencv_annotation -images <folder location> -annotations <output file>`.

* Under number 1 at the bottom of the page there is the `cmakemake` command which is wrong. It should be replaced by 

```
cmake CMakeLists.txt
make
```

* Finally the at number 1 there is already the annotation command, which needs files that are being generated in later fases. Therefore the line could be removed in order to keep it more straightforward, since the command is returning at bullet 5.

Erratum reported by *John Allen* on *March 19th, 2016*.
