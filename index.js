const path = require('path');
const nativeOrbIndexer = require('./build/Release/orbidx');

var OrbIndexer = function(wordIndexPath) {
  wordIndexPath = wordIndexPath || path.resolve(path.join(__dirname, 'data', 'visualWordsORB.dat'));
  console.log('Word Index Path: ' + wordIndexPath);

}

module.exports = OrbIndexer;
