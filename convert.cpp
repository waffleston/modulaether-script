#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

const char cmd = '%';
	// unused
const string cSourceDef = "%^srcdef";
	// _1_ (first argument) is an output file to be located at _1_.js
const string cFunc = "%^fn";
	// function _2+_ <- should be in source _1_.
const string cVar = "%^var";
	// Unused
const string cGSource = "%^thisfile";
	// This entire file is raw javascript, and should be placed in source _1_.

char retChar(char inbound) {
	// This is just a test to see how to unlink variable from function.
	return inbound;
}
#define peekChar(up_char) \
	do { \
		int nextcharcounter = 0; \
		while (true) { \
			up_char = filein.peek(); \
			if (up_char == ' ') { \
				up_char = filein.get(); \
				nextcharcounter++; \
				up_char = filein.get(); \
				nextcharcounter++; \
				if (up_char != ' ') { \
					break; \
				} \
			} else { \
				break; \
			} \
		} \
		for (int i = 0; i < nextcharcounter; i++) { \
			filein.unget(); \
		} \
	} while (0)

#define prevChar(tResult) \
	do { \
		if (filein.tellg() <= tResult.size()) { \
			break; \
			cout << "Broke"; \
		} \
		int prevcharcounter = 0; \
		int isLastWord = 1; \
		char next_char; \
		peekChar(next_char); \
		if (next_char == 13 || next_char == 10) { \
			isLastWord = 2; \
		} \
		for (int i = isLastWord; i < tResult.size(); i++) { \
			filein.unget(); \
			prevcharcounter++; \
		} \
		string pre_word; \
		char prev_char; \
		while (true) { \
			filein.unget(); \
			prevcharcounter++; \
			filein.unget(); \
			prev_char = filein.get(); \
			/*cout << "Character encountered: " << prev_char << endl;*/ \
			if (prev_char != ' ' && prev_char != '\t') { \
				/*cout << "Char encountered: " << prev_char << endl;*/ \
				break; \
			} else { \
				pre_word += prev_char; \
			} \
		} \
		for (int i = 0; i < prevcharcounter; i++) { \
			filein.get(); \
		} \
		tResult = pre_word + tResult; \
	} while (0)


string trimCR(string inbound) {
	inbound.erase(inbound.find_last_not_of("\n\r")+1);
	return inbound;
}

string replacer (string sFn) {
	string resultant = "";
	string rFn;
	int iParen = count(sFn.begin(), sFn.end(), '(');
	if (iParen > 0 && sFn.size() > 1) {
		for (int a = 0; sFn[a] != '(' && a < sFn.size(); a++) {
			rFn += " ";
			rFn[a] = sFn[a];
		}
	} else if (sFn.size() > 1) {
		rFn = sFn;
	} else {
		return "";
	}
	resultant += rFn+"() { \n"
	"if (typeof maes_"+rFn+" == 'undefined') { \n"
	"window.setTimeout("+rFn+",100); \n"
	"} else { \n"
	""+rFn+" = maes_"+rFn+"; \n"
	"maes_"+rFn+".apply(this, arguments); \n"
	"}\n}\n";
	cout << "Remote function created: "+rFn+"\n";
	return resultant;
}
int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Modulaether-script; preprocesses .aes files to modular javascript." << endl;
		cout << "Usage: maejs main_file [other_files...]" << endl;
		return 1;
	}
	string file_mae;
	file_mae = argv[1];
	string fn_js = file_mae.substr(0, file_mae.size() - 3) + "js";
	//char * ches = new char[file_mae.length() + 1];
	char * chjs = new char[fn_js.length() + 1];
	//strcpy(ches, file_mae.c_str());
	strcpy(chjs, fn_js.c_str());
	//ifstream filein(ches);
	ofstream fileout(chjs);
	if (!fileout) {
		cout << "Could not create main output file." << endl;
		return 1;
	}
	fileout << "/*\n * This file generated from Modulaetherschrift source.\n */\n";
	//string strTemp;
	int functionflag = 0;
	int srcflag = 0;
	string content_core;
	vector<string> documents;
	vector<string> vSources;
	int curDocIndex = -1;
	int iSources = 0;
	int iBrackets = 0;
	int writetodoc = 0;
	
	for (int a = 1; a < argc; a++) {
		string filename;
		filename = argv[a];
		char * readFile = new char[filename.length() + 1];
		strcpy(readFile, filename.c_str());
		ifstream filein(readFile);
		if (!filein) {
			cout << "File " << readFile << " could not be opened." << endl;
			return 1;
		}
		string strTemp;

		filein >> strTemp;
		if (strTemp == cGSource) {
			filein >> strTemp;
			try {
				for (int i = 0; i < vSources.size(); i++) {
					if (vSources[i] == strTemp) {
						curDocIndex = i;
					}
				}
			} catch(string err) {
				cout << "Source \"" << strTemp << "\" not defined.";
				return 1;
			}
			while (filein >> strTemp) {
				char next_char;
				peekChar(next_char);
				// Maintain carriage returns
				if (next_char == 13 || next_char == 10) {
					strTemp += '\n';
				}
				prevChar(strTemp);
				documents[curDocIndex] += strTemp;
			}
			filein.close();
		} else {
			for(int b = 0; b < strTemp.size(); b++) {
				filein.unget();
			}

	while (filein >> strTemp) {
		char next_char;
		peekChar(next_char);
		// Maintain carriage returns
		if (next_char == 13 || next_char == 10) {
			strTemp += '\n';
		}

		// Source Definition
		if (strTemp == cSourceDef) {
			strTemp = "";
			srcflag = 1;
		} else if (srcflag == 1) {
			vSources.push_back(trimCR(strTemp));
			documents.push_back("/**\n * This file generated from Modulaetherschrift source.\n**/\n");
			iSources++;
			cout << "External file " << trimCR(strTemp) << ".js created\n";
			//strTemp = "setTimeout($.getScript(\"./"+strTemp+".js\").fail(function(){console.error(\"$.get failed on "+strTemp+".js!\")}), 5000);\n";
			strTemp = "setTimeout($.getScript(\"./"+trimCR(strTemp)+".js\"),5000);\n";
			srcflag = 0;
		}
		// Function
		else if (strTemp == cFunc/* && functionflag != 1*/) {
			//cout << strTemp;
			strTemp = "";
			functionflag = 1;
			writetodoc = 0;
		} else if (functionflag == 1) {
			functionflag = 2;
			try {
				for (int i = 0; i < vSources.size(); i++) {
					if (vSources[i] == strTemp) {
						curDocIndex = i;
					}
				}
			} catch(string err) {
				cout << "Source \"" << strTemp << "\" not defined.";
				return 1;
			}
			//cout << "function created in " << strTemp << ": ";
			strTemp = "function maes_";
			documents[curDocIndex] += strTemp;
			
			
			strTemp = "function ";
			//strTemp += " ";
			//strTemp = "";
		} else if (functionflag == 2) {
			//prevChar(strTemp);
			documents[curDocIndex] += strTemp;
			iBrackets += count(strTemp.begin(), strTemp.end(), '{');
			iBrackets -= count(strTemp.begin(), strTemp.end(), '}');
			//
			if (iBrackets >= 1) {
				functionflag = 3;
				//cout << "\n";
			}
			if (writetodoc == 0) {
				//cout << strTemp << "\n";
				strTemp = replacer(strTemp);
				writetodoc = 1;
				
			} else {
				strTemp = "";
			}
		} else if (functionflag == 3) {
			prevChar(strTemp);
			
			iBrackets += count(strTemp.begin(), strTemp.end(), '{');
			iBrackets -= count(strTemp.begin(), strTemp.end(), '}');
			
			if (iBrackets == 0) {
				functionflag = 0;
			}
			if (iBrackets < 0) {
				cout << "Syntax error: Too many closing brackets.";
				return 1;
			}
			if (filein.peek() == '\n') {
				strTemp += "\n";
			}
			documents[curDocIndex] += strTemp;
			strTemp = "";
		} else {
			prevChar(strTemp);
			if (filein.peek() == '\n') {
				strTemp += "\n";
			}
		}
		fileout << strTemp;
	}
		filein.close();
	}
	}
	fileout.close();
	for (int i = 0; i < documents.size(); i++) {
		//cout << "documents.size() block hit!\n";
		vSources[i] = trimCR(vSources[i]);
		char * filename = new char[vSources[i].length()+4];
		string sFilename = vSources[i] + ".js";
		strcpy(filename, sFilename.c_str());
		ofstream fileout(filename);
		if (!fileout) {
			cout << vSources[i] << ".js could not be created/opened.";
			return 1;
		}
		fileout << documents[i];
		fileout.close();
	}
	return 0;
}
