#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

const char cmd = '%'
const string cSourceDef = "%^srcdef";
const string cFunc = "%^fn";
const string cVar = "%^var"; 

char retChar(char inbound) {
	// This is just a test to see how to unlink variable from function.
	return inbound;
}
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
	string file_mae;
	file_mae = argv[1];
	string fn_js = file_mae.substr(0, file_mae.size() - 3) + "js";
	char * ches = new char[file_mae.length() + 1];
	char * chjs = new char[fn_js.length() + 1];
	strcpy(ches, file_mae.c_str());
	strcpy(chjs, fn_js.c_str());
	ifstream filein(ches);
	ofstream fileout(chjs);
	if (!filein || !fileout) {
		cout << "The requested file was not found." << endl;
		return 1;
	}
	fileout << "/*\n * This file generated from Modulaetherschrift source.\n */\n";
	string strTemp;
	int functionflag = 0;
	int srcflag = 0;
	string content_core;
	vector<string> documents;
	vector<string> vSources;
	int curDocIndex = -1;
	int iSources = 0;
	int iBrackets = 0;
	int writetodoc = 0;
	while (filein >> strTemp) {
		char peek_char;
		peek_char = retChar(filein.peek());
		// Maintain carriage returns
		if (peek_char == 13 || peek_char == 10) {
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
			cout << "file " << strTemp << ".js created\n";
			//strTemp = "setTimeout($.getScript(\".\\"+strTemp+".js\").fail(function(){console.error(\"$.get failed on "+strTemp+".js!\")}), 5000);\n";
			strTemp = "$.getScript(\".\\"+trimCR(strTemp)+".js\");\n";
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
			strTemp += " ";
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
			strTemp += " ";
			
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
			strTemp += " ";
			if (filein.peek() == '\n') {
				strTemp += "\n";
			}
		}
		fileout << strTemp;
	}
	fileout.close();
	for (int i = 0; i < documents.size(); i++) {
		cout << "documents.size() block hit!\n";
		vSources[i] = trimCR(vSources[i]);
		char * filename = new char[vSources[i].length()+4];
		string sFilename = vSources[i] + ".js";
		strcpy(filename, sFilename.c_str());
		ofstream fileout(filename);
		if (!fileout) {
			cout << vSources[i] << ".js could not be opened.";
			return 1;
		}
		fileout << documents[i];
		fileout.close();
	}
	return 0;
}
