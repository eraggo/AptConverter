// Very ugly Brute-Force apt->RAPID converter
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <sstream>
using namespace std;

void handle_tool();
void create_arc();
void handle_direction();
void create_linear();
void create_feed();
void tool(string);
void fedrate(string);
void dir(string);
void comment(string);
void linear(string);
void arc(string);
template <typename T>
string to_str(T);
template <typename T>
string point(T,T,T);
void replace(string&, string, string);

// direction
float dx, dy, dz;
// current coordinate (changed after every move)
float x, y, z;
// current feedrate
float feed;
// current tool
int ctool;
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

	// Create filenames
	// Create name for tmpfile
	string tmpfile(argv[1]);
	tmpfile = tmpfile + ".tmp";
	string output(argv[1]); //TODO
	// open and clear tmpfile
	out.open(tmpfile.c_str(), ios::trunc);

	// Try to open input file
	in.open(argv[1]);
	if (in.is_open()) {
		cout << "File " << argv[1] << " opened succesfully\n";
	} else {
		cout << "Error opening file.\n";
		out.close();
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
		else if(rivi.find("FEDRAT")!=string::npos)
			fedrate(rivi);
		else if(rivi.find("LOADTL")!=string::npos)
			tool(rivi);
	}

	// Data has been caught... time to "swap"
	in.close();
	out.close();

	// STAGE 2
	in.open(tmpfile.c_str());
	out.open("OUTPUT", ios::trunc);
	//figure out which command
	char cmd;

	while (!in.eof()) {
		in >> cmd;
		if(cmd=='L')
			create_linear();
		else if(cmd=='T')
			handle_tool();
		else if(cmd=='I')
			handle_direction();
		else if(cmd=='A')
			create_arc();
		else if(cmd=='F')
			create_feed();
	}

	out.close();
	in.close();
	return 0;
}

void handle_tool()
{
	in >> ctool;
}

void create_arc()
{
	// center coordinate
	float cx, cy, cz;
	// radius
	float r;
	// end point
	float ex, ey, ez;
	// Temp variables
	float kx, ky, kz;
	// Read points and radius
	in >> cx >> cy >> cz >> r >> ex >> ey >> ez;
	// Time for math
	// Necessary to calculate??
	kx = ex - x;
	ky = ey - y;
	kz = ez - z;
	// Direction effects calculation
	if (dx) {
		ky=ky/2;
		kx=dx*sqrt(r*r-ky*ky);
		ky=ky+y;
		kx=kx+x;
	} else {
		kx=kx/2;
		ky=dy*sqrt(r*r-kx*kx);
		kx=kx+x;
		ky=ky+y;
	}
	// Generate RAPID
	// First point in arc
	string rapid = "MoveC "+point(kx, ky, kz);
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]";
	// next end point
	rapid = rapid + "," + point(ex,ey,ez);
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]";
	// then feed rate
	rapid = rapid + ",v" + to_str(feed);
	// next "zone"
	rapid = rapid + ",fine";
	// and lastly tool code
	rapid = rapid + ",tool" + to_str(ctool) + "\n";
	out << rapid;
	// Update current location
	x = ex;
	y = ey;
	z = ez;
}

void handle_direction()
{
	in >> dx >> dy >> dz;
	if (dx<0)
		dx=-1.0;
	else if (dx>0)
		dx=1.0;
	else
		dx=0.0;

	if (dy<0)
		dy=-1.0;
	else if (dy>0)
		dy=1.0;
	else
		dy=0.0;

	if (dz<0)
		dz=-1.0;
	else if (dz>0)
		dz=1.0;
	else
		dz=0.0;
}

void create_linear()
{
	in >> x >> y >> z;
	string rapid="MoveL "+point(x,y,z);
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[0,0,0,0]";
	rapid = rapid + ",[9E+09,9E+09,9E+09,9E+09,9E+09,9E+09]";
	rapid = rapid + ",v" + to_str(feed);
	rapid = rapid + ",fine";
	rapid = rapid + ",tool" + to_str(ctool) + "\n";
	out << rapid;
}

void create_feed()
{
	in >> feed;
}

void dir(string rivi)
{
	replace(rivi, "INDIRV / ","");
	replace(rivi, ",", "\t");
	rivi = "I\t" + rivi + "\n";
	out << rivi;
}

void tool(string rivi)
{
	replace(rivi, "LOADTL / ","");
	out << "T\t" << rivi << endl;
}

void fedrate(string rivi)
{
	replace(rivi, "FEDRAT / ","");
	replace(rivi, ",  MMPM","");
	out << "F\t" << rivi << endl;
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
	out << rivi;
}

void arc(string rivi)
{
	string radius;
	string end;
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
	rivi = "A\t"+rivi+radius+"\t"+end+"\n";
	out << rivi;
}

// funtion returns string format of point (in rapid style)
template <typename T>
string point(T tx, T ty, T tz)
{
	string rapid="["+to_str(tx)+","+to_str(ty)+","+to_str(tz)+"]";
	return rapid;
}

/*
Funtion for changing numbers to strings
*/
template <typename T>
string to_str(T source)
{
	ostringstream target;
	target << source;
	return target.str();
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
