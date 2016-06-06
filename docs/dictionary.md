# Creating a dictionary from your own images

The sequence to create your own dictionary is pretty simple:

```javascript

const OrbIndexer = require('node-orbidx')

// Create an instance of the indexer and
// initialize it...
var orbIndexer = new OrbIndexer()
orbIndexer.initialize()

// Tell the indexer we are starting the training
orbIndexer.startTraining()

// Now for each image you want to train for:
orbIndexer.trainImageFile('image.jpg')
  .then(function(result) {
    console.log('Image added to training set')
  })
  .catch(function(error) {
    console.log('Error training with image: ', error.message);
  })

// Once you're done:
orbIndexer.saveTrainedIndex('trained-words.dat')
  .then(function(result) {
    console.log('Image Dictionary Saved!')
  })
  .catch(function(error) {
    console.log('Failed to save training results: ', error.message)
  })
```
You can use the dictionary you created in two ways, first you can use it at
initialization time:

```javascript
var orbIndexer = new OrbIndexer({
  wordIndexPath: 'trained-words.dat' // Use this file as visual words dictionary
})
```

Secondly you can load it when you want:

```javascript
orbIndexer.loadTrainedIndex('trained-words.dat')
  .then(function() {
    console.log('Custom word dictionary loaded')
  })
  .catch(function(error) {
    console.log('Failed to load custom dictionary: ', error.message)
  })
```
