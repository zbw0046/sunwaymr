/*
 * ShuffledRDD.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLEDRDD_H_
#define HEADERS_SHUFFLEDRDD_H_

#include "Messaging.h"
#include "IteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
#include "Pair.h"
#include "SunwayMRContext.h"
#include "ShuffledRDDPartition.h"
#include "Aggregator.h"
#include "HashDivider.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

template <class K, class V, class C>

class ShuffledRDD : public RDD< Pair<K, C> >, public Messaging
{
public:
	ShuffledRDD(RDD< Pair<K, V> > &_preRDD, Aggregator< Pair<K, V>, Pair<K, C> > &_agg, HashDivider &_hd, long (*hf)(Pair<K, C>), string (*strf)(Pair<K, C>), long _shuffleID, Pair<K, C> (*_recoverFunc)(string));
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition &p);
	IteratorSeq< Pair<K, C> > iteratorSeq(Partition &p);
	void shuffle();
	double getRandom(double start, double end);
	map<K, C> merge(vector<string> replys);

private:
	RDD< Pair<K, V> > &preRDD;
	Aggregator< Pair<K, V>, Pair<K, C> > &agg;
	HashDivider &hd;
	long (*hashFunc)(Pair<K, C>); // function to compute hashCode of a pair
    string (*strFunc)(Pair<K, C>); // function  to serialize a pair to string (to save to file)
    Pair<K, C> (*recoverFunc)(string); // function to deserialize a string to a pair
    long shuffleID;
};


#endif /* HEADERS_SHUFFLEDRDD_H_ */