const path = require('path');
const fs = require('fs');
const defaults = require('lodash.defaults');
const Promise = require('bluebird');

const NativeOrbIndexer = require('./build/Release/orbidx');

var default_options = {
  nfeatures: 2000,
  scaleFactor: 1.02,
  nlevels: 100,
  edgeThreshold: 15,
  firstLevel: 0,
  WTA_K: 2,
  scoreType: 0, // 0: Harris Score, 1: Fast Score
  patchSize: 15,
  fastThreshold: 20,
  maxKeypoints: 2000, // Max number of keypoints in the grid
  gridRows: 1,  // Grid Based Features - to make sure they are
  gridCols: 1,   // distributed in the image; use 1x1 to have the
                // usual distribution, -1x-1 will use Pyramid
  minFeatureDistance: 30 // The minumum distance of a feature from an existing
                        // one before it's added to the visual word DB
}

var OrbIndexer = function (options) {
  options = defaults(options, default_options);
  options.wordIndexPath = options.wordIndexPath || path.resolve(path.join(__dirname, 'data', 'visualWordsORB.dat'));
  this.options = options;
  this.orbIndexer = new NativeOrbIndexer.OrbIndexer();
}

OrbIndexer.prototype = {
  initialize: function () {
    var _this = this;
    return new Promise(function (resolve, reject) {
      try {
        _this.orbIndexer.initWordIndex(function (err, res) {
          if (err) {
            console.log('Error: ' + err);
            return reject(new Error(err));
          }
          resolve(res);
        }, _this.options.wordIndexPath);
      } catch (err) {
        console.log('Error: ' + err.message);
        reject(err);
      }
    });
  },
  _loadImageFile: function (imagePath) {
    return new Promise(function (resolve, reject) {
      fs.readFile(imagePath, (err, data) => {
        if (err) {
          return reject(err);
        }
        resolve(new Buffer(data));
      });
    });
  },
  indexImageFile: function (imageId, imagePath) {
    var _this = this;
    return _this._loadImageFile(imagePath)
      .then(function (imageData) {
        return _this.indexImage(imageId, imageData);
      });
  },
  indexImage: function (imageId, imageBuffer) {
    var _this = this;
    return new Promise(function (resolve, reject) {
      try {
        _this.orbIndexer.indexImage(function (err, hits) {
            if (err) {
              return reject(new Error(err));
            }
            resolve(hits);
          }, imageId, imageBuffer, imageBuffer.length,
          _this.options.nfeatures,
          _this.options.scaleFactor,
          _this.options.nlevels,
          _this.options.edgeThreshold,
          _this.options.firstLevel,
          _this.options.WTA_K,
          _this.options.scoreType,
          _this.options.patchSize,
          _this.options.fastThreshold,
          _this.options.maxKeypoints,
          _this.options.gridRows,
          _this.options.gridCols
        );
      } catch (err) {
        reject(err);
      }
    });
  },
  startTraining: function() {
    return this.orbIndexer.startTraining();
  },
  trainImageFile: function (imagePath) {
    var _this = this;
    return _this._loadImageFile(imagePath)
      .then(function (imageData) {
        return _this.trainImage(imageData);
      });
  },
  trainImage: function(imageBuffer) {
    var _this = this;
    return new Promise(function (resolve, reject) {
      try {
        _this.orbIndexer.trainImage(function (err, message) {
            if (err) {
              return reject(new Error(err));
            }
            resolve(message);
          }, imageBuffer, imageBuffer.length,
          _this.options.nfeatures,
          _this.options.scaleFactor,
          _this.options.nlevels,
          _this.options.edgeThreshold,
          _this.options.firstLevel,
          _this.options.WTA_K,
          _this.options.scoreType,
          _this.options.patchSize,
          _this.options.fastThreshold,
          _this.options.maxKeypoints,
          _this.options.gridRows,
          _this.options.gridCols,
          _this.options.minFeatureDistance
        );
      } catch (err) {
        reject(err);
      }
    });
  },
  saveTrainedIndex: function(trainingFile) {
    var _this = this;
    return new Promise(function (resolve, reject) {
      try {
        _this.orbIndexer.saveTraining(function (err, result) {
            if (err) {
              return reject(new Error(err));
            }
            resolve(result);
          }, trainingFile);
      } catch (err) {
        reject(err);
      }
    });
  },
  loadTrainedIndex: function(trainingFile) {
    var _this = this;
    return new Promise(function (resolve, reject) {
      try {
        _this.orbIndexer.loadTraining(function (err, result) {
            if (err) {
              return reject(new Error(err));
            }
            resolve(result);
          }, trainingFile);
      } catch (err) {
        reject(err);
      }
    });
  }
}

module.exports = OrbIndexer;
