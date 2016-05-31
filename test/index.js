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
  var orbIndexer = new OrbIndexer();

  var testImage = path.resolve(path.join(__dirname, 'image/windmill-1.jpg'));
  var testTraining = path.resolve(path.join(__dirname, 'training.dat'));

  var startResult = orbIndexer.startTraining();

  console.log('Start Training: ', startResult);

  var trainedIndex;

  orbIndexer.trainImageFile(testImage)
    .then(function (result) {
      console.log('Training result: ', result);
      return orbIndexer.trainImageFile(testImage);
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
      return orbIndexer.indexImageFile('sample', testImage);
    })
    .then(function(result) {
      console.log('Image index result: ', result.length);
    })
    .catch(function (err) {
      console.log('Error: ', err.message);
    })
    .finally(function () {
      process.exit();
    });

}
