// netlist.h
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


#ifndef NETLIST_H
#define NETLIST_H

#include <vector>
#include <map>
#include <string>

#include "annealer_types.h"

const long NO_MATCHING_ELEMENT = -1;

class netlist_elem;
class location_t;
class Rng;

class netlist
{
public:
	netlist(const std::string& filename); //ctor
	void get_random_pair(netlist_elem** a, netlist_elem** b, Rng* rng); // will return an element that we have a valid mutex on
	void swap_locations(netlist_elem* elem_a, netlist_elem* elem_b);
	void shuffle(Rng* rng);
	netlist_elem* netlist_elem_from_loc(location_t& loc);
	netlist_elem* netlist_elem_from_name(std::string& name);
	routing_cost_t total_routing_cost();
	void print_locations(const std::string& filename);
	void release(netlist_elem* elem);
	netlist_elem* get_random_element(long* elem_id, long different_from, Rng* rng);
	
protected:
	unsigned _num_elements;
	unsigned _max_x;
	unsigned _max_y;
	unsigned _chip_size;
	std::vector<netlist_elem> _elements;//store the actual elements here
	std::vector< std::vector<location_t> > _locations;//store the actual locations here
	std::map<std::string, netlist_elem*> _elem_names;
	netlist_elem* create_elem_if_necessary(std::string& name);
	//due to the pointers, perhaps I should make the copy operator protected to prevent copying
};



#endif

