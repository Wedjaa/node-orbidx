const fs = require('fs');
const path = require('path');
const OrbIndexer = require('../index.js');

var orbIndexer = new OrbIndexer();

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
