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

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>

#include "orbwordindex.h"

using namespace cv;
using namespace std;


ORBWordIndex::ORBWordIndex()
{
        words = new Mat(0, 32, CV_8U); // The matrix that stores the visual words.
        kdIndex = NULL;
        training = false;
}


ORBWordIndex::~ORBWordIndex()
{
        delete words;
        if (kdIndex!=NULL) {
                delete kdIndex;
        }
}

bool ORBWordIndex::isTraining() {
        return training;
}

int ORBWordIndex::startTraining() {
        if (training) {
                return ALREADY_TRAINING;
        }
        // Release existing words
        delete words;
        // ..and create a new empty words list
        words = new Mat(0, 32, CV_8U);

        training = true;
        return SUCCESS;
}

bool ORBWordIndex::wordPresent(Mat word) {
        for (int word_idx = 0; word_idx<words->rows; word_idx++) {
                if ( cv::countNonZero(word!=words->row(word_idx)) == 0) {
                        return true;
                }
        }
        return false;
}

u_int32_t ORBWordIndex::addTrainingFeatures(Mat training_features, unsigned min_distance) {
        // Lock for adding words to our vocabulary
        unique_lock<mutex> locker(trainingMutex);

        cv::Mat newDescriptors;
        if (words->rows==0) {
                newDescriptors = training_features;
        } else {
                cvflann::Matrix<unsigned char> m_features
                        ((unsigned char*)words->ptr<unsigned char>(0), words->rows, words->cols);

                kdIndex = new cvflann::HierarchicalClusteringIndex<cvflann::Hamming<unsigned char> >
                                  (m_features,cvflann::HierarchicalClusteringIndexParams(10, cvflann::FLANN_CENTERS_RANDOM, 8, 100));

                kdIndex->buildIndex();


                for (int i = 0; i < training_features.rows; ++i)
                {

                        std::vector<int> indices(1);
                        std::vector<int> dists(1);

                        knnSearch(training_features.row(i), indices, dists, 1);

                        for (unsigned j = 0; j < indices.size(); ++j)
                        {
                                const unsigned i_distance = dists[j];
                                if ( i_distance > min_distance ) {
                                  newDescriptors.push_back(training_features.row(i));
                                }
                        }

                }
        }

        int added_features = newDescriptors.rows;
        for (int feature_idx = 0; feature_idx<newDescriptors.rows; feature_idx++) {
                words->push_back(newDescriptors.row(feature_idx));
        }
        locker.unlock();
        return added_features;
}

int ORBWordIndex::endTraining(string visualWordsPath) {

        if (!training) {
                return NOT_TRAINING;
        }

        if (kdIndex!=NULL) {
                delete kdIndex;
        }

        // Make sure we wait for the last pending
        // training to be complete if one is pending
        unique_lock<mutex> locker(trainingMutex);

        cvflann::Matrix<unsigned char> m_features
                ((unsigned char*)words->ptr<unsigned char>(0), words->rows, words->cols);

        kdIndex = new cvflann::HierarchicalClusteringIndex<cvflann::Hamming<unsigned char> >
                          (m_features,cvflann::HierarchicalClusteringIndexParams(10, cvflann::FLANN_CENTERS_RANDOM, 8, 100));

        kdIndex->buildIndex();

        training = false;

        if (!saveVisualWords(visualWordsPath)) {
                locker.unlock();
                return SAVE_FAILED;
        }
        locker.unlock();
        return SUCCESS;
}

int ORBWordIndex::initialize(string visualWordsPath, int numberOfWords) {

        if (!readVisualWords(visualWordsPath)) {
		std::cerr << "DB File Missing: " << visualWordsPath << std::endl;
                return WORD_DB_FILE_MISSING;
	}

        if (numberOfWords>0 && words->rows != numberOfWords) {
		std::cerr << "Wrong size" << std::endl;
                return WORD_DB_WRONG_ROW_SIZE;
        }

        cvflann::Matrix<unsigned char> m_features
                ((unsigned char*)words->ptr<unsigned char>(0), words->rows, words->cols);

        kdIndex = new cvflann::HierarchicalClusteringIndex<cvflann::Hamming<unsigned char> >
                          (m_features,cvflann::HierarchicalClusteringIndexParams(10, cvflann::FLANN_CENTERS_RANDOM, 8, 100));

        kdIndex->buildIndex();

        return SUCCESS;
}

void ORBWordIndex::knnSearch(const Mat& query, vector<int>& indices,
                             vector<int>& dists, int knn, u_int16_t search_params)
{
        cvflann::KNNResultSet<int> m_indices(knn);

        m_indices.init(indices.data(), dists.data());

        kdIndex->findNeighbors(m_indices, (unsigned char*)query.ptr<unsigned char>(0),
                               cvflann::SearchParams(search_params));
}


/**
 * @brief Read the list of visual words from an external file.
 * @param fileName the path of the input file name.
 * @param words a pointer to a matrix to store the words.
 * @return true on success else false.
 */
bool ORBWordIndex::readVisualWords(string fileName)
{
	std::cerr << "Reading Visual Words: " << fileName << std::endl;
	cv::FileStorage wordsFile(fileName, cv::FileStorage::READ);
	wordsFile["words"] >> *words;
	wordsFile.release();
	std::cerr << "Words read!" << std::endl;
        return true;
}

/**
 * @brief Save the list of visual words to an external file.
 * @param fileName the path of the output file name.
 * @return true on success else false.
 */
bool ORBWordIndex::saveVisualWords(string fileName)
{
	cv::FileStorage wordsStorage(fileName, cv::FileStorage::WRITE);
	wordsStorage << "words" <<  *words;
        return true;
}

const char* ORBWordIndex::messages[] = {
        "Success",
        "Could not access word index DB file",
        "Word Index DB has the wrong number of rows",
        "Already training",
        "Detector is not being trained",
        "Failed to save Word Index DB"
};
