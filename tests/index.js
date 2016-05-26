const fs = require('fs');
const path = require('path');
const OrbIndexer = require('../index.js');

var orbIndexer = new OrbIndexer();

var testImage = path.resolve(path.join(__dirname, 'image/windmill-1.jpg'));

orbIndexer.initialize()
  .then(function(wordIndexPath) {
      console.log('Initialized: ' + wordIndexPath);
      return orbIndexer.indexImageFile('sample', testImage);
  })
  .then(function(imageWords) {
      console.log('Image Words - From file: \n' + JSON.stringify(imageWords, null, 2));
      return orbIndexer.indexImage('sample', new Buffer(fs.readFileSync(testImage)));
  })
  .then(function(imageWords) {
    console.log('Image Words - From file: \n' + JSON.stringify(imageWords, null, 2));
  })
  .catch(function(error) {
    console.log('Error: ' + error.message);
  });

/*
function loadImage(imageName) {
  return new Buffer(fs.readFileSync(imageName));
}

var imageBuffer = loadImage(path.join(__dirname, 'image/windmill-1.jpg'));

orbIndexer.initWordIndex(function (err, res) {

  if (err) {
    console.log('Error!')
    console.log(err);
    return;
  }
  console.log(res);

  orbIndexer.indexImage(function (err, image_hits) {
    if (err) {
      console.log('Error: ' + err);
      return;
    }

    console.log(JSON.stringify(image_hits, null,2));
    console.log('Done');
  }, 'sample_image', imageBuffer, imageBuffer.length);
}, 'visualWordsORB.dat');
*/
