const fs = require('fs');
const path = require('path');
const OrbIndexer = require('../index.js');
const cluster = require('cluster');
const numWorkers = 1;

if (cluster.isMaster) {
  console.log('Starting %d workers', numWorkers)
  for (var i = 0; i < numWorkers; i++) {
    cluster.fork()
  }

  cluster.on('exit', (worker, code, signal) => {
    console.log(`Worker with PID ${worker.process.pid} exited.`)
  })
} else {
  var orbIndexer = new OrbIndexer({
    minFeatureDistance: 20
  });

  var testImage1 = path.resolve(path.join(__dirname, 'image/test-image-1.jpg'));
  var testImage2 = path.resolve(path.join(__dirname, 'image/test-image-2.jpg'));
  var testImage3 = path.resolve(path.join(__dirname, 'image/test-image-3.jpg'));
  var testImage4 = path.resolve(path.join(__dirname, 'image/test-image-4.jpg'));
  var testImage5 = path.resolve(path.join(__dirname, 'image/test-image-5.jpg'));
  var testImage6 = path.resolve(path.join(__dirname, 'image/test-image-6.jpg'));
  var testImage7 = path.resolve(path.join(__dirname, 'image/test-image-7.jpg'));
  var testImage8 = path.resolve(path.join(__dirname, 'image/test-image-8.jpg'));
  var testTraining = path.resolve(path.join(__dirname, 'training.dat'));

  var startResult = orbIndexer.startTraining();

  console.log('Start Training: ', startResult);

  var trainedIndex;

  orbIndexer.trainImageFile(testImage1)
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage2);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage3);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage4);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage5);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage6);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage7);
    })
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.saveTrainedIndex(testTraining);
    })
    .then(function (result) {
      console.log('Save Index Result: ', result);
      return orbIndexer.loadTrainedIndex(testTraining);
    })
    .then(function(result) {
      console.log('Load index result: ', result);
      return orbIndexer.indexImageFile('sample', testImage8);
    })
    .then(function(result) {
      result.sort(function(a, b) {
        return a.distance - b.distance;
      });
      for (var idx=0,len=result.length; idx<len; idx++) {
        console.log('[', idx, '] : ', result[idx].word_id, ' Dist: ', result[idx].distance);
      }
    })
    .catch(function (err) {
      console.log('Error: ', err.message);
    })
    .finally(function () {
      console.log('Done!');
      process.exit();
    });

}
