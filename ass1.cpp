#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
using namespace std;

//This defines the parseFunciton as a type which takes a vector and returns a bool
using parseFunction = bool (*)(const vector<string>&);

//This is the id umap which is used as a global variable due to it being used in many functions
unordered_map<string, string> umap2;

//This function takes a line and breaks it up into its individual tokes using a wide range of checks which make sure that the command is being correctly tokenized
vector<string> tokenize(const string& line, int check) {
    vector<string> tokens;
    int indx;
    string phrase = "";
    for (indx = 0; indx < line.size(); indx++) {
        if (line[indx] != '"') {
            if (line[indx] != ' ' && line[indx] != ';' && line[indx] != '\n') {
                phrase = phrase + line[indx];
            } else if (line[indx] == ' ') {
                tokens.push_back(phrase);
                phrase.clear();
                continue;
            } else if (line[indx] == ';') {
                tokens.push_back(phrase);
                tokens.push_back(";");
                phrase.clear();
                continue;
            } else if (line[indx] == '\n') {
                if (indx + 1 == line.size()) {
                    continue;
                }
                tokens.push_back(phrase);
                phrase.clear();
                continue;
            }
        } else {
            indx++;
            phrase = phrase + '"';
            while (line[indx] != '"') {
                phrase = phrase + line[indx];
                if (line[indx + 1] == '\\') {
                    phrase = phrase + '"';
                    indx = indx + 2;
                }
                indx++;
            }
            phrase = phrase + '"';
            tokens.push_back(phrase);
            phrase.clear();
        }
    }
    if (check == 1) {
        tokens.push_back(phrase);
        return tokens;
    } else {
        return tokens;
    }
}

//this function removes double quotes from experssion which are verified to have a double quote at the start and end.
string outerQuotes_remove(const string& input) {
    return input.substr(1, input.size() - 2);
}

//This function extracts expressions from given id's or constants by checking the constant umap and the id umap
bool extract_expression(const string& token, string& expression) {
    unordered_map<string, string> constant_umap = {
        {"SPACE", " "},
        {"TAB", "\t"},
        {"NEWLINE", "\n"},
    };
    if (constant_umap.find(token) != constant_umap.end()) {
        expression = expression + constant_umap.find(token) -> second;
        return true;
    } else if (umap2.find(token) != umap2.end()) {
        expression = expression + umap2.find(token) -> second;
        return true;
    } else {
        cout << "Error: No such id: " << token << " exists" << endl;
        return false;
    }

}

//This function is responsible for checking id's or constants and adding their corresponding expressions to the main expression which will be put in the target id
//It also makes sure to check for double quotes or other characters to make sure that the main expression is correctly created
bool parseAlgo(const vector<string>& tokens, string & expression) {
    string id;
    int counter = 0;
    int indx;
    for (indx = 2; indx < tokens.size(); indx++) {
        id = tokens[indx];
        if (id == ";") {
            break;
        }
        if (id != "+" && counter == 1 && id != "") {
            cout << "Error: expected concatenation" << endl;
            return false;
        } else if (id != "+" && counter == 0 && id[0] != '"' && id[0] !='\n' && id != "") {
            if (!extract_expression(id, expression)) {
                 return false;
            } else {
                counter = 1;
             }   
        } else if (id[0] == '"' || id[0] == '\n') {
            expression = expression + outerQuotes_remove(id);
            counter = 1;
        } else if (id == "") {
            continue;
        } else {
            counter = 0;
            continue;
        }
    }
    return true;
}

//This function is responsible for appending expressions to already existing id's using a wide range of functions
bool parseAppend(const vector<string>& tokens) {
    string expression;
    if (umap2.find(tokens[1]) != umap2.end()) {
        if (tokens[2][0] == '"' && tokens[4] != "+") {
            umap2[tokens[1]] = umap2[tokens[1]] + outerQuotes_remove(tokens[2]);
            return true;
        } 
        if (!parseAlgo(tokens, expression)) {
            return false;
        } else {
            umap2[tokens[1]] = umap2[tokens[1]] + expression;
        }
    } else {
        cout << "Error: no such id: " << tokens[1] << endl;
        return false;
    }
    return true;
}

//This function lists all of the id's and their experssions by going through the start of the umap to the end
bool parseList(const vector<string>& tokens) {
    if (umap2.size() == 0) {
        cout << "Error: the identifier list is empty" << endl;
        return false;
    } else {
        vector<string> id;
        vector<string> expression;
        cout << "Identifier list (" << umap2.size() << "):" << endl; 
        for (auto indx = umap2.begin(); indx != umap2.end(); indx++) {
            cout << indx -> first << ": " << indx -> second << endl;
        }

        return true;
    }
    return true;
}

//This function causes the program to exit;
bool parseExit(const vector<string>& tokens) {
    exit(10);
    return false;
}

//This function is used to strip words of all their special characters leaving behind only the permitted characters
vector<string> word_algo(const vector<string>& words) {
    vector<string> finish;
    string word;
    for (int indx = 0; indx < words.size(); indx++) {
        word = "";
        for (int i = 0; i < words[indx].size(); i++) {
            if ((words[indx][i] >= 65 && words[indx][i] <=90) || (words[indx][i] >=97 && words[indx][i] <=122) || (words[indx][i] >=48 && words[indx][i] <=57) || (words[indx][i] ==39)) {
                word = word + words[indx][i];
            } 
        }
        if (word != "") {
            finish.push_back(word);
        }
    }
    return finish;
}

//This function is used to take an expresion and separate it into individual words using for loops which check for individual character which signify how things need to be separated
vector<string> separate_words(string expression) {
    vector<string> words_unfinished = tokenize(expression, 1);
    vector<string> words_finshed;
    vector<string> words_final;
    string word = "";
    int count;
    for (int indx = 0; indx < words_unfinished.size(); indx++) {
        word = "";
        for (int j = 0; j < words_unfinished[indx].size(); j++) {
            if (words_unfinished[indx][j] == '\n') {
                words_final.push_back(word);
                word = "";
                count = 1;
                j++;
            }
            word = word + words_unfinished[indx][j];
        }
        if (count == 1) {
            words_final.push_back(word);
            count = 0;
            continue;
        }
        if (words_unfinished[indx][0] == '"') {
            words_finshed = tokenize(outerQuotes_remove(words_unfinished[indx]), 1);
            for (int i = 0; i < words_finshed.size(); i ++) {
                words_final.push_back(words_finshed[i]);
            }
        } else {
            words_final.push_back(words_unfinished[indx]);
        }
    }
    return words_final;
}

//This function is responsible for all the print commands using many functions. It checks the print command and sends it to the correct function
bool parsePrint(const vector<string>& tokens) {
    string id = tokens[1];
    int indx;
    if (umap2.find(id) != umap2.end()) {
        string expression = umap2.find(id) -> second;
        if (tokens[0]== "print") {
            cout << expression << endl;
            return true;
        } else if (tokens[0] == "printlength") {
            cout << "Length is: " << expression.size() << endl;
            return true;
        } else if (tokens[0] == "printwords") {
            vector<string> words = word_algo(separate_words(expression));
            cout << "words are: " << endl;
            for (indx = 0; indx < words.size(); indx++) {
                cout << words[indx] << endl;
            }
            return true;
        } else if (tokens[0] == "printwordcount") {
            vector<string> words = word_algo(separate_words(expression));
            cout << "Wordcount is: " << words.size() << endl;
            return true;
        }
    } else {
        cout << "Error: no such id: " << id << endl;
        return false;
    }
    return true;
}

//this function is used in the set function being the very base case when setting a function
void basic_set(string main_id, string expression) {
    umap2[main_id] = expression;
}

//this function sets ids to expressions by using a range of functions
bool parseSet(const vector<string>& tokens) {
    
    if (tokens[2][0] == '"' && tokens[4] != "+") {
        basic_set(tokens[1], outerQuotes_remove(tokens[2]));
        return true;
    } else {
        string main_id = tokens[1];
        string expression;
        if (!parseAlgo(tokens, expression)) {
            return false;
        } else {
            basic_set(main_id, expression);
        return true;
        }
    }
}

//This function reverses the order of a expression by taking the expression out of a id tokenizing it and putting it into a phrase in reverse order
bool parseReverse(const vector<string>& tokens) {
    if (tokens.size() > 3) {
        cout << "Error: Ivalid use of command" << endl;
        return false;
    } else if (umap2.find(tokens[1]) != umap2.end()) {
        string phrase = "";
        string expression = umap2.find(tokens[1]) -> second;
        vector<string> words = tokenize(expression, 1);
        for (int indx = words.size() - 1; indx != 0; indx--) {
            phrase = phrase + words[indx];
            phrase = phrase + " ";
        }
        phrase = phrase + words[0];
        umap2.find(tokens[1]) -> second = phrase;
        return true;
    } else {
        cout << "Error: no such id: " << tokens[1] << endl;
        return false;
    }
}

//this function is responsible for executing the correcting functionsby running the tokens through a umap and comapring which one matches with its destined function
bool execute(const string& command) {
    vector<string> tokens = tokenize(command, 0);
    int indx;
    if (tokens[tokens.size() - 1] != ";") {
        cout << "Error: The command is not valid try again " << endl;
        return false;
    }
    unordered_map<string, parseFunction> umap = {
        {"append", parseAppend},
        {"list", parseList},
        {"exit", parseExit},
        {"print", parsePrint},
        {"printlength", parsePrint},
        {"printwords", parsePrint},
        {"printwordcount", parsePrint},
        {"set", parseSet},
        {"reverse", parseReverse}
    };

    if (umap.find(tokens[0]) != umap.end()) {
        return umap.find(tokens[0])->second(tokens);
    } else {
        cout << "Error: The token is not valid try again " << endl;
        return false;
    }

}

//This is the main function which begins when the program starts, it takes lines out of input and waits until the ; is used at the end to signify the ending of a command
int main() {
    cout << "----------------------------------------" << endl;
    cout << " 159.341 Assignment 1 Semester 1 2024 " << endl;
    cout << " Submitted by: Michael Polianski, 22011942 " << endl;
    cout << "----------------------------------------" << endl;
    string command;
    stringstream input_buffer;

    while (true) {
        string line;
        getline(cin, line);
        input_buffer << line << '\n';

        if (line.back() == ';') {
            command = input_buffer.str();
            input_buffer.str("");
            input_buffer.clear();
            execute(command);
        }
    }
}