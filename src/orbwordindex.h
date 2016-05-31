/*****************************************************************************
 * Copyright (C) 2014 Visualink
 *
 * Authors: Adrien Maglo <adrien@visualink.io>
 *
 * This file is part of Pastec.
 *
 * Pastec is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pastec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Pastec.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef PASTEC_ORBWORDINDEX_H
#define PASTEC_ORBWORDINDEX_H

#include <vector>
#include <mutex>

#include <opencv2/core/core.hpp>
#include <opencv2/flann/flann.hpp>

class ORBWordIndex
{
public:
    ORBWordIndex();
    ~ORBWordIndex();
    void knnSearch(const cv::Mat &query, std::vector<int>& indices,
                   std::vector<int> &dists, int knn, u_int16_t search_params = 2000);
    int initialize(std::string visualWordsPath, int numberOfWords = 0);
    int startTraining();
    bool wordPresent(cv::Mat word);
    u_int32_t addTrainingFeatures(cv::Mat training_features, unsigned min_distance = MIN_DISTANCE_TRAINING);
    int endTraining(std::string visualWordsPath = "/dev/null");
    bool isTraining();
    const static int SUCCESS = 0;
    const static int WORD_DB_FILE_MISSING = 1;
    const static int WORD_DB_WRONG_ROW_SIZE = 2;
    const static int ALREADY_TRAINING = 3;
    const static int NOT_TRAINING = 4;
    const static int SAVE_FAILED = 5;
    const static char * messages[];

private:
    const static unsigned MIN_DISTANCE_TRAINING = 30;
    bool readVisualWords(std::string fileName);
    bool saveVisualWords(std::string fileName);
    bool training;
    std::mutex trainingMutex;
    cv::Mat *words;  // The matrix that stores the visual words.
    cvflann::HierarchicalClusteringIndex<cvflann::Hamming<unsigned char> > *kdIndex; // The kd-tree index.
};

#endif // PASTEC_ORBWORDINDEX_H
