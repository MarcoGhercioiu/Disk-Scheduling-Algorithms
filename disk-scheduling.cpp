/*
 File: main_mjg170001.cpp
 Author: Vlad
 Procedures:
 main 					- creates copy of I/O requests for each algorithm, runs 1000 experiments of I/O requests
 	 	 	 	 	 	 from 50 to 150 for each algorithm, calculates average request time based on results.
 distributionGenerator 	- generate normal distribution.
 printVector 			- print formatted data from vector.
 calcSeekTime	 		- calculates seek time based on number of tracks traversed and average seek time.
 calcRequestTime 		- calculates total request time based on seek time, rotational latency, and transfer time.
 FIFO 					- run first in first out scheduling algorithm
 SSTF					- run shortest service time first scheduling algorithm
 SCAN					- run SCAN scheduling algorithm
 LIFO					- run last in first out scheduling algorithm

 */

#include <time.h>
#include <iostream>
#include <random>
#include <unistd.h>
#include <vector>
#include <algorithm>

// define head of HDD
#define head 100
// define interval for steps
#define step 10
// define number of intervals
#define num_steps 11
// define initial I/O requests
#define initialRequests 50
// counter for number of requests
int numRequest = initialRequests;

using namespace std;

/*
 vector<int> distributionGenerator(int upper, int lower, int total, int increment)
 Author: Vlad
 Date: 11/20/2020
 Description: generate a uniform random distribution that has an upper and lower bound, using total
 	 	 	 to determine the amount of values generated.

 Parameters:
 upper					I/P		int				upper bound
 lower					I/P		int				lower bound
 total					I/P		int				total numbers to be generated
 increment				I/P		int				increment to seed generator
 distributionGenerator	O/P		vector<int>		status code
 */

vector<int> distributionGenerator(int upper, int lower, int total,
		int increment) {
	vector<int> temp;
	// create random device
	random_device rand_dev;
	mt19937 generator(rand_dev());
	// seed generator with time and increment
	generator.seed(time(NULL) + increment);
	// set bounds
	uniform_int_distribution<> distribution(lower, upper);
	// generate I/O request data
	for (int i = 0; i < total; i++) {
		temp.push_back(distribution(generator));
	}
	return temp;
}

/*
 void printVector(vector<int> vect)
 Author: Vlad
 Date: 11/20/2020
 Description: print values contained in 1 dimensional vector

 Parameters:
 vect					I/P		vector<int> 	vector to be printed
 printVector			O/P		void			status code
 */

void printVector(vector<int> vect) {
	for (int i = 0; (unsigned) i < vect.size(); i++) {
		cout << "Index " << i << " = " << vect[i] << endl;
	}
}

/*
 double calcSeekTime(int trackCount)
 Author: Vlad
 Date: 11/20/2020
 Description: calculates seek time based on number of tracks traversed and average seek time
 Parameters:
 trackCount				I/P		int				track distance traversed
 calcSeekTime			O/P		double			status code
 */

double calcSeekTime(int trackCount) {
	return (trackCount * .025);
}

/*
 double calcRequestTime(int trackCount, int sectorCount)
 Author: Vlad
 Date: 11/20/2020
 Description: calculates total request time based on seek time, rotational latency, and transfer time

 Parameters:
 trackCount				I/P		int				distance of tracks traversed
 sectorCount			I/P		int				distance of sectors traversed
 calcRequestTime		O/P		double			status code
 */

double calcRequestTime(int trackCount, int sectorCount) {
	return (calcSeekTime(trackCount) + ((5 / 360) * sectorCount)
			+ (4000 / 6000000));
}

/*
 vector<int> FIFO(vector<int> trackNum, vector<int> sectorNum)
 Author: Vlad
 Date: 11/20/2020
 Description: Performs first in first out disk scheduling on I/O requests that are passed in

 Parameters:
 trackCount				I/P		vector<int>		distance of tracks traversed
 sectorCount			I/P		vector<int>		distance of sectors traversed
 FIFO					O/P		vector<int>		status code
 */

vector<int> FIFO(vector<int> trackNum, vector<int> sectorNum) {
	vector<int> temp;

	int tempHead = head;
	int tempSectHead = 0;

	int trackCount = 0;
	int sectorCount = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector;
	// loop through requests
	for (int i = 0; i < numRequest; i++) {
		cur_track = trackNum[i];
		cur_sector = sectorNum[i];

		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		sectorDistance = abs(cur_sector - tempSectHead);



		// increase the total distance traversed
		trackCount += trackDistance;
		sectorCount += sectorDistance;

		// current track is now head
		tempHead = cur_track;
		tempSectHead = cur_sector;
	}


	temp.push_back(trackCount);
	temp.push_back(sectorCount);


	return temp;

}


/*
 vector<int> SSTF(vector<int> trackNum, vector<int> sectorNum)
 Author: Vlad
 Date: 11/20/2020
 Description: Performs shortest service time first disk scheduling on I/O requests that are passed in

 Parameters:
 trackCount				I/P		vector<int>		distance of tracks traversed
 sectorCount			I/P		vector<int>		distance of sectors traversed
 SSTF					O/P		vector<int>		status code
 */

vector<int> SSTF(vector<int> trackNum, vector<int> sectorNum) {
	vector<int> temp;

	int tempHead = head;
	int tempSectHead = 0;

	int trackCount = 0;
	int sectorCount = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector;

	// loop through requests
	for (int i = 0; i < numRequest; i++) {

		int bestServiceTime = 1000;
		int bestServiceTimeIndex = -1;
	// loop through requests within to find shortest service time
		for (int j = 0; j < numRequest; j++) {

			int checkServiceTime = abs(trackNum[j] - tempHead);
	// find index of shortest service time next
			if (bestServiceTime > checkServiceTime && trackNum[j] != -1) {
				bestServiceTimeIndex = j;
				bestServiceTime = checkServiceTime;
			}
		}
	// initialize to shortest service time request
		cur_track = trackNum[bestServiceTimeIndex];
		cur_sector = sectorNum[bestServiceTimeIndex];

		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		trackNum[bestServiceTimeIndex] = -1;
		sectorNum[bestServiceTimeIndex] = -1;

		sectorDistance = abs(cur_sector - tempSectHead);

		// increase the total distance traversed
		trackCount += trackDistance;
		sectorCount += sectorDistance;

		// current track is now head
		tempHead = cur_track;
		tempSectHead = cur_sector;

	}

	temp.push_back(trackCount);
	temp.push_back(sectorCount);

	return temp;

}

/*
 vector<int> SCAN(vector<int> trackNum, vector<int> sectorNum)
 Author: Vlad
 Date: 11/20/2020
 Description: Performs SCAN disk scheduling on I/O requests that are passed in

 Parameters:
 trackCount				I/P		vector<int>		distance of tracks traversed
 sectorCount			I/P		vector<int>		distance of sectors traversed
 SCAN					O/P		vector<int>		status code
 */

vector<int> SCAN(vector<int> trackNum, vector<int> sectorNum) {
	vector<int> temp;

	int tempHead = head;
	int sectorHead = 0;

	int trackCount = 0;
	int sectorCount = 0;
	int trackDistance, sectorDistance, cur_track, cur_sector;


	// create vectors for each side of SCAN
	vector<int> leftTrack, rightTrack, leftSector, rightSector;
	vector<int> trackSeekSequence, sectorSeekSequence;

	// add requests to designated sides based on head location
	for (int i = 0; i < numRequest; i++) {
		if (trackNum[i] < tempHead) {
			leftTrack.push_back(trackNum[i]);
			leftSector.push_back(sectorNum[i]);
		} else if (trackNum[i] > tempHead) {
			rightTrack.push_back(trackNum[i]);
			rightSector.push_back(sectorNum[i]);

		}
	}
	// sort vectors
	sort(leftTrack.begin(), leftTrack.end());
	sort(leftSector.begin(), leftSector.end());

	sort(rightTrack.begin(), rightTrack.end());
	sort(rightSector.begin(), rightSector.end());

	// determine which direction to traverse first, false = right, true = left
	bool direction = false;

	for (int i = 0; i < 2; i++) {
		if (direction) {
			for (int j = leftTrack.size() - 1; j >= 0; j--) {
				cur_track = leftTrack[j];
				cur_sector = leftSector[j];

				trackSeekSequence.push_back(cur_track);
				sectorSeekSequence.push_back(cur_sector);

				// calculate absolute distance
				trackDistance = abs(cur_track - tempHead);
				sectorDistance = abs(cur_sector - sectorHead);

				// increase the total distance traversed
				trackCount += trackDistance;
				sectorCount += sectorDistance;

				// current track is now head
				tempHead = cur_track;
				sectorHead = cur_sector;
			}
			direction = false;
		} else if (!direction) {
			for (int j = 0; (unsigned) j < rightTrack.size(); j++) {
				cur_track = rightTrack[j];
				cur_sector = rightSector[j];

				trackSeekSequence.push_back(cur_track);
				sectorSeekSequence.push_back(cur_sector);

				trackDistance = abs(cur_track - tempHead);
				sectorDistance = abs(cur_sector - sectorHead);

				trackCount += trackDistance;
				sectorCount += sectorDistance;

				tempHead = cur_track;
				sectorHead = cur_sector;
			}
			direction = true;
		}
	}

	temp.push_back(trackCount);
	temp.push_back(sectorCount);

	return temp;

}

/*
 vector<int> LIFO(vector<int> trackNum, vector<int> sectorNum)
 Author: Vlad
 Date: 11/20/2020
 Description: Performs last in first out disk scheduling on I/O requests that are passed in

 Parameters:
 trackCount				I/P		vector<int>		distance of tracks traversed
 sectorCount			I/P		vector<int>		distance of sectors traversed
 LIFO					O/P		vector<int>		status code
 */

vector<int> LIFO(vector<int> trackNum, vector<int> sectorNum) {
	vector<int> temp;

	int tempHead = head;
	int tempSectHead = 0;

	int trackCount = 0;
	int sectorCount = 0;

	int trackDistance, sectorDistance, cur_track, cur_sector;

	for (int i = numRequest - 1; i > -1; i--) {
		cur_track = trackNum[i];
		cur_sector = sectorNum[i];

		// calculate absolute distance
		trackDistance = abs(cur_track - tempHead);
		sectorDistance = abs(cur_sector - tempSectHead);
		// increase the total distance
		trackCount += trackDistance;
		sectorCount += sectorDistance;
		// current track is now head
		tempHead = cur_track;
		tempSectHead = cur_sector;
	}

	temp.push_back(trackCount);
	temp.push_back(sectorCount);

	return temp;
}

/*
 int main()
 Author: Vlad
 Date: 11/20/2020
 Description: creates copy of I/O requests for each algorithm, runs 1000 experiments of I/O requests
 	 	 	 from 50 to 150 for each algorithm, calculates average request time based on results.

 Parameters:
 main		O/P		int		status code
 */

int main() {


	vector<int> stats;
	int increment = 0;

	// create vectors for each algorithm
	vector<int> trackNumFIFO;
	vector<int> sectorNumFIFO;

	vector<int> trackNumSSTF;
	vector<int> sectorNumSSTF;

	vector<int> trackNumSCAN;
	vector<int> sectorNumSCAN;

	vector<int> trackNumLIFO;
	vector<int> sectorNumLIFO;

	vector<vector<double>> totalTimeFIFO;
	vector<vector<double>> totalTimeSSTF;
	vector<vector<double>> totalTimeSCAN;
	vector<vector<double>> totalTimeLIFO;


	// conduct 1000 tests
	for (int iteration = 0; iteration < 1000; iteration++) {

		numRequest = initialRequests;
		vector<int> trackNum;
		vector<int> sectorNum;
		vector<int> temp;

		// generate I/O requests
		temp = distributionGenerator(201, 0, numRequest, increment);
		trackNum.insert(trackNum.end(), temp.begin(), temp.end());

		increment = rand() % 100 + 1;

		temp = distributionGenerator(360, 0, numRequest, increment);
		sectorNum.insert(sectorNum.end(), temp.begin(), temp.end());

		increment = rand() % 100 + 1;


		vector<double> iterationTimeFIFO;
		vector<double> iterationTimeSSTF;
		vector<double> iterationTimeSCAN;
		vector<double> iterationTimeLIFO;

		// generate new I/O requests for each step
		for (int i = 0; i < num_steps; i++) {

			vector<double> data;

			if (i != 0) {
				temp = distributionGenerator(201, 0, step, increment);
				trackNum.insert(trackNum.end(), temp.begin(), temp.end());

				increment = rand() % 100 + 1;

				temp = distributionGenerator(360, 0, step, increment);
				sectorNum.insert(sectorNum.end(), temp.begin(), temp.end());
			}

			increment = rand() % 100 + 1;

			trackNumFIFO = trackNum;
			sectorNumFIFO = sectorNum;

			trackNumSSTF = trackNum;
			sectorNumSSTF = sectorNum;

			trackNumSCAN = trackNum;
			sectorNumSCAN = sectorNum;

			trackNumLIFO = trackNum;
			sectorNumLIFO = sectorNum;

			// get track and sector distances traversed from each algorithm and calculate average request time

			stats = FIFO(trackNumFIFO, sectorNumFIFO);

			iterationTimeFIFO.push_back(calcRequestTime(stats[0], stats[1]) / numRequest);

			stats = SSTF(trackNumSSTF, sectorNumSSTF);

			iterationTimeSSTF.push_back(calcRequestTime(stats[0], stats[1]) / numRequest);

			stats = SCAN(trackNumSCAN, sectorNumSCAN);

			iterationTimeSCAN.push_back(calcRequestTime(stats[0], stats[1]) / numRequest);

			stats = LIFO(trackNumLIFO, sectorNumLIFO);

			iterationTimeLIFO.push_back(calcRequestTime(stats[0], stats[1]) / numRequest);

			// increment total number of requests
			numRequest += step;
		}

		totalTimeFIFO.push_back(iterationTimeFIFO);
		totalTimeSSTF.push_back(iterationTimeSSTF);
		totalTimeSCAN.push_back(iterationTimeSCAN);
		totalTimeLIFO.push_back(iterationTimeLIFO);

	}

	// arrays for average request time
	double averageTimeFIFO[num_steps] = { 0 };
	double averageTimeSSTF[num_steps] = { 0 };
	double averageTimeSCAN[num_steps] = { 0 };
	double averageTimeLIFO[num_steps] = { 0 };

	// add total time of each test to arrays
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < num_steps; j++) {
			averageTimeFIFO[j] += totalTimeFIFO[i][j];
			averageTimeSSTF[j] += totalTimeSSTF[i][j];
			averageTimeSCAN[j] += totalTimeSCAN[i][j];
			averageTimeLIFO[j] += totalTimeLIFO[i][j];
		}
	}

	// calculate average of all tests and print results
	for (int j = 0; j < num_steps; j++) {
		cout << "Average Request Time for " << ((j + 5) * 10)
				<< " Number of Requests:" << endl;

		averageTimeFIFO[j] /= 1000;
		averageTimeSSTF[j] /= 1000;
		averageTimeSCAN[j] /= 1000;
		averageTimeLIFO[j] /= 1000;

		cout << "FIFO: " << averageTimeFIFO[j] << " ms" << endl;
		cout << "SSTF: " << averageTimeSSTF[j] << " ms" << endl;
		cout << "SCAN: " << averageTimeSCAN[j] << " ms" << endl;
		cout << "LIFO: " << averageTimeLIFO[j] << " ms" << endl;


		cout << endl;
	}

	return 0;
}

