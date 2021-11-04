/*
  codexter.c
  Interpreter for the codexter esolang created by June Stenzel (https://github.com/jsstenzel)
  Implementation by Alex Stenzel (https://github.com/ahstenzel)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <stack>
using namespace std;

// Queue of input data from the user
string inputQueue = "";

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
      if (chunk.find('#') != string::npos) {
        break;
      }

      // Save the chunk if its a numeric string
      if (isDigits(chunk)) {
        fullString += chunk;
      }
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
    cin >> in;
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
          if (d < 0) {
            i += 3;
          }
          else {
            write(&input, i, d);
          }
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
          if (pos != input.npos) {
            i = int(pos);
          }
        }
        break;
      case 4:
        // PUSH
        mem.push_front(read(input, i - 1));
        break;
      case 5:
        // POP
        if (mem.empty()) {
          i += 1;
        }
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
        if (read(input, i - 1) == 7) {
          flag = !flag;
        }
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
            for (int j = i - startCount; j >= 0; --j) {
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

int main(int argc, char* argv[]) {
  // Initialize
  cout << "+===========================+" << endl;
  cout << "| CODEXTER Interpreter v1.0 |" << endl;
  cout << "+===========================+" << endl;
  deque<string> inputStrings;

  // Parse arguments into seperate strings
  for (int i = 1; i < argc; ++i) {
    string input(argv[i]);
    if (input.find(".txt") != string::npos) {
      // Argument is a text file
      inputStrings.push_back(parseFile(input));
    }
    else {
      // Argument is a pure input
      inputStrings.push_back(input);
    }
  }

  // Evaluate each string
  for (const string& str : inputStrings) {
    cout << ">> " << str << endl;
    cout << parseString(str) << endl;
  }

  return 0;
}