#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

/* Look for all **'s and complete them */

//=====================================================
// File scanner.cpp written by: Group Number: 3
//=====================================================

// --------- Two DFAs ---------------------------------

// WORD DFA
// Done by: Farris Bader
// RE: RE = (vowel | vowel n | consonant vowel | consonant vowel n | consonant-pair vowel | consonant-pair vowel n)^+
bool word(string s)
{
    int state = 0; // initial state of DFA
    int charpos = 0; // character position in string

    // lambda function to check if character is a vowel
    auto is_vowel = [](char c) -> bool {
        c = tolower(c);  // convert to lowercase for case-insensitive check
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    };

   /* CD: removed this bc it accepts c as a consonant which is wrong
    * // lambda function to check if character is a consonant
    auto is_consonant = [is_vowel](char c) -> bool {
        c = tolower(c);  // Convert to lowercase for case-insensitive check
        return !is_vowel(c) &&  c != 'w'; //removed : c != 'y' &&
    };*/
    auto is_consonant = [](char c) -> bool {
        c = tolower(c); // Convert to lowercase for case-insensitive check

        string valid_consonants = "bdghjkmnprstwyz"; // list of valid single consonants

        // Check for valid single consonants
        return valid_consonants.find(c) != string::npos;
    };



    // loop through each character in string
    while (s[charpos] != '\0')
    {
        if (charpos == s.length() - 1 && (s[charpos] == 'I' || s[charpos] == 'E')) //dont lowercase these
        {
            break;
        }
        char currentChar = tolower(s[charpos]);  // Convert to lowercase for case-insensitive check
        //debug statement cout << "Current State: " << state << " Current Char: " << currentChar << endl;


        // Normal DFA transitions
        if (state == 0 && is_vowel(currentChar))
        {
            state = 1;
        }
        else if (state == 0 && currentChar == 'c' ) //special case ch
        {
            state = 3;
        }
        else if (state == 0 && is_consonant(currentChar))
        {
            state = 2;
        }
        else if (state == 1 && is_vowel(currentChar))
        {
            state = 1;
        }
        else if (state == 1 && currentChar == 'n')
        {
            state = 1;
        }
        else if (state == 1 && currentChar == 'c' ) //special case ch
        {
            state = 3;
        }
        else if (state == 1 && is_consonant(currentChar))
        {
            state = 2;
        }
        else if (state == 2 && is_vowel(currentChar))
        {
            state = 1;
        }
        else if (state == 2 && currentChar == 'y' && is_vowel(s[charpos+1]))
        {
            state = 1;
        }
        else if (state == 2 && currentChar == 'w' && is_vowel(s[charpos+1]))
        {
            state = 1;
        }
        else if (state == 2 && is_consonant(currentChar))
        {
            state = 2;
        }
        else if(state == 3 && is_consonant(currentChar)) //special case ch
        {
            state = 2;
        }
        else
        {
            return false;
        }

        charpos++;  // move to next character
    }

// checks final state
    return (state == 1 || s[s.length() - 1] == 'I' || s[s.length() - 1] == 'E');
}


// PERIOD DFA
// Done by: **
bool period(string s)
{ // complete this **
  return s == ".";
}

// ------ Three  Tables -------------------------------------

// TABLES Done by: Alex Shahtout

// ** Update the tokentype to be WORD1, WORD2, PERIOD, ERROR, EOFM, etc.
enum tokentype {WORD, RESWORD, WORD1, WORD2, PERIOD, ERROR, EOFM, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR};

// ** For the display names of tokens - must be in the same order as the tokentype.
string tokenName[30] = {"WORD", "RESERVED WORD", "WORD 1", "WORD 2", "PERIOD", "ERROR", "END OF FILE MARKER", "VERB", "NEGATIVE VERB", "PAST-TENSE VERB", "PAST-TENSE NEGATIVE VERB", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"};

// ** Need the reservedwords table to be set up here.
int const reservedWordsNum = 19;
string reservedWords[reservedWordsNum] = {"masu", "masen", "mashita", "masendeshita", "desu", "deshita", "o", "wa", "ni", "watashi", "anata", "kare", "kanojo", "sore", "mata", "soshite", "shikashi", "dakara", "eofm"};

tokentype reservedWordsTokens[reservedWordsNum] = {VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, PRONOUN, PRONOUN, PRONOUN, PRONOUN, CONNECTOR, CONNECTOR, CONNECTOR, CONNECTOR, EOFM};

// ** Do not require any file input for this. Hard code the table.
// ** a.out should work without any additional files.

// ------------ Scanner and Driver -----------------------

ifstream fin; // global stream for reading from the input file

// Scanner processes only one word each time it is called
// Gives back the token type and the word itself
// ** Done by: Alex Shahtout,
int scanner(tokentype &tt, string &w)
{
  fin >> w; // Grab the next word from the file via fin

  cout << "Processing: " << w << endl;

  // 1. If it is eofm, return right now.
  if (w == "eofm")
  {
    tt = EOFM;
    return 0; // word is end of file marker, normal return
  }


  for (int i = 0; i < reservedWordsNum; i++) //goes through all reserved words
    {
        if (w == reservedWords[i]) //if the word matches a reserved word...
        {
            tt = reservedWordsTokens[i]; //sets the corresponding tokentype
            cout << "\"" << w << "\" is token type " << tokenName[tt] << endl; // display token type
            return 0; //normal return, tokentype and w are passed down by reference
        }
    }

  /*2. Call the token functions (word and period)
  one after another (if-then-else).
  Generate a lexical error message if both DFAs failed.
  Let the tokentype be ERROR in that case. **/

  if (word(w)) // check if word matches the word DFA or period DFA
    {
        char lastChar = w[w.length() - 1]; 

        // determine if word is of type WORD1 or WORD2 based on its last character
        if (lastChar == 'a' || lastChar == 'e' || lastChar == 'i' || lastChar == 'o' || lastChar == 'u' || lastChar == 'n')
        {
            tt = WORD1;
        }
        else if (lastChar == 'I' || lastChar == 'E')
        {
            tt = WORD2;
        }
        cout << "\"" << w << "\" is token type " << tokenName[tt] << endl;
    }
  else if (period(w)) // checks if word is a period
    {
        tt = PERIOD;
        cout << "\"" << w << "\" is token type " << tokenName[tt] << endl;
    }
  else // else if word doesnt match any DFA, return error
    {
        cout << "LEXICAL ERROR :( ....." << endl;
        tt = ERROR;
    }

  /* 3. If it was a word,
  check against the reservedwords list.
  If not reserved, tokentype is WORD1 or WORD2
  decided based on the last character. */
  

  /* 4. Return the token type & string  (pass by reference). Also found elsewhere within this function*/
  return 0;

} // the end of scanner

// The temporary test driver to just call the scanner repeatedly
// This will go away after this assignment
// DO NOT CHANGE THIS!!!!!!
// Done by:  Louis
int main()
{
  tokentype thetype;
  string theword;
  string filename;

  cout << "Enter the input file name: ";
  cin >> filename;

  fin.open(filename.c_str());

  // the loop continues until eofm is returned.
  while (true)
  {
    scanner(thetype, theword); // call the scanner which sets
                               // the arguments
    if (theword == "eofm")
      break; // stop now

    cout << "Type is:" << tokenName[thetype] << endl;
    cout << "Word is:" << theword << endl;
      //add endl for console readability
      cout<< endl;
  }

  cout << "End of file is encountered." << endl;
  fin.close();

} // end
