/*
 * ShuffleTask.hpp
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLETASK_HPP_
#define INCLUDE_SHUFFLETASK_HPP_

#include "ShuffleTask.h"

#include "RDD.hpp"
#include "Partition.hpp"
#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "Utils.hpp"
#include "SunwayMRContext.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

template <class T, class U> ShuffleTask<T, U>::ShuffleTask(RDD<T> &r, Partition &p, long shID, int nPs, HashDivider &hashDivider, Aggregator<T, U> &aggregator, long (*hFunc)(U), string (*sf)(U))
:RDDTask< T, int >::RDDTask(r, p), hd(hashDivider), agg(aggregator)
{
	shuffleID = shID;
    numPartitions = nPs;
	hashFunc = hFunc;
	strFunc = sf;
}

template <class T, class U> int&  ShuffleTask<T, U>::run()
{
	// get current RDD value
	IteratorSeq<T> iter = RDDTask< T, int >::rdd.iteratorSeq(RDDTask< T, int >::partition);
	vector<T> datas = iter.getVector();

	// T -> U
	vector<U> datas1;
	for(int i=0; i<datas.size(); i++)
		datas1.push_back(agg.createCombiner(datas[i]));

	// get list
    vector< vector<string> > list; // save partitionID -> keys
    for(int i=0; i<numPartitions; i++)
    {
    	vector<string> v;
    	list.push_back(v);
    }

	for(int i=0; i<datas.size(); i++)
	{
		long hashCode = hashFunc(datas1[i]);
		int pos = hd.getPartition(hashCode); // get the new partition index
		list[pos].push_back(strFunc(datas1[i]));
	}
	//merge data to a string
	string fileContent;
	for(int i=0; i<list.size(); i++)
	{
		if(list[i].size() == 0)
		{
			fileContent += string(SHUFFLETASK_EMPTY_DELIMITATION) + string(SHUFFLETASK_PARTITION_DELIMITATION);
			continue;
		}
		for(int j=0; j<list[i].size()-1; j++)
			fileContent += list[i][j] + string(SHUFFLETASK_KV_DELIMITATION);

		fileContent += list[i].back();
		fileContent += string(SHUFFLETASK_PARTITION_DELIMITATION);
	}

	// save to file
	string app_id = num2string(SUNWAYMR_CONTEXT_ID);
	string shuffle_id = num2string(shuffleID);
	string task_id = num2string(this->taskID);

	string dir = app_id.append("/shuffle-") + shuffle_id.append("/");
	string fileName = "shuffleTaskFile";
	fileName += task_id;
	writeFile(dir, fileName, fileContent);

	int *ret = new int(1);
	return *ret;
}



template <class T, class U> string ShuffleTask<T, U>::serialize(int &t)
{
	stringstream ss;
	string str;
	ss<<t;
	ss>>str;
	return str;
}

template <class T, class U> int& ShuffleTask<T, U>::deserialize(string s)
{
	int val = 0;
	stringstream ss;
	ss<<s;
	ss>>val;

	int *ret = new int(val);
	return *ret;
}

#endif /* INCLUDE_SHUFFLETASK_HPP_ */
