// secret(cpp) -- main file
/**
 * ISA project 2021
 * Autor: David Fridrich
 * 
*/

// HOW TO RUN
// secret -r <file> -s <ip|hostname> [-l]
// -r <file> : specifikace souboru pro přenos
// -s <ip|hostname> : ip adresa/hostname na kterou se má soubor zaslat
// -l : pokud je program spuštěn s tímto parametrem, jedná se o server, který naslouchá příchozím ICMP zprávám a ukládá soubor do stejného adresáře, kde byl spuštěn.


using namespace std;

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <sys/stat.h>
// #include <fstream>
// #include <unistd.h>


// input options globally available
class App {
	public:
		string r_opt = ""; //specifikace souboru
		string s_opt = ""; // ip adresa/hostname kam se ma soubor poslat
};


// ------------- function declarations -------------
void printErr(string s);
void printHelp();
void prepareServer();
int loopServer();
int parser(App *app, int argc, vector <string> argv);

// DEBUG
void gibVector(vector <string>v);
//  -------------    -------------    -------------

string encryptionKey = "xfridr08";

void printErr(string s){
	cout << "Error: " << s << endl;
}

// help function prints out help message
void printHelp(){
}




void prepareServer() {
	cout << "preparing this bad boy server...";

	cout << "done" << endl;
	loopServer();
	exit(0); //dont return control -> when server stops running, exit.
}

int loopServer() {
	cout << "starting server..." << endl;
	return 0;
}


//RUN: secret -r <file> -s <ip|hostname> [-l]


void gibVector(vector <string>v){
	cout << "...Vector..." << endl;
	for (size_t i = 0; i < v.size(); i++)
	{
		cout << v[i] << ',';
	}
	
	cout<< endl << "........" << endl;
}

int parser(App *app, int argc, vector <string> argv){
	string param;
	bool err = false;

	/**
	 * ------ this instance will become a listening server ------
	 * 
	 * if -l option is given, it overrides every other option, by being a server
	 * therefore invalid options can be given as well, as long as '-l' is given
	*/
	if (find(argv.begin(),argv.end(),(string)"-l") != argv.end()){
		prepareServer();
		//control is not returned here
	}


	// check parameters
	while(argv.begin() != argv.end()){

		// calling .front() on an empty vector is undefined behavour
		if(argv.empty()){
			printHelp();
		}

		param = argv.front();
		argv.erase(argv.begin());

		if(argv.empty()){
			printErr("No info was given after parameter");
			exit(1);
		}
		if (!param.compare((string)"-r")){ //file	
			app->r_opt = argv.front();
			
			// check if file exists
			struct stat b;
			if(!stat(app->r_opt.c_str(),&b) == 0){
				printErr("Given file " + app->r_opt + " doesn't exist");
				exit(1);
			}
			// do something with file
		} 
		else if (!param.compare((string)"-s")){ //ip adresa/hostname
			app->s_opt = argv.front();
			//check legitimity

		}
		else {
			printErr("The option |"+ param +"| was not recognized, try --help.");
			err = true;
			break;
		}

		argv.erase(argv.begin());
	}

	// return 0 on success, otherwise an error
	return (err == false) ? 0 : -1;
}


int main(int argc, char **argv){
	cout << "------code start-------" << endl;
	App app;
	vector <string> args(argv+1, argv+argc); //range constructor

	// parse cmdl arguments
	if(parser(&app,argc,args) != 0){
		// error -- message is dealt with from within the func (err func caled)
		return 1;
	}

	return 0;
}
