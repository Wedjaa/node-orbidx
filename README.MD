# Node ORB Image Indexer

## Introduction

This is a naive implementation of the **_Pastec ORB based image indexer_**, the words indexing class has been slightly remodelled, but it's pretty much the same logic _and code_  as the implementation from [Pastec GitHub Repository](https://github.com/Visu4link/pastec).

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

orbIndexer.initialize();
 ```

This piece of code will create  an indexer with all the default settings; that means it will be using the same options for ORB features extraction as __PASTEC__ and the same visual word dictionary.

What makes this a great tool for experimenting with ORB based visual search is that you can pass a set of options that will allow you to use any customized set of options for ORB features extraction and your very own visual words dictionary.

*When using the original PASTEC words dictionary you may see better results by sticking to the PASTEC options for ORB features extraction.*

### Indexer options

See the [API](docs/api.md) for a detailed description of the options that can be passed to the constructor of the indexer.

# More Documentation

[A simple example of usage](docs/example.md)

[Create your own visual words dictionary](docs/dictionary.md)

[API Documentation](docs/api.md)
