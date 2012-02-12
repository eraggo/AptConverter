// Very ugly Brute-Force apt->RAPID converter
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void create_arc();
void handle_direction();
void create_linear();
void dir(string);
void comment(string);
void linear(string);
void arc(string);
void replace(string&, string, string);

// direction
float dx, dy, dz;
// current coordinate (changed after every move)
float x, y, z;
// input stream
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
	out.open(tmpfile.c_str(), ios::trunc);

	// Try to open input file
	in.open(argv[1]);
	if (in.is_open()) {
		cout << "File " << argv[1] << " opened succesfully\n";
	} else {
		cout << "Error opening file.\n";
		return -1;
	}

	while (!in.eof()) {
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
	out.close();
	////////////////////////////////////////////////////

	in.open(tmpfile.c_str());
	out.open("OUTPUT", ios::trunc);
	//figure out which command
	char cmd;
	in >> cmd;
	if(cmd=='L')
		create_linear();
	else if(cmd=='I')
		handle_direction();
	else if(cmd=='A')
		create_arc();

	out.close();
	in.close();
	return 0;
}

void create_arc()
{

}

void handle_direction()
{

}

void create_linear()
{

}

void dir(string rivi)
{
	replace(rivi, "INDIRV / ","");
	replace(rivi, ",", "\t");
	cout << "Direction from center: " << rivi << endl;
	rivi = "I\t" + rivi + "\n";
	out << rivi;
}

void comment(string rivi)
{
	cout << "Kommentti:" << rivi << endl;
	return;
}

void linear(string rivi)
{
	replace(rivi, "GOTO / ", "");
	replace(rivi, ",", "\t");
	rivi="L\t" + rivi + "\n";
	cout << rivi;
	out << rivi;
}

void arc(string rivi)
{
	string radius;
	string end;
	cout << "****Found ARC\n";
	// Format first coordinates
	// Some "magic" happening here
	replace(rivi,"GOFWD / ( CIRCLE / ", ""); 
	replace(rivi,"$","");
	replace(rivi,",","\t");
	// Parsing radius data....
	getline(in, radius);
	replace(radius," ), $","");
	// Parsing end point data
	getline(in, end);	// Not needed
	getline(in, end);
	replace(end, " ", "");
	replace(end, ")", "");
	replace(end, ",", "\t");
	cout << rivi << endl;
	cout << radius << endl;
	cout << end << endl;
	rivi = "A\t"+rivi+radius+"\t"+end+"\n";
	out << rivi;
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
