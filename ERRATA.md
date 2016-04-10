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

## Page 203: erratum in the explanation of step 2 - precalculation of integral image

Near the end of the page, the following line can be found

 > This can take some time according to the size of your model and according to the amount of training samples, especially when    knowing that a model of 24x24 pixels can yield more than 16,000 features.

In this line, the total number of features should be **160,000** instead of 16,000.

Erratum reported by *LorenaGdL* on *April 10th, 2016*.

## Page 200: erratum in the explanation of `-maxFalseAlarmRate` parameter

At the top of the page, the following explanation can be found for this parameter

 > This is the threshold that defines how much of your negative samples need to be classified as negatives before the boosting process should stop adding weak classifiers to the current stage. The default value is 0.5 and ensures that a stage of weak classifier will only do slightly better than random guessing on the negative samples. Increasing this value too much could lead to a single stage that already filters out most of your given windows, resulting in a very slow model at detection time due to the vast amount of features that need to be validated for each window. This will simply remove the large advantage of the concept of early window rejection.

In this explanation 2 things should be changed:

1. The sentence `This is the threshold that defines how much of your negative samples need to be classified as negatives before the boosting process should stop adding weak classifiers to the current stage.` should be changed to `This is the threshold that defines how much of your negative samples are allowed to be classified as positives before the boosting process should stop adding weak classifiers to the current stage.`.
2. Then correspondingly the word `Increasing` should be replaced by `Decreasing`.

Erratum reported by *LorenaGdL* on *April 10th, 2016*.
