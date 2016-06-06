# A practical example using elasticsearch

In this example we will use **elasticsearch** to index and search for images. All the calculations for term frequency will be handled transparently for us - so we need not to worry about any of that.

## Installing the libraries

We are installing the following:
- __node-orbidx__: for extracting the image features;
- __elasticsearch__: to access elasticsearch;
- __bluebird__: my favorite Promise implementation;

```bash
npm install node-orbidx elasticsearch bluebird
```

## Indexing our first image in ES

Lets create the helpers we will use in this example, this code expects *elasticsearch* to be running on the **localhost** and listening to the default HTTP port of **9200**. We will also be using the default visual words dictionary from **PASTEC**:

```javascript
const OrbIndexer = require('node-orbidx')
const elasticsearch = require('elasticsearch')

var esClient = new elasticsearch.Client({
  host: 'localhost:9200',
  log: 'info'
});

var orbIndexer = new OrbIndexer()
```

We first want to initialize the indexer - this needs to be done before using the indexer:

```javascript
orbIndexer.initialize()
```

Now it's time to index an image - we will pass a path to the image:

```javascript
orbIndexer.indexImageFile('sample', 'sample-image.jpg')
  .then(function(words) {
      // Look for this implementation in the
      // next paragraph.
      return esIndexImage(words);
  })
  .catch(function(error) {
    console.log('Failed to index image: ', error.message)
  })
```

We need to create a document we can index in **elasticsearch** to later do some searching. This function takes the visual words extracted from ORB and creates and indexes a document **elasticsearch**:

```javascript
function esIndexImage(words) {
  return new Promise(function(resolve, reject) {
    if (words && words.length>0) {
      // Extract the words from the result
      var imageWords = words.map(word => {
        word.word_id
      })
      // Create the document. The 'image_id' attribute
      // of each word contains the first parameter we
      // passes to the orbIndexer index function, 'sample'
      // in our case
      var imageDoc = {
        words: imageWords,
        image: words[0].image_id
      }

      esClient.create({
        refresh: true, // Make this document available immediately

        index: 'orb', // Index will be created if it does not
        type: 'orb'   // exist already
      })
      .then(function(result) {
        resolve(result);
      })
      .catch(function(error) {
        reject(error);
      })
    }
    // Nothing to index, return empty object
    resolve({});
  })
}
```
Ok - we now have indexed an image in *elasticsearch*, but how do we look it up? Well - the process is almost the same:

```javascript
orbIndexer.indexImageFile('sample', 'sample-image.jpg')
  .then(function(words) {
      // Look for this implementation in the
      // next paragraph.
      return esSearchImage(words);
  })
  .catch(function(error) {
    console.log('Failed to search for image: ', error.message)
  })
```
Instead of creating a new document we will use the same features  to create a search query and get a set of scored results.

```javascript
function esSearchImage(words) {
  return new Promise(function(resolve, reject) {
    if (words && words.length>0) {
      // Create an array of **term** queries
      // from the visual words extracted from
      // the image
      var searchTerms = words.map(word => {
        return {
          term: {
            words: word.word_id
          }
        }
      })

      // Simple **bool** query, scores will be calculated by
      // elasticsearch taking into consideration the terms that
      // matched and the frequency of each matched term in all the
      // indexed images.
      var imageQuery = {
        bool: {
          should: searchTerms
        }
      };

      esClient.search({
        size: 10, // Return maximum 10 matching images
        body: {
            query: imageQuery
        }
      })
      .then(function(result) {
        // Return the hits that matched
        resolve(result.hits);
      })
      .catch(function(error) {
        reject(error);
      })
    }
    // Nothing to index, return empty object
    resolve({});
  })
}
```

## Your turn

In this example I have shown how to index a single image, so the search results
are not quite impressive. Use the same code to index multiple images and see
what your search results look like.

