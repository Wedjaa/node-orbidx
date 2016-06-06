# Node ORB Image Indexer

## Introduction

This is a naive implementation of the **_Pastec ORB based image indexer_**, the words indexing class has been slightly remodelled, but it's pretty much the same logic _and code_  as the implementation from [Pastec GitHub Repository](https://github.com/Visu4link/pastec:Github).

What this implementation is missing is the database/search part - this is because we believe that there are many other backends that can be used - and easily scaled, once the need hits you - without having to implement one from scratch.

_Currently this code is more usable for research and experimenting purposes rather than real life usage in a production environment_; **_don't say you have not been warned!_**

## A bit of theory

The indexer will take the image you have given to it and create a list of *__Visual Words__* that describe that image. In this implementation this list of words is nothing more than a list of IDs of words as listed in a *__Dictionary of Visual Words__*. You can then use this list of words either for indexing your image in your favorite DB or to search the DB for a similar image.

The way the list of words is generated is by first extracting the features of the image that has been passed and then by looking in the *dictionary* for features that are similar to the ones we extracted from the image.

You have two options in terms of what dictionary to use:

- Use the pre-trained visual words that comes with PASTEC;

- Create your own dictionary by using our very simple trainer and feeding it your images to create the dictionary.

Our implementation of the trainer is (also) pretty naive - and may not scale well, especially in terms of performance, as your image dataset grows.

What the trainer does is extract features from each image and then add to the dictionary of visual words only those features that are not within a specific distance from existing words.

 ## And now for some practical examples

 _**"Code speaks louder than words!"**_, *Mark Twain, Circa 1972*

### Installing

Installing this node module requires that you have already installed _**OpenCV**_ on your system; this has been tested with *__OpenCV version 2.4.x__*.

```sh
npm install node-orbidx
```

### Initializing the indexer

 ```javascript
const OrbIndexer = require('node-orbidx');

var orbIndexer = new OrbIndexer();
 ```

This piece of code will create  an indexer with all the default settings; that means it will be using the same options for ORB features extraction as __PASTEC__ and the same visual word dictionary.

What makes this a great tool for experimenting with ORB based visual search is that you can pass a set of options that will allow you to use any customized set of options for ORB features extraction and your very own visual words dictionary.

*When using the original PASTEC words dictionary you may see better results by sticking to the PASTEC options for ORB features extraction.*

### Indexer options

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

# More Documentation

[A simple example of usage](docs/example.md)
[Create your own visual words dictionary](docs/dictionary.md)
