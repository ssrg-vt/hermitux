// netlist.cpp
//
// Created by Daniel Schwartz-Narbonne on 14/04/07.
//
// Copyright 2007 Princeton University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.


#include "location_t.h"
#include "netlist.h"
#include "netlist_elem.h"
#include "rng.h"

#include <fstream>
#include <iostream>
#include <assert.h>

using namespace std;

void netlist::release(netlist_elem* elem)
{
	return;
}


//*****************************************************************************************
//not thread safe, tho i could make it so if i needed to
//only look at the non_blank elements:  this saves some time
//*****************************************************************************************
routing_cost_t netlist::total_routing_cost()
{
	routing_cost_t rval = 0;
	for (std::map<std::string, netlist_elem*>::iterator iter = _elem_names.begin();
		 iter != _elem_names.end();
		 ++iter){
		netlist_elem* elem = iter->second;
		rval += elem->routing_cost_given_loc(*(elem->present_loc.Get()));
	}
	return rval / 2; //since routing_cost calculates both input and output routing, we have double counted
}



//*****************************************************************************************
// just use a simple shuffle algorithm
//*****************************************************************************************
void netlist::shuffle(Rng* rng)
{
	for (int i = 0; i < _max_x * _max_y * 1000; i++){
		netlist_elem *a, *b;
		get_random_pair(&a, &b, rng);
		swap_locations(a, b);
	}
}


//*****************************************************************************************
//  SYNC need chris' atomic swap algorithm
//*****************************************************************************************
void netlist::swap_locations(netlist_elem* elem_a, netlist_elem* elem_b)
{
	//and swap the locations stored in the actual netlist_elem
	elem_a->present_loc.Swap(elem_b->present_loc);
}


//*****************************************************************************************
//returns an elemment that is different from the different_from element
// if different_from == NO_MATCHING_ELEMENT then returns any element
//*****************************************************************************************
netlist_elem* netlist::get_random_element(long* elem_id, long different_from, Rng* rng)
{
	long id = rng->rand(_chip_size);
	netlist_elem* elem = &(_elements[id]);
	
	//loop until we get a non duplicate element
	//-1 is not a possible element, will never enter this loop in that case
	//if it doesn't work, try a new one
	while (id == different_from){ 
		id = rng->rand(_chip_size);
		elem = &(_elements[id]);
	}
	*elem_id=id;
	return elem;
}


//*****************************************************************************************
//assumption: will return elements a, b which we can get a valid lock on
//*****************************************************************************************
void netlist::get_random_pair(netlist_elem** a, netlist_elem** b, Rng* rng)
{
	//get a random element
	long id_a = rng->rand(_chip_size);
	netlist_elem* elem_a = &(_elements[id_a]);
	
	//now do the same for b
	long id_b = rng->rand(_chip_size);
	netlist_elem* elem_b = &(_elements[id_b]);

	//keep trying new elements until we get one that works
	//get required locks automatically rolls back if it fails
	//keep going until we get
	while (id_b == id_a){ //no duplicate elements
		//if it doesn't work, try a new one
		id_b = rng->rand(_chip_size);
		elem_b = &(_elements[id_b]);
	}

	*a = elem_a;
	*b = elem_b;
	return;
}

//*****************************************************************************************
//  No longer easy to implement
//*****************************************************************************************
netlist_elem* netlist::netlist_elem_from_loc(location_t& loc)
{
	assert(false);
	return NULL;
}

//*****************************************************************************************
//
//*****************************************************************************************
netlist_elem* netlist::netlist_elem_from_name(std::string& name)
{
	return (_elem_names[name]);
}

//*****************************************************************************************
//  TODO add errorchecking
// ctor.  Takes a properly formatted input file, and converts it into a 
//*****************************************************************************************
netlist::netlist(const std::string& filename)
{
	ifstream fin (filename.c_str());
	assert(fin.is_open()); // were there any errors on opening?

	//read the chip_array paramaters
	fin >> _num_elements >> _max_x >> _max_y;
	_chip_size = _max_x * _max_y;
	assert(_num_elements < _chip_size);
	
	//create a chip of the right size
	_elements.resize(_chip_size);
	
	cout << "locs created" << endl;
	//create the location elements
	vector<location_t> y_vec(_max_y); 
	_locations.resize(_max_x, y_vec);
	
	//and set each one to its correct value
	unsigned i_elem = 0;
	for (int x = 0; x < _max_x; x++){
		for (int y = 0; y < _max_y; y++){
			location_t* loc = &_locations.at(x).at(y);
			loc->x = x;
			loc->y = y;
			_elements.at(i_elem).present_loc.Set(loc);
			i_elem++;
		}//for (int y = 0; y < _max_y; y++)
	}//for (int x = 0; x < _max_x; x++)
	cout << "locs assigned" << endl;

	int i=0;
	while (!fin.eof()){
		i++;
		if ((i % 100000) == 0){
			cout << "Just saw element: " << i << endl;
		}
		std::string name;
		fin >> name;
		netlist_elem* present_elem = create_elem_if_necessary(name); // the element that we are presently working on
		//use create if necessary because it might have been created as a previous elements fanin

		//set the basic info for the element
		present_elem->item_name = name; //its name

		int type; //its type TODO errorcheck here
		fin >> type; // presently, don't actually use this 

		std::string fanin_name;
		while (fin >> fanin_name){
			if (fanin_name == "END"){
				break; //last element in fanin
			} //otherwise, make present elem the fanout of fanin_elem, and vice versa
			netlist_elem* fanin_elem = create_elem_if_necessary(fanin_name);
			present_elem->fanin.push_back(fanin_elem);
			fanin_elem->fanout.push_back(present_elem);
		}//while (fin >> fanin_name)
	}//while (!fin.eof())
		cout << "netlist created. " << i-1 << " elements." << endl;
}

//*****************************************************************************************
// Used in the ctor.  Since an element have fanin from an element that can occur both
// earlier and later in the input file, we must handle both cases
//*****************************************************************************************
netlist_elem* netlist::create_elem_if_necessary(std::string& name)
{
	static unsigned unused_elem = 0;//the first unused element
	netlist_elem* rval;
	//check whether we already have a netlist element with that name
	std::map<std::string, netlist_elem*>::iterator iter = _elem_names.find(name);
	if (iter == _elem_names.end()){
		rval = &_elements.at(unused_elem);//if not, get one from the _elements pool
		_elem_names[name] = rval;//put it in the map
		unused_elem++;
	} else {
		//if it is in the map, just get a pointer to it
		rval = iter->second;
	}
	return rval;
}

//*****************************************************************************************
// simple dump file
// not threadsafe
//*****************************************************************************************
void netlist::print_locations(const std::string& filename)
{
	ofstream fout(filename.c_str());
	assert(fout.is_open());

	for (std::map<std::string, netlist_elem*>::iterator iter = _elem_names.begin();
		 iter != _elem_names.end();
		 ++iter){
		netlist_elem* elem = iter->second;
		fout << elem->item_name << "\t" << elem->present_loc.Get()->x << "\t" << elem->present_loc.Get()->y << std::endl;
	}
}
