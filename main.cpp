// Very ugly Brute-Force apt->RAPID converter
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void dir(string);
void comment(string);
void linear(string);
void arc(string);
void replace(string&, string, string);

// direction
float dx, dy, dz;

// input streams
ifstream in;
// Output stream
ofstream out;

int main(int argc, char ** argv)
{
	string rivi;

	// First argument (file name) missing
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " <filename>\n";
		return -1;
	}

	// Create name for tmpfile
	string tmpfile(argv[1]);
	tmpfile = tmpfile + ".tmp";
	// open and clear tmpfile
//	out.open(tmpfile.c_str(), ios::trunc);

	// Try to open input file
	in.open(argv[1]);
	if (in.is_open()) {
		cout << "File " << argv[1] << " opened succesfully\n";
	} else {
		cout << "Error opening file.\n";
		return -1;
	}

	//TODO Make while loop
	for (int i=0; i<50; i++) {
		getline(in,rivi);
		if(rivi[0]=='$')
			comment(rivi);
		else if(rivi.find("INDIRV")!=string::npos)
			dir(rivi);
		else if(rivi.find("CIRCLE")!=string::npos)
			arc(rivi);
		else if(rivi.find("GOTO")!=string::npos)
			linear(rivi);
	}

	// Data has been caught... time to "swap"
	in.close();
//	out.close();
	
	return 0;
}

void dir(string rivi)
{
	replace(rivi, "INDIRV / ","");
	replace(rivi, ",", "\t");
	cout << "Direction from center: " << rivi << endl;
}

void comment(string rivi)
{
	cout << "Kommentti:" << rivi << endl;
	return;
}

void linear(string rivi)
{
	replace(rivi, "GOTO / ", "");
	rivi="MoveL " + rivi;
	cout << rivi << endl;
	
}

void arc(string rivi)
{
	string radius;
	cout << "****Found ARC\n";
	// Format first coordinates
	// Some "magic" happening here
	replace(rivi,"GOFWD / ( CIRCLE / ", ""); 
	replace(rivi,"$","");
	replace(rivi,",","\t");
	// Parsing radius data....
	in >> radius;
	replace(radius," ), $","");
	cout << rivi << endl;
	cout << radius << endl;
}

/* 
Function to replace parts of source string
var r=string to being replacing s
*/
void replace(string& source, string s, string r)
{
	size_t found;
	while (source.find(s)!=string::npos) {
		found=source.find(s);
		source.replace(found, s.length(), r);
	}
}
