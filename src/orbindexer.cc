#include "common.h"

#if ((CV_MAJOR_VERSION == 2) && (CV_MINOR_VERSION >=4))
#include "orbindexer.h"
#include <stdio.h>

void OrbIndexer::Init(Local<Object> target) {
  Nan::SetMethod(target, "indexImage", IndexImage);
  Nan::SetMethod(target, "initWordIndex", InitWordIndex);
}

ORBWordIndex * OrbIndexer::wordIndex;

const char* OrbIndexer::ToCString(const String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

class AsyncIndexImage: public Nan::AsyncWorker {

public:

  AsyncIndexImage(Nan::Callback *callback, std::string str_imageId, unsigned char * buff_imageData, unsigned int i_imageSize) :
      Nan::AsyncWorker(callback),
      str_imageId(str_imageId),
      buff_imageData(buff_imageData),
      i_imageSize(i_imageSize),
      success(false) {

  }

  ~AsyncIndexImage() {

  }

  void setupExtractor(int nfeatures, double scaleFactor, int nlevels, int edgeThreshold,
    int firstLevel, int WTA_K, int scoreType, int patchSize, int fastThreshold,
    int maxKeypoints, int gridRows, int gridCols) {
    orbDetector = new cv::ORB(
            nfeatures,
            scaleFactor,
            nlevels,
            edgeThreshold,
            firstLevel,
            WTA_K,
            scoreType,
            patchSize
            // --> 3 ,fastThreshold
    );

    gridDetector = new cv::GridAdaptedFeatureDetector(orbDetector, maxKeypoints, gridRows, gridCols);
  }

  void resize(cv::Mat image) {
    unsigned int image_width = image.cols;
    unsigned int image_height = image.rows;

    if (image_width > 1000  || image_height > 1000)
    {
        cv::Size size;
        if (image_width > image_height)
        {
            size.width = 1000;
            size.height = (float) image_height / image_width * 1000;
        }
        else
        {
            size.width = (float) image_width / image_height * 1000;
            size.height = 1000;
        }

        cv::resize(image, image, size);
    }
  }

  Local<Object> createHitObject(HitForward hit) {
    Local<Object> js_hit = Nan::New<Object>();
    Nan::Set(js_hit, Nan::New("word_id").ToLocalChecked(),
              Nan::New(hit.i_wordId));
    Nan::Set(js_hit, Nan::New("image_id").ToLocalChecked(),
              Nan::New(hit.str_imageId).ToLocalChecked());
    Nan::Set(js_hit, Nan::New("angle").ToLocalChecked(),
              Nan::New(hit.i_angle));
    Nan::Set(js_hit, Nan::New("x").ToLocalChecked(),
              Nan::New(hit.x));
    Nan::Set(js_hit, Nan::New("y").ToLocalChecked(),
              Nan::New(hit.y));
    return js_hit;
  }

  void Execute() {
    try {

      std::vector<char> image_data(buff_imageData, buff_imageData + i_imageSize);

      cv::InputArray image_array = cv::InputArray(image_data);
      cv::Mat inputImage = cv::imdecode(image_array, 0);

      resize(inputImage);

      unsigned int image_width = inputImage.cols;
      unsigned int image_height = inputImage.rows;

      std::vector<int> compression_params;

      // CV_IMWRITE_PNG_COMPRESSION == 16
      compression_params.push_back(16);
      compression_params.push_back(2);

      std::vector<cv::KeyPoint> keypoints;
      cv::Mat descriptors;

      gridDetector->detect(inputImage, keypoints);
      orbDetector->compute(inputImage, keypoints, descriptors);

      unsigned i_nbKeyPoints = 0;

      std::unordered_set<u_int32_t> matchedWords;
      for (unsigned i = 0; i < keypoints.size(); ++i)
      {
          i_nbKeyPoints++;

          // Recording the angle on 16 bits.
          u_int16_t angle = keypoints[i].angle / 360 * (1 << 16);
          float x = keypoints[i].pt.x / image_width;
          float y = keypoints[i].pt.y / image_height;

          std::vector<int> indices(1);
          std::vector<int> dists(1);

          OrbIndexer::wordIndex->knnSearch(descriptors.row(i), indices, dists, 1);

          for (unsigned j = 0; j < indices.size(); ++j)
          {
              const unsigned i_wordId = indices[j];
              if (matchedWords.find(i_wordId) == matchedWords.end())
              {
                  HitForward newHit;
                  newHit.i_wordId = i_wordId;
                  newHit.str_imageId = str_imageId.c_str();
                  newHit.i_angle = angle;
                  newHit.x = x;
                  newHit.y = y;
                  imageHits.push_back(newHit);
                  matchedWords.insert(i_wordId);
              }
          }
      }
      success = true;
    } catch (cv::Exception &e) {
      err_msg = e.what();
      return;
    }
  }

  void HandleOKCallback() {
    Local<Value> argv[2];
    if (success) {
      argv[0] = Nan::Null();
      Local<Array> image_hits = Nan::New<Array>();
      std::list<HitForward>::const_iterator iterator;
      int idx = 0;
      for (iterator = imageHits.begin(); iterator != imageHits.end(); ++iterator) {
        Nan::Set(image_hits, idx++, createHitObject(*iterator));
      }
      argv[1] = image_hits;
    } else {
      argv[0] = Nan::New(err_msg).ToLocalChecked();
      argv[1] = Nan::Null();
    }
    callback->Call(2, argv);
  }

private:
  std::string str_imageId;
  unsigned char * buff_imageData;
  unsigned int i_imageSize;
  bool success;
  const char *err_msg;
  std::list<HitForward> imageHits;
  cv::GridAdaptedFeatureDetector * gridDetector;
  cv::ORB * orbDetector;
};


NAN_METHOD(OrbIndexer::IndexImage) {

  Isolate * isolate = info.GetIsolate();

  if (info.Length() < 4) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  Local<Function> cb = Local<Function>::Cast(info[0]);
  Nan::Callback *callback = new Nan::Callback(cb.As<Function>());
  String::Utf8Value imageIdUTF(info[1]);
  std::string image_id =  ToCString(imageIdUTF);
  unsigned char * image_buffer = (unsigned char*) node::Buffer::Data(info[2]->ToObject());
  unsigned int image_size = info[3]->Uint32Value();

  int nfeatures= 2000;
  if ( info.Length() >= 5 ) {
    nfeatures = info[4]->IntegerValue();
  }

  float scaleFactor=1.2f;
  if ( info.Length() >= 6 ) {
    scaleFactor = info[5]->NumberValue();
  }

  int nlevels=100;
  if ( info.Length() >= 7 ) {
    nlevels = info[6]->IntegerValue();
  }

  int edgeThreshold=15; // Changed default (31);
  if ( info.Length() >= 8 ) {
    edgeThreshold = info[7]->IntegerValue();
  }

  int firstLevel=0;
  if ( info.Length() >= 9 ) {
    firstLevel = info[8]->IntegerValue();
  }

  int WTA_K=2;
  if ( info.Length() >= 10 ) {
    WTA_K = info[9]->IntegerValue();
  }

  int scoreType= cv::ORB::HARRIS_SCORE;
  if ( info.Length() >= 11 ) {
    scoreType = info[10]->IntegerValue();
  }

  int patchSize=31;
  if ( info.Length() >= 12 ) {
    patchSize = info[11]->IntegerValue();
  }

  int fastThreshold=20;
  if ( info.Length() >= 13 ) {
    fastThreshold = info[12]->IntegerValue();
  }

  int maxKeypoints=2000;
  if ( info.Length() >= 14 ) {
    maxKeypoints = info[13]->IntegerValue();
  }

  int gridRows=8;
  if ( info.Length() >= 15 ) {
    gridRows = info[14]->IntegerValue();
  }

  int gridCols=8;
  if ( info.Length() >= 16 ) {
    gridCols = info[15]->IntegerValue();
  }

  AsyncIndexImage * asyncDetect = new AsyncIndexImage(callback, image_id, image_buffer, image_size);

  asyncDetect->setupExtractor(
    nfeatures,
    scaleFactor,
    nlevels,
    edgeThreshold,
    firstLevel,
    WTA_K,
    scoreType,
    patchSize,
    fastThreshold,
    maxKeypoints,
    gridRows,
    gridCols
  );

  Nan::AsyncQueueWorker(asyncDetect);
  return;
}

class AsyncWordIndexInitializer: public Nan::AsyncWorker {

public:

  AsyncWordIndexInitializer(Nan::Callback *callback,  std::string indexPath, ORBWordIndex * wordIndex) :
      Nan::AsyncWorker(callback),
      indexPath(indexPath),
      wordIndex(wordIndex),
      success(false) {
  }

  ~AsyncWordIndexInitializer() {

  }

  void Execute() {
    try {
      int res = wordIndex->initialize(indexPath);
      if (res != ORBWordIndex::SUCCESS) {
        success = false;
        error_message =  "Error initializing word index [";
        error_message  += indexPath;
        error_message += "]: ";
        error_message += ORBWordIndex::messages[res];
        std::cerr << "Error: " << error_message.c_str() << "\n";
        return;
      }
      success = true;
    } catch (cv::Exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      error_message = e.what();
      return;
    }
  }

  void HandleOKCallback() {
    Local<Value> argv[2];
    if (success) {
      argv[0] = Nan::Null();
      argv[1] = Nan::New("Word Index Initialized").ToLocalChecked();
    } else {
      argv[0] = Nan::New(error_message.c_str()).ToLocalChecked();
      argv[1] = Nan::Null();
    }
    callback->Call(2, argv);
  }

private:
  std::string indexPath;
  ORBWordIndex * wordIndex;
  bool success;
  std::string error_message;
};

  NAN_METHOD(OrbIndexer::InitWordIndex) {

    Isolate * isolate = info.GetIsolate();

    if (info.Length() < 1) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong number of arguments")));
      return;
    }

    if (OrbIndexer::wordIndex != NULL) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "ORB Work Index Already Initialized!")));
      return;
    }

    Local<Function> cb = Local<Function>::Cast(info[0]);
    Nan::Callback *callback = new Nan::Callback(cb.As<Function>());
    String::Utf8Value utf_path(info[1]);
    std::string index_path =  ToCString(utf_path);
    OrbIndexer::wordIndex = new ORBWordIndex();
    AsyncWordIndexInitializer * asyncWordIndexInit = new AsyncWordIndexInitializer(callback, index_path, OrbIndexer::wordIndex);
    Nan::AsyncQueueWorker(asyncWordIndexInit);
    return;
  }

#endif
