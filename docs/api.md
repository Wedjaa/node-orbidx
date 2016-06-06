# API

## Constructor

The constructor  - `OrbIndexer` - has the following signature:

```javascript
OrbIndexer(<options>)
```

Here **`options`** is an options object with one or more of the following properties:

- **nfeatures**: [_default_: **2000**] The maximum number of features (*descriptors* if you speak OpenCV) that will be extracted from an image;

- **scaleFactor**: [_default_: **1.02**] The scale factor to be used to initialize the ORB feature detector. This value determines the resolution for the levels that will be generated for feature extraction, each level will have a resolution reduced by this factor. For a **scaleFactor** of **2**, as per ORB documentation example, each layer of the pyramid will have **4** times less pixels than the previous level;

- **nlevels**: [_default_: **100**] How many pyramid levels to create to detect features. From the ORB documentation:
> The smallest level will have linear size equal to `input_image_linear_size/pow(scaleFactor, nlevels)`

- **edgeThreshold**: [_default_: **15**] This is the border around the images that will be ignored when looking for features, it should have roughly the same value as **patchSize**;

- **firstLevel**: [_default_: **0**] The current ORB implementation in OpenCV expects this value to be always **0**;
- **WTA_K**: [_default_: **2**] Number of points that are used to produce each element of the oriented BRIEF descriptor, check  [OpenCV ORB documentation](http://docs.opencv.org/2.4/modules/features2d/doc/feature_detection_and_description.html#orb-orb) for more details;

- **scoreType**: [_default_: **0**] Type of scoring to be used when ranking the features, supported values are:
  - **0** - _Harris Score_
  - **1** - _Fast Score_


- **patchSize**: [_default_: **15**] The patch size used to evaluate features;

- **fastThreshold**: [_default_: **20**] This value is not used in the current implementation;

- **maxKeypoints**: [_default_: **2000**] Max number of keypoints in the grid detector;

- **gridRows**: [_default_: **1**] If this value or the value of  **gridCols** is greater than **1** the feature detection will use a grid system to make sure that features are detected in different parts of the submitted image. This seems initially a good idea, but try and expriment with it and see if it works with your set of images;

- **gridCols**: [_default_: **1**] See **gridRows**;

- **minFeatureDistance**: [_default_: **30**] This options is used during training to determine which features of a training image will be added to the visual words dictionary. The idea is that only features that have a distance from the already existing features in the dictionary of at least this number will be added to the dictionary;

- **wordIndexPath**: [_default_: Use included PASTEC visual words DB] This option allows you to set the path to a visual words database that you have created using training or you got from some other source.

## Methods

## `initialize`

This method **MUST** be called before executing any other method on the specific instance of OrbIndexer. It will setup the needed objects and prepare for training and visual words extraction.

The method returns a promise that resolves on success to the visual words database file used.

## `indexImage` and `indexImageFile`

These are the signatures of these two methods:

`indexImage(<image_id>, <image_buffer>)`

`indexImageFile(<image_id>, <image_file_path>)`

`indexImage` takes a string as an **image id** and a buffer that contains the binary representation of an image.

`indexImageFile` takes a string as an **image id** and a string as a path to an image file that needs visual words extracted from.

These two methods will return a promise that resolves to a list of visual words.

The array returned by the promise has the following elements:

```javascript
{
  image_id: "<image_id as passed to the method>",
  word_id: "<a numeric representation of the visual word>",
  angle: "the angle of the feature matching the visual word",
  x: "the ratio of the horizontal position of the feature",
  y: "the ratio of the vertical position of the feature"
}
```

**x** and **y** are expressed as a ratio of the image width and height, this allows to calculate the position no matter what the scaled size of the image is.

The actual pixel values can be calculated - as an example - as follows:

```javascript
var x_pix = Math.floor(x * image.width);
var y_pix = Math.floor(y * image.height);
```
## `startTraining`

The method will remove any existing visual words list and initialize the training. This method **returns immediately**.

## `trainImage` and `trainImageFile`
The signature of these two methods is the following:

`trainImage(<image_buffer>)`

`trainImageFile(<image_file_path>)`

This two methods return a promise that will resolve to a message string indicating the effect of the training in terms of detected features and how many of the features are added to the visual words dictionary.


## `saveTrainedIndex`

The signature of this method is:

`saveTrainedIndex(<wordsIndexDBPath>)`

This method returns a promise that resolves to the path to the saved visual words database - the same path that was passed to the function.

## `loadTrainedIndex`

The signature of this method is:

`loadTrainedIndex(<wordsIndexDBPath>)`

This method returns a promise that resolves to the path to the loaded visual words database - the same path that was passed to the function.
