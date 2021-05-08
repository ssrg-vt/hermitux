/*
 * datagen.cpp
 * by Sam Kauffman - University of Virginia
 *
 * This program generates datasets for Rodinia's kmeans
 *
 * Usage:
 * datagen <numObjects> [ <numFeatures> ] [-f]
 *
 * numFeatures defaults to 34
 * Features are integers from 0 to 255 by default
 * With -f, features are floats from 0.0 to 1.0
 * Output file is "<numObjects>_<numFeatures>.txt"
 * One object per line. The first number in each line is an object ID and is
 * ignored by kmeans.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace std;

int main( int argc, char * argv[] )
{
	if ( argc < 2 )
	{
		cerr << "Error: Specify a number of objects to generate.\n";
		exit( 1 );
	}
	int nObj = atoi( argv[1] );
	int nFeat = 0;
	if ( argc > 2 )
		nFeat = atoi( argv[2] );
	if ( nFeat == 0 && argc > 3 )
		nFeat = atoi( argv[3] );
	if ( nFeat == 0 )
		nFeat = 34;
	if ( nObj < 1 || nFeat < 1 )
	{
		cerr << "Error: Invalid argument(s).\n";
		exit( 1 );
	}
	bool floats = false;
	if ( ( argc > 2 && strcmp( argv[2], "-f" ) == 0 ) || ( argc > 3 && strcmp( argv[2], "-f" ) == 0 ) )
		floats = true;

	stringstream ss;
	string f = floats ? "f" : "";
	ss << nObj << "_" << nFeat << f << ".txt";
	ofstream outf( ss.str().c_str(), ios::out | ios::trunc );
	srand( time( NULL ) );

	if ( !floats )
		for ( int i = 0; i < nObj; i++ )
		{
			outf << i << " ";
			for ( int j = 0; j < nFeat; j++ )
				outf << ( rand() % 256 ) << " ";
			outf << endl;
	}
	else
	{
		outf.precision( 4 );
		outf.setf( ios::fixed );
		for ( int i = 0; i < nObj; i++ )
		{
			outf << i << " ";
			for ( int j = 0; j < nFeat; j++ )
				outf << ( (float)rand() / (float)RAND_MAX ) << " ";
			outf << endl;
		}
	}

	outf.close();
	string type = floats ? " \"float\"" : " \"int\"";
	cout << "Generated " << nObj << " objects with " << nFeat << type << " features in " << ss.str() << ".\n";
}
