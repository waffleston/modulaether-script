/*
 * modulaether-script
 * (c) 2017-2018 Brendyn Sonntag
 * Licensed under Apache 2.0
 */
#define VERSION "SNAPSHOT v0.1.57 (2018-mar-12)"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime> // Fix for windows.

using std::ifstream;
using std::ofstream;
using std::endl;
using std::string;
using std::cout;
using std::getline;

string transpileFile(const string&);
string getAesWord(const string&);
string getAesArg(const string&);
bool readAesArg(const string&);
string removeAllTabs(const string&);
string removeExcessSpaces(const string&);
void trimSpaceOffEach(string&, const string&);
void trimBeforeOrAfterEach(string&, const string&);
int count(const string&, const string&);
bool isEnclosedByQuotesInString(const string&, int);
string removeLineComment(const string&);
string readAesFileName(const string&);

// Neutralization characters.
#define NEUTRALIZED_SLASH char(1)
#define NEUTRALIZED_SPACE char(2)

//#define CRLF "\x0D\x0A"
#define CRLF "\x0A"

// TODO
// [ ] srcdef
// [ ] fn
// [x] thisfile
// [ ] defer
// [ ] root
// [x] insert (Added new inject keyword as counterpart.)
// [x] comments
// [x] creturn
// [x] spaces
// [x] tabs

int main(int argc, char* argv[])
{
   // Ensure that there's at least one argument.
   if (argc < 2)
   {
      cout << "Modulaether-script " << VERSION << "; Transpiles aes files into javascript files." << endl;
      cout << "Usage: maejs main_file [other_files]" << endl;
      return 1;
   }

   // There's at least one argument, so let's jump right in.
   for (int primaryFileCount = 1; primaryFileCount < argc; primaryFileCount++)
   {
      // Define the input file name.
      string inputFileName = argv[primaryFileCount];

      // Notify
      cout << inputFileName;

      // Operate on it.
      string output;
      output = transpileFile(inputFileName);

      // test for failure
      if (output.empty())
      {
         cout << endl;
         continue;
      }

      // Generate the output file name.
      string outputFileName = inputFileName;
      try
      {
         outputFileName = outputFileName.substr(0, outputFileName.find_last_of("."));
      }
      catch (...) { /* That's fine, the filename might not have a dot. */ }
      outputFileName = outputFileName + ".js";

      // Write content to file.
      ofstream outputFile(outputFileName.c_str());
      if (!outputFile)
      {
         cout << " {Could not create: " << outputFileName << "}";
         return 1;
      }

      // Get timestamp
      time_t rawTime;
      struct tm * currentTime;
      time(&rawTime);
      currentTime = localtime(&rawTime);
      string timeString = asctime(currentTime);
      timeString = timeString.substr(0,timeString.length()-1);

      outputFile << "/* Generated by Modulaether-Script at " << timeString << " */" << CRLF << output << CRLF;
      outputFile.close();

      // Notify
      cout << " -> " << outputFileName << endl;
   }
}

string transpileFile(const string& inputFileName)
{
   // Local Vars
   //
   string outputContents = "";
   string outputContentsLine = "";
   string alternateFileName = "";
   // Each aes file gets its own set of flags.
   bool creturn = true; // Add CR by default.
   bool tabs = true; // Allow tabs by default.
   bool spaces = true; // Don't trim whitespace by default.
   bool comments = true; // Show comments by default.

   // Internal Flags
   bool insideCommentBlock = false;
   bool skipReturn = false;

   // Open up the primary source file.
   ifstream sourceFile(inputFileName.c_str());

   // Verify that everything opened fine.
   if (!sourceFile)
   {
      cout << " {Could not read file: " << inputFileName << "}";
      return "";
   }
      
   // Read the file.
   while (!sourceFile.eof())
   {
      // Get the next line.
      getline(sourceFile, outputContentsLine);

      // remove CR from end.
      if (outputContentsLine[outputContentsLine.length() - 1] == char(13))
      {
         outputContentsLine = outputContentsLine.substr(0, outputContentsLine.length() - 1);
      }

      // Test for Existence of aes preprocessing command.
      if ((outputContentsLine.find("%^") == outputContentsLine.find_first_not_of(' ') && outputContentsLine.find("%^") != string::npos)
            || (outputContentsLine.find("%^") == outputContentsLine.find_first_not_of('\t') && outputContentsLine.find("%^") != string::npos))
      {
         string word = getAesWord(outputContentsLine);
         string condition = getAesArg(outputContentsLine);
         if (word == "%^creturn")
         {
            creturn = readAesArg(condition);
         }
         else if (word == "%^tabs")
         {
            tabs = readAesArg(condition);
         }
         else if (word == "%^spaces")
         {
            spaces = readAesArg(condition);
         }
         else if (word == "%^comments")
         {
            comments = readAesArg(condition);
         }
         else if (word == "%^insert")
         {
            outputContentsLine = outputContentsLine + transpileFile(readAesFileName(condition));
            cout << " [& " << readAesFileName(condition) << "]";

            // Trim first CR LF from end, if it exists.
            if (outputContentsLine[outputContentsLine.length() - 1] == char(10))
            {
               outputContentsLine = outputContentsLine.substr(0, outputContentsLine.length() - 1);
            }
            if (outputContentsLine[outputContentsLine.length() - 1] == char(13))
            {
               outputContentsLine = outputContentsLine.substr(0, outputContentsLine.length() - 1);
            }
         }
         else if (word == "%^inject")
         {
            outputContentsLine = transpileFile(readAesFileName(condition));
            cout << " [&(raw) " << readAesFileName(condition) << "]";
            // Send to the output and exit without doing normal minification.
            outputContents += outputContentsLine;
            continue;
         }
         else if (word == "%^thisfile")
         {
            skipReturn = true;
            alternateFileName = readAesFileName(condition);
         }
         else
         {
            cout << "Unknown command: " << word << " " << condition;
         }

         // Remove the command.
         outputContentsLine.erase(outputContentsLine.find(word+' '+condition),word.length()+1+condition.length());
      }

      if (insideCommentBlock)
      {
         if (outputContentsLine.find("*/") != string::npos && outputContentsLine.find("*/") + 2 < outputContentsLine.length())
         {
            insideCommentBlock = false;
            outputContentsLine = outputContentsLine.substr(outputContentsLine.find("*/")+2);
         }
         else if (outputContentsLine.find("*/") != string::npos)
         {
            insideCommentBlock = false;
            outputContentsLine = "";
         }
         else
         {
            outputContentsLine = "";
         }
      }

      // Remove comments if disallowed.
      if (!comments)
      {
         // Detect start of block comment.
         while (outputContentsLine.find("/*") != string::npos)
         {
            if (!isEnclosedByQuotesInString(outputContentsLine, outputContentsLine.find("/*")))
            {
               if (outputContentsLine.substr(outputContentsLine.find("/*")).find("*/") != string::npos)
               {
                  outputContentsLine = outputContentsLine.substr(0,outputContentsLine.find("/*")) + outputContentsLine.substr(outputContentsLine.find("/*")).substr(outputContentsLine.substr(outputContentsLine.find("/*")).find("*/") + 2);
               }
               else
               {
                  outputContentsLine = outputContentsLine.substr(0,outputContentsLine.find("/*"));
                  insideCommentBlock = true;
               }
            }
            else
               outputContentsLine[outputContentsLine.find("/*")] = NEUTRALIZED_SLASH; // Temporarily neutralize the `//`
         }

         // Repair the neutralized `/*`.
         while (outputContentsLine.find(NEUTRALIZED_SLASH) != string::npos)
         {
            outputContentsLine[outputContentsLine.find(NEUTRALIZED_SLASH)] = '/';
         }

         // Line comment.
         outputContentsLine = removeLineComment(outputContentsLine);
      }

      // Remove all tab characters if disallowed.
      if (!tabs)
      {
         outputContentsLine = removeAllTabs(outputContentsLine);
      }

      // Trim whitespace if disallowed.
      if (!spaces)
      {
         outputContentsLine = removeExcessSpaces(outputContentsLine);
      }

      // Append CR if allowed.
      if (creturn && !outputContentsLine.empty())
      {
         outputContentsLine += CRLF;
         //outputContentsLine += char(13);
         //outputContentsLine += char(10);
      }

      // Send to the output.
      outputContents += outputContentsLine;
   }

   // Close the file.
   sourceFile.close();

   if (skipReturn)
   {
      ofstream alternateFile (alternateFileName.c_str()); //, std::ios_base::app);
      alternateFile << "/* Generated by Modulaether-Script in batch job from " << inputFileName << " */" << CRLF << outputContents << CRLF;
      alternateFile.close();
      cout << " [-> " << alternateFileName << "]";
      return "";
   }
   // else
   return outputContents;
}

string getAesWord(const string& line)
{
   string temp = line;
   temp = temp.substr(temp.find("%^"), temp.length());
   try {
   temp = temp.substr(0, temp.find(" "));
   }
   catch (...) { /* Apparently there's no argument. */ }
   return temp;
}

string getAesArg(const string& line)
{
   string temp = line;
   temp = temp.substr(temp.find("%^"), temp.length());
   temp = temp.substr(temp.find(" ") + 1, temp.length());
   try
   {
      temp = temp.substr(0, temp.find(" "));
   }
   catch (...) { /* The line likely ends - which should be expected. */ }
   return temp;
}

bool readAesArg(const string& condition)
{
   if (condition == "off")
   {
      return false;
   }
   else if (condition == "on")
   {
      return true;
   }
   else
   {
      cout << "Invalid condition: `" << condition << "` on a logical word, defaulting to 'on'." << endl;
      return true;
   }
}

string removeAllTabs(const string& line)
{
   string temp = line;
   // As long as tab chars exist, remove them.
   while (temp.find('\t') != string::npos)
   {
      temp.erase(temp.find('\t'), 1);
   }

   return temp;
}

string removeExcessSpaces(const string& line)
{
   string temp = line;

   // Remove spaces around spaces.
   trimBeforeOrAfterEach(temp, " ");

   // Remove spaces around commas.
   trimBeforeOrAfterEach(temp, ",");

   // Remove spaces around semicolons.
   trimBeforeOrAfterEach(temp, ";");

   // Remove spaces around ternary symbols.
   trimBeforeOrAfterEach(temp, ":");
   trimBeforeOrAfterEach(temp, "?");

   // Remove spaces around logical.
   trimBeforeOrAfterEach(temp, "!");
   trimBeforeOrAfterEach(temp, "&");
   trimBeforeOrAfterEach(temp, "|");
   trimBeforeOrAfterEach(temp, "<");
   trimBeforeOrAfterEach(temp, ">");
   
   // Remove spaces around enclosures.
   trimBeforeOrAfterEach(temp, "(");
   trimBeforeOrAfterEach(temp, ")");
   trimBeforeOrAfterEach(temp, "{");
   trimBeforeOrAfterEach(temp, "}");
   trimBeforeOrAfterEach(temp, "[");
   trimBeforeOrAfterEach(temp, "]");

   // Remove spaces around math operators.
   trimBeforeOrAfterEach(temp, "-");
   trimBeforeOrAfterEach(temp, "+");
   trimBeforeOrAfterEach(temp, "/");
   trimBeforeOrAfterEach(temp, "*");
   trimBeforeOrAfterEach(temp, "=");
   trimBeforeOrAfterEach(temp, "%");

   return temp;
}

void trimSpaceOffEach(string& line, const string& constraint)
{
   // Determine postfix or prefix.
   int marker = constraint.find(' ');

   while (line.find(constraint) != string::npos)
   {
      line.erase(line.find(constraint) + marker, 1);
   }
}

void trimBeforeOrAfterEach(string& line, const string& constraint)
{
   //bool isEnclosedByQuotesInString(const string& line, int pointOfInterest)
   while (line.find(constraint+' ') != string::npos)
   {
      if (!isEnclosedByQuotesInString(line,line.find(constraint+' ')))
         line.erase(line.find(constraint+' ') + 1, 1);
      else
         line[line.find(constraint+' ') + 1] = NEUTRALIZED_SPACE;
   }
   // Repair the neutralized space.
   while (line.find(NEUTRALIZED_SPACE) != string::npos)
   {
      line[line.find(NEUTRALIZED_SPACE)] = ' ';
   }

   while (line.find(' '+constraint) != string::npos)
   {
      if (!isEnclosedByQuotesInString(line,line.find(' '+constraint)))
         line.erase(line.find(' '+constraint), 1);
      else
         line[line.find(' '+constraint)] = NEUTRALIZED_SPACE;
   }
   // Repair the neutralized space.
   while (line.find(NEUTRALIZED_SPACE) != string::npos)
   {
      line[line.find(NEUTRALIZED_SPACE)] = ' ';
   }
}

int count(const string& segment, const string& marker)
{
   int count = 0;
   string temp = segment;
   while (temp.find(marker) != string::npos)
   {
      temp = temp.substr(temp.find(marker)+marker.length());
      count++;
   }
   return count;
}

bool isEnclosedByQuotesInString(const string& line, int pointOfInterest)
{
   // Get number of quotes.
   int numDQuote = count(line.substr(0,pointOfInterest), "\"") - count(line.substr(0,pointOfInterest), "\\\"");
   //int numSQuote = count(line, "\'") - count(line, "\\\'");

   if (numDQuote%2 == 1)
      return true;
   //if (numSQuote%2 == 1)

   return false;
}

string removeLineComment(const string& line)
{
   string temp = line;
   while (temp.find("//") != string::npos)
   {
      if (!isEnclosedByQuotesInString(temp, temp.find("//")))
         temp = temp.substr(0,temp.find("//"));
      else
         temp[temp.find("//")] = NEUTRALIZED_SLASH; // Temporarily neutralize the `//`
   }

   // Repair the neutralized `//`.
   while (temp.find(NEUTRALIZED_SLASH) != string::npos)
   {
      temp[temp.find(NEUTRALIZED_SLASH)] = '/';
   }

   return temp;
}

string readAesFileName(const string& filename)
{
   if (filename.find(".") != string::npos)
   {
      return filename;
   }
   else
   {
      return filename + ".js";
   }
}
