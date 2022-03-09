/*
  codexter.cpp
  Interpreter for the codexter esolang created by June Stenzel (https://github.com/jsstenzel)
  Implementation by Alex Stenzel (https://github.com/ahstenzel)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <stack>
#include <limits>
using namespace std;

// Queue of input data from the user
string inputQueue = "";

// Output the state of the interpreter at every step
bool debugOutput = false;

/*
  itoc(i)
  Convert digit i to its corresponding character
 */
char itoc(int i) {
  return char(i) + '0';
}

/*
  ctoi(i)
  Convert character c to its corresponding digit
 */
int ctoi(char c) {
  return int(c - '0');
}

/*
  numDigits(i)
 */
int numDigits(int i) {
  int digits = 0; 
  do { 
    i /= 10; 
    digits++; 
  } while ( i != 0);
  return digits;
}

/*
  isDigits(str)
  Return true if the string contains only digits
 */
bool isDigits(string str) {
  return (str.find_first_not_of("0123456789") == string::npos);
}

/*
  parseFile(filename)
  Concatenates all the whitespace-seperated strings
  in the given text file
 */
string parseFile(string filename) {
  //Initialize
  string line, chunk, fullString;
  ifstream file(filename);
  
  // Error check
  if (!file) {
    cout << "ERROR: Failed to open file: " << filename << "!" << endl;
  }

  // Read entire line
  while (getline(file, line)) {
    // Seperate line by whitespace
    stringstream ss(line);
    while (ss >> chunk) {
      // Discard the rest of the line if theres a comment
      if (chunk.find('#') != string::npos) { break; }

      // Save the chunk if its a numeric string
      if (isDigits(chunk)) { fullString += chunk; }
    }
  }
  return fullString;
}

/*
  getInput()
  Get a single digit from cin and return it,
  or -1 if the input buffer is empty.
 */
int getInput() {
  // Get input
  if (inputQueue.empty()) {
    string in = "";
    cout << "Reading input...";
    getline(cin, in);
    inputQueue += in;
  }

  // Get the next valid char
  bool valid = false;
  int val = -1;
  do {
    // Get next char
    string s = inputQueue.substr(0, 1);
    inputQueue.erase(0, 1);

    // Check if it's valid
    if (isDigits(s)) {
      val = ctoi(s[0]);
      valid = true;
    }
  } while (!valid);
  return val;
}


/*
  read(string, pos)
  Returns the digit at the given point in the input,
  or zero if the position is out of bounds
 */
int read(const string& input, int pos) {
  return (pos < 0 || pos >= input.size()) ? 0 : (ctoi(input[size_t(pos)]));
}

/*
  write(string, pos, val)
  Writes the value to the string at the given
  position as long as its in bounds
 */
void write(string* input, int pos, int val) {
  if (pos >= 0 && pos < input->size()) { input->at(size_t(pos)) = itoc(val); }
}

/*
  outputState(input, output, pos, memory, flag)
  Output the current state of the parser and
  wait for user input before proceeding
 */
void outputState(const string& input, const string& output, const int& pos, const deque<int>& mem, const bool& flag) {
  /* Draw instruction tape section */
  // Top border
  cout << "[TAPE]";
  for (int i = 0; i < 44; ++i) { cout << "="; }
  cout << endl;
  // Tape values - these are stationary for the first 25 instructions, then scroll off to the left
  int tapeStartPos = (pos < 25) ? 0 : pos - 25;
  for (int i = tapeStartPos; i<int(input.size()); ++i) { cout << input[i]; }
  cout << endl;
  // Position marker - carat moves to the right for the first 25 instructions, then is stationary
  for (int i = 0; i < 50; ++i) {
	  if (i == pos && pos <= 25) { cout << "^"; }
	  else if (i == 25 && pos > 25) { cout << "^"; }
    else if ((i + tapeStartPos) % 5 == 0) { cout << "|"; }
    else { cout << " "; }
  }
  cout << endl;
  // Indices
  for (int i = 0; i < 50; ++i) {
    int p = i + tapeStartPos;
    if (p % 5 == 0) { 
      cout << p; 
      i += (numDigits(p) - 1);
    }
    else { cout << " "; }
  }
  cout << endl;
  // Bottom border
  for (int i = 0; i < 50; ++i) { cout << "="; }
  cout << endl;

  /* Draw stack section */
  cout << "[STACK] = ";
  string stack = "^{";
  for (auto& it : mem) {
    // Add elements to list
    stack += to_string(it) + ", ";
  }
  if (stack.size() > 2) {
    // Remove trailing commas
    stack = stack.substr(0, stack.size() - 2);
  }
  stack += "}";
  cout << stack << endl;

  /* Draw flag state */
  string state = (flag) ? "TRUE" : "FALSE";
  cout << "[FLAG] = " << state << endl;

  /* Draw output buffer state */
  cout << "[OUT] = " << output << endl;

  /* Wait for user */
  cout << "Press enter to continue...";
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  cout << endl;
  return;
}

/*
  parseString(input)
  Evaluates the numerical string digit by digit and
  produces an output.
 */
string parseString(string input) {
  // Error checking
  if (!isDigits(input)) {
    return "ERROR: String contains non-numeric characters!";
  }

  // Initialize
  deque<int> mem;
  bool flag = false;
  string output = "";

  // Step through characters
  for (int i = 0; i < int(input.size()); ++i) {
    // Debug state
    if (debugOutput) { outputState(input, output, i, mem, flag); }
    
    // Evaluate char
    int a = 0, b = 0, c = 0, d = 0;
    switch (read(input, i)) {
      case 0:
        // NOP
        break;
      case 1:
        // ADD
        a = read(input, i - 1);
        b = read(input, i - 2);
        write(&input, i, (a + b) % 10);
        break;
      case 2:
        // IO
        a = read(input, i + 1);
        b = read(input, i + 2);
        c = read(input, i + 3);
        d = ((a * 100) + (b * 10) + c) % 256;
        if (d == 127 || d < 32) {
          d = getInput();
          if (d < 0) { i += 3; }
          else { write(&input, i, d); }
        }
        else {
          output.push_back(char(d));
          a = (mem.empty()) ? 0 : mem.front(); 
          if (!mem.empty()) { mem.pop_front(); }
          b = (mem.empty()) ? 0 : mem.front();
          if (!mem.empty()) { mem.pop_front(); }
          c = (mem.empty()) ? 0 : mem.front();
          if (!mem.empty()) { mem.pop_front(); }
          write(&input, i + 1, a);
          write(&input, i + 2, b);
          write(&input, i + 3, c);
          i += 3;
        }
        break;
      case 3:
        // JUMP
        if (flag) {
          size_t pos = input.find_first_of('3', size_t(i) + 1);
          if (pos != input.npos) { i = int(pos); }
        }
        break;
      case 4:
        // PUSH
        mem.push_front(read(input, i - 1));
        break;
      case 5:
        // POP
        if (mem.empty()) { i += 1; }
        else {
          write(&input, i, mem.front());
          mem.pop_front();
        }
        break;
      case 6:
        // SKIP
        i += 1;
        break;
      case 7:
        // FLAG
        if (read(input, i - 1) == 7) { flag = !flag; }
        break;
      case 8:
        // ROTATE
        mem.push_back(mem.front());
        mem.pop_front();
        break;
      case 9:
        // LOOP
        if (flag) {
          // If you're at the end of a string of 9s,
          if (read(input, i + 1) != 9) {
            // Count the length of the string
            int startCount = 1;
            while (read(input, i - startCount) == 9) {
              startCount++;
            }
            // Find the previous string of 9s
            bool newString = false;
            int endCount = 0;
            for (int j = i - startCount; j >= -1; --j) {
              if (read(input, j) == 9) {
                // Counting the string length
                newString = true;
                endCount++;
              }
              else if (newString) {
                // Reached the end of the string
                if (endCount == startCount) {
                  // Earlier string is the same as the original string, so
                  // jump execeution to the end of the earlier string
                  i = j + endCount;
                  break;
                }
                else if (endCount > startCount) {
                  // Earlier string is longer than the original string, so
                  // carry on execution
                  break;
                }
                else {
                  // Earlier string is shorter than the original string, so
                  // keep looking for another string
                  endCount = 0;
                  newString = false;
                }
              }
            }
            // Reached the beginning of the input and didn't find an
            // equal length string of 9s, carry on execution
          }
        }
        break;
    }
  }
  
  // Return final output buffer
  return output;
}

/*
  printHeader()
  Print the interpreter header greeting.
 */
void printHeader() {
  string str = "CODEXTER Interpreter";
  #ifdef INTERPRETER_VERSION
  str += " v";
  str += INTERPRETER_VERSION;
  #endif

  cout << "+" << string(str.length() + 2,'=') << "+" << endl;
  cout << "| " << str << " |" << endl;
  cout << "+" << string(str.length() + 2,'=') << "+" << endl << endl;
}

/*
  printUsage()
  Print the command format for invoking the interpreter.
 */
void printUsage() {
  cout << "Usage: codexter [flags] <input file>" << endl;
  cout << "Flags:" << endl;
  cout << "  -h   Show this help menu" << endl;
  cout << "  -d   Execute step-by-step with debug output" << endl;
  cout << "  -v   Print interpreter version" << endl;
}

/*
  printHelp()
  Print a message showing flags and a brief overview
  of instructions.
 */
void printHelp() {
  printUsage();
  cout << "Overview:" << endl;
  cout << "  Codexter is an esolang that operates on strings of integers. Each non-flag" << endl;
  cout << "  argument to this program will be treated as a seperate standalone string to" << endl;
  cout << "  be parsed. Arguments can either be a literal number string or a path to a" << endl;
  cout << "  .txt file. All numeric strings in a file will be concatenated and treated as" << endl;
  cout << "  one large string. Any string containing non-digit characters is discarded." << endl;
}

int main(int argc, char* argv[]) {
  // Initialize
  deque<string> inputStrings;

  // Parse arguments into seperate strings
  if (argc == 1) {
    printUsage();
    return 1;
  }
  for (int i = 1; i < argc; ++i) {
    string input(argv[i]);
    if (input[0] == '-') {
      // Argument is a flag
      if (input == "-d") { 
        debugOutput = true; 
      }
      else if (input == "-h") {
        printHelp();
        return 0;
      }
      else if (input == "-v") {
        #ifdef INTERPRETER_VERSION
        cout << INTERPRETER_VERSION << endl;
        #endif
        return 0;
      }
      else {
        printUsage();
        return 1;
      }
    }
    else if (input.find(".txt") != string::npos) {
      // Argument is a text file
      inputStrings.push_back(parseFile(input));
    }
    else {
      // Argument is a pure input
      inputStrings.push_back(input);
    }
  }

  // Evaluate each string
  if (debugOutput) { printHeader(); }
  for (const string& str : inputStrings) {
    cout << ">> " << str << endl;
    cout << parseString(str) << endl;
  }

  return 0;
}