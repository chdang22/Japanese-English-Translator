#include<iostream>
#include<fstream>
#include<string>
#include <algorithm>
#include <cstdlib>
using namespace std;

/* INSTRUCTION:  Complete all ** parts.
   You may use any method to connect this file to scanner.cpp
   that you had written.  
  e.g. You can copy scanner.cpp here by:
          cp ../ScannerFiles/scanner.cpp .  
       and then append the two files into one: 
          cat scanner.cpp parser.cpp > myparser.cpp
*/
//=====================================================
// File scanner.cpp written by: Group Number: 3
//=====================================================

// --------- Two DFAs ---------------------------------

// WORD DFA
// Done by: Farris Bader
// RE: RE = (vowel | vowel n | consonant vowel | consonant vowel n | consonant-pair vowel | consonant-pair vowel n)^+
bool word(string s){
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
// Done by: Farris Bader
bool period(string s)
{ // complete this **
  return s == ".";
}

// ------ Three  Tables -------------------------------------

// TABLES Done by: Alex Shahtout

// ** Update the tokentype to be WORD1, WORD2, PERIOD, ERROR, EOFM, etc.
enum tokentype {WORD, RESWORD, WORD1, WORD2, PERIOD, ERROR, EOFM, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR};

// ** For the display names of tokens - must be in the same order as the tokentype.
string tokenName[30] = {"WORD", "RESERVED WORD", "WORD 1", "WORD 2", "PERIOD", "ERROR", "END OF FILE MARKER", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"};

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
// ** Done by: Alex Shahtout
int scanner(tokentype &tt, string &w)
{
    fin >> w; // Grab the next word from the file via fin

    cout << "Scanner called using word: " << w << endl;
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
            //cout << "\"" << w << "\" is token type " << tokenName[tt] << endl; // display token type
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
       // cout << "\"" << w << "\" is token type " << tokenName[tt] << endl;
    }
    else if (period(w)) // checks if word is a period
    {
        tt = PERIOD;
        //cout << "\"" << w << "\" is token type " << tokenName[tt] << endl;
    }
    else // else if word doesnt match any DFA, return error
    {
        cout << "LEXICAL ERROR :( ... " << w << " is not a valid token!" << endl;
        tt = ERROR;
    }

    /* 3. If it was a word,
    check against the reservedwords list.
    If not reserved, tokentype is WORD1 or WORD2
    decided based on the last character. */

    /* 4. Return the token type & string  (pass by reference). Also found elsewhere within this function*/
    return 0;

} // the end of scanner

//=================================================
// File parser.cpp written by Group Number: 3
//=================================================

// ----- Four Utility Functions and Globals -----------------------------------

// ** Need syntaxerror1 and syntaxerror2 functions (each takes 2 args)
//    to display syntax error messages as specified by me.  
ofstream errorFile("errors.txt", ios::trunc); // Open errors.txt (overwrite on)
bool tracingOn = false; // for tracing extra credit
bool token_available = false;

//moved here for syntax use
tokentype saved_token; //moved here for extra credit replace syntax1
string saved_lexeme;
bool match(tokentype expected); // declaration forwarding
// Type of error: Syntax error for when a match fails
// Done by: Alex Shahtout
void syntaxerror1(tokentype tt, string sl){
   //error message:
   //cout << "SYNTAX ERROR: expected " << tokenName[tt] <<  " but found " << sl << "." << endl;
   //exit(1); //closes program

   //extra credit
   // Write error message to both console and file
    string errorMessage = "SYNTAX ERROR: expected " + tokenName[tt] + " but found " + sl + ".";
    cout << errorMessage << endl;
    errorFile << errorMessage << endl;

    // Interact with user for error correction
    cout << "Do you want to (s)kip this token or (r)eplace it? (Enter 's' or 'r'): ";
    char choice;
    cin >> choice;

    if (choice == 's') {
        // Skip token: advance to next token in stream
        string skip = "Skipping token.";
        cout << skip << endl;
        errorFile << skip << endl;

        scanner(saved_token, saved_lexeme); // Fetch the next word
        token_available = true; // indicate new token fetched
        match(saved_token); // try to match new token
    } else if (choice == 'r') {
        // Replace token
        saved_token = tt;
        // Find the lexeme for the token type
        for (int i = 0; i < reservedWordsNum; ++i) {
            if (reservedWordsTokens[i] == tt) {
                saved_lexeme = reservedWords[i];
                break;
            }
        }
        string replace = "Replacing token with " + tokenName[tt] + " and " + saved_lexeme + ".";
        cout << replace << endl;
        errorFile << replace << endl;

        // Set token_available to false so next_token fetches this new token

        token_available = false;
    }
}


// Type of error: Syntax error for when a switch deafults
// Done by: Alex Shahtout
void syntaxerror2(string sl, string pf) {
 //error message:
 cout << "SYNTAX ERROR: unexpected " << sl <<  " found in " << pf << "." << endl;
 exit(1); //closes program
}

// ** Need the updated match and next_token with 2 global vars
// saved_token and saved_lexeme




// Purpose: Retrieve the next token from the scnner and save it to be returned
// Done by: Kerry Wilcher
tokentype next_token()
{
    if (!token_available) // no saved token yet
    {
        scanner(saved_token,saved_lexeme);//call scanner to grab new token,
                                                // saved_token is token type
                                                // saved_lexme is word read i
        token_available = true;                 // mark that have saved it
    }
    return saved_token;
}

// Purpose: to match expected token
// Done by: Farris Bader
bool match(tokentype expected) {
    // if token doesn't match expected token type, report error and exit
    if(next_token() != expected)
    {
        // calls syntax error function to gen synt error mssg and do recovery
        syntaxerror1(expected, saved_lexeme);
        return false;  // no match, error
    }
    else //match occur
    {
        //else if token matches then print success and return true
        token_available = false; // eat up token
        cout << "Matched " << tokenName[saved_token] << endl; // say there was match
        //scanner(saved_token, saved_lexeme);

        return true;

    }

}

// ----- RDP functions - one per non-term -------------------
//link for rules:

//https://docs.google.com/document/d/1jMfJwwL0wR7kk_1hS0hubv60uayAKMZbDxLehmabw64/edit?usp=sharing

// ** Make each non-terminal into a function here
// ** Be sure to put the corresponding grammar rule above each function
// ** Be sure to put the name of the programmer above each function
void afterSubject(); // Forward declaration
void afterNoun(); // Forward declaration
void afterObject(); // Forward declaration

// Grammar: <noun> ::= WORD1 | PRONOUN
// Done by: Alex Shahtout
void noun(){ //non-terminal function for "noun"
    if (tracingOn)
        cout << "Processing <noun>" << endl;
    switch(next_token()){ //switch for what the next token is between...
        case WORD1:
            match(WORD1);
            break;
        case PRONOUN:
            match(PRONOUN);
            break;
        default:
            syntaxerror2(saved_lexeme, "noun");
    }
}

// Grammar: <verb> ::= WORD2
// Done by: Farris Bader
void verb(){ //non-terminal function for "verb"
    if (tracingOn)
        cout << "Processing <verb> " << endl; // display non terminal being processed
    // checks if next token is WORD2. if so, then match with expected token type
    if(next_token() == WORD2) 
    {
        match(WORD2);
    }
    else
    {
        // if next token is not WORD2 then report error
        syntaxerror2(saved_lexeme, "verb");
    }
}

// Grammar: <be> ::= IS | WAS
// Done by: Kerry Wilcher
void be()
{
    //non-terminal function for "be"
    if (tracingOn)
        cout << "Processing <be>" << endl;
    switch (next_token())
    {
        case IS:
            match(IS); //match IS token
            break;
        case WAS:
            match(WAS); // match WAS token
            break;
        default: // to handle syntax error is neither IS or WAS
            syntaxerror2(saved_lexeme, "be"); 

    }
}

// Grammar: <tense> := VERBPAST | VERBPASTNEG | VERB | VERBNEG
// Done by: Kerry Wilcher
void tense()
{
    //non-terminal function for "tense"
    if (tracingOn)
        cout << "Processing <tense>" << endl;
    switch (next_token())
    {
        case VERBPAST:
            match(VERBPAST); // match VERBPAST token
            break;
        case VERBPASTNEG:
            match(VERBPASTNEG); // match VERBPASTNEG token
            break;
        case VERB:
            match(VERB); // match VERB token
            break;
        case VERBNEG:
            match (VERBNEG); // match VERBNEG token
            break;
        default: // handle synta x error if none of the expected tokens
            syntaxerror2(saved_lexeme, "tense");
    }
}

// Grammar: <s> ::= [CONNECTOR] <noun> SUBJECT <after subject>
// Done by: Caroline Dang
void s() {
    if (tracingOn)
        cout << "Processing <s>" << endl; // Tracing statement
    if (next_token() == CONNECTOR) { // if optional [CONNECTOR]
        match(CONNECTOR);
    }
    noun();
    match(SUBJECT);
    afterSubject();
}
// Grammar: <after subject> ::= <verb> <tense> PERIOD | <noun> <after noun>
// Done by: **Caroline Dang
void afterSubject() {
    if (tracingOn)
        cout << "Processing <afterSubject>" << endl;
    // IF after subject token =  <verb> ::= WORD2, then expect
    //<after subject> ::= <verb> <tense> PERIOD
    if (next_token() == WORD2) {
        verb(); // Non-terminal so call verb to match <verb> ::= WORD2,
        tense(); // Non-terminal so call tense to match <tense> := VERBPAST | VERBPASTNEG | VERB | VERBNEG
        match(PERIOD); // Terminal
    } else if (next_token() == WORD1 || next_token() == PRONOUN) { // if after subject is <noun> ::= WORD1 | PRONOUN, expect
    // <after subject> ::= <noun> <after noun>
        noun(); //<noun> ::= WORD1 | PRONOUN
        afterNoun(); //call helper funct, <after noun> ::= <be> PERIOD | DESTINATION <verb> <tense> PERIOD | OBJECT <after object>
    } else {
        syntaxerror2(saved_lexeme, "<s>");
    }
}
// Grammar: <after noun> ::= <be> PERIOD | DESTINATION <verb> <tense> PERIOD | OBJECT <after object>
// Done by: **Caroline Dang
void afterNoun() {
    if (tracingOn)
        cout << "Processing <afterNoun>" << endl;
    //if token after noun <be> ::= IS | WAS, then expect:
    //<after noun>::= <be> PERIOD
    if (next_token() == IS || next_token() == WAS ){
        be(); // 9 <be> ::= IS | WAS
        match(PERIOD); // terminal PERIOD
    } else if (next_token() == DESTINATION) { //if token after noun = DESTINATION then expect:
    // <after noun> ::= DESTINATION <verb> <tense> PERIOD

        match(DESTINATION); //terminal
        verb(); // 8 <verb> ::= WORD2
        tense(); // 10 <tense> := VERBPAST | VERBPASTNEG | VERB | VERBNEG
        match(PERIOD); //terminal

    } else if (next_token() == OBJECT) { // if token after noun = OBJECT then expect:
    //<after noun> ::= OBJECT <after object>
        match(OBJECT); //does match terminal OBJECT?
        afterObject(); // non terminal <after object> ::= <verb>  <tense> PERIOD | <noun> DESTINATION <verb> <tense> PERIOD
    } else {
        //no matching found
        syntaxerror2(saved_lexeme, "<afterNoun>");
    }
}
// Grammar: <after object> ::= <verb>  <tense> PERIOD | <noun> DESTINATION <verb> <tense> PERIOD
// Done by: **Caroline Dang
void afterObject() {
    if (tracingOn)
        cout << "Processing <afterObject>" << endl;
    // if token after object is  <verb> ::= WORD2  then expect:
    // <after object> ::= <verb> <tense> PERIOD
    if (next_token() == WORD2 ) {
        verb(); // non terminal <verb> ::= WORD2
        tense(); // non terminal <tense>
        match(PERIOD); // terminal PERIOD

    }
    // if token after object = <noun> ::= WORD1 | PRONOUN then:
    // <noun> DESTINATION <verb> <tense> PERIOD
    else if (next_token() == WORD1 || next_token() == PRONOUN) {
        noun();  //7 <noun> ::= WORD1 | PRONOUN
        match(DESTINATION); //Terminal DESTINATION
        verb(); // 8 <verb> ::= WORD2
        tense(); // 10 <tense> := VERBPAST | VERBPASTNEG | VERB | VERBNEG
        match(PERIOD);
    }
    // else no match for <afterObject>, error
    else {
        syntaxerror2(saved_lexeme, "<afterObject>");
    }
}




// Grammar: <story> ::= <s> { <s> }
// Done by: Caroline Dang
void story() {
    if (tracingOn)
        cout << "Processing <story>" << endl << endl;
    do {
        s(); // Process <s>
    } while (next_token() == CONNECTOR || next_token() == WORD1 || next_token() == PRONOUN);
    cout << "Successfully parsed <story>." << endl;
}

string filename;

//----------- Driver ---------------------------

// The new test driver to start the parser
// Done by:  **Caroline Dang
int main(){
    //----tracing option----
    char traceChoice;
    cout << "Do you want to turn tracing on? (y/n): ";
    cin >> traceChoice;

    if (traceChoice == 'y' || traceChoice == 'Y') {
        tracingOn = true;
    }
    cout << "Enter the input file name: ";
    cin >> filename;
    fin.open(filename.c_str());

 //** calls the <story> to start parsing
    story();

    fin.close(); //** closes the input file

}// end
//** require no other input files!
//** syntax error EC requires producing errors.txt of error messages
//** tracing On/Off EC requires sending a flag to trace message output functions
