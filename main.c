#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// struct that represents a line in the test1.fsm
// file that defines the FSM
struct InputNextState {
    int state;
    char input;
    int nextState;
};

// struct of one of the possible 50 states of the FSM
// has the name which can be an int > 0
// and a list of the possible inputs
// Extra credit: allowing 52 possible inputs of
// ('a'-'z') and ('A'-'Z')
struct State {
    int name;
    int inputs[52];
};

// function that takes the letter of the input
// and returns its int value for the array
// of all possible inputs
// ex: 'a' = 0, 'b' = 1, 'z' = 25, 'A' = 26, 'Z' = 51
int getNumOfLetter(char letter){
    if (letter <= 'z' && letter >= 'a')
        return letter - 97;
    else if(letter <= 'Z' && letter >= 'A')
        return letter - 39;
    else
        return -1;
}

// counts the number of lines in the file
// useful to know how many different structs
// of "InputNextState" there will be
int countLines(char *filePath){
    char c;
    int count = 0;
    FILE* fp = fopen(filePath, "r"); /* should check the result */
    if (fp==NULL) {
        printf("File open is unsuccessful\n");
        return false;
    }
    for (c = getc(fp); c!= EOF; c = getc(fp)){
        if (c == '\n')
            count++;
    }
    fclose(fp);
    return count;
}

// checks if the element is already in the array
// so don't add duplicates
bool checkElem(struct State arr[], int elem, int size ) {
        for (int i=0; i<size; i++){
            if (arr[i].name == elem)
                return 1;
        }
        return 0;
}
// Looping through list of structs of arrInputStructs to make
// sure that the "nextState" values are possible states in the FSM
bool checkNextStateValues(int numLines, struct State arrOfStates[], struct InputNextState arrInputStructs[], int numStates){
    for (int i = 0; i < numLines; i++ ){
        bool stateExists = false;
        for (int j = 0; j < numStates; j++){
            printf("arrOfStates[y].name is %d and arrInputStructs[z].nextState is %d\n", arrOfStates[j].name, arrInputStructs[i].nextState);
            if (arrInputStructs[i].nextState == arrOfStates[j].name) {
                stateExists = true;
                break;
            }
        }
        // if one of the nextState values from arrInputStructs is not a valid state, will return false
        if (stateExists == false) {
            return false;
        }
    }
    return true;

}

// makes sure FSM has state zero
bool checkStateZero(int numStates, struct State arrOfStates[]){
    for (int j=0; j<numStates; j++){
        if (arrOfStates[j].name == 0)
            return true;
    }
    return false;
}

// Sets in the array of inputs for each state a default value of -1
// since that is not a valid state number.
// This will make it easier to check when get the inputs
// whether the input is valid and can go to another state.
void setDefaultNextStateVal(int numStates, struct State arrOfStates[]){
    for (int i=0; i<numStates; i++){
        for (int j = 0; j < 52; j++){
            arrOfStates[i].inputs[j] = -1;
        }
    }
}

// Adds all the information from the arrInputStructs into the State structs
void fillStructStates(int numLines, struct InputNextState arrInputStructs[], int numStates, struct State arrOfStates[]){
    for (int i=0; i<numLines; i++){
        int s = arrInputStructs[i].state;
        for (int j=0; j<numStates; j++){
            if (s == arrOfStates[j].name){
                int numChar = getNumOfLetter(arrInputStructs[i].input);
                arrOfStates[j].inputs[numChar] = arrInputStructs[i].nextState;
                printf("Now state %d has in its list of inputs[%d] the value  %d\n", s, numChar, arrInputStructs[i].nextState);
                break;
            }
        }
    }
}


// creates the FSM simulation
bool createFSM(char *defsFile, struct State arrOfStates[], int *numStates) {

    // get number of lines of file which is number of inputs > nextStates for test1.fsm
    int numLines = countLines(defsFile);

    FILE *fsmDef = fopen(defsFile, "r");

    // if file is empty leave function
    if (fsmDef == NULL) {
        printf("ERROR: File open is unsuccessful\n");
        return false;
    }

    printf("processing FSM definition file test1.fsm\n");
    printf("FSM has %d transitions\n",  numLines);

    int curState;
    int nextState;
    char input;

    // struct for each line of FSM inputs
    struct InputNextState arrInputStructs[numLines];

    int i = 0;

    // loops through each line in test1.inputs
    while (fscanf(fsmDef, "%d:%[^>]>%d", &curState, &input, &nextState) != EOF) {

        // checks to see if the states are valid
        if (curState < 0 || nextState < 0){
            printf("ERROR: There are no negative states this FSM will not work\n");
            return false;
        }

        // checks to see if the input is valid
        if (!((input <= 'z' && input  >= 'a') || (input >= 'A' && input <= 'Z'))){
            printf("Sadly since there is a non alphabetic character as input this FSM will not work\n");
            return false;
        }

        // counts how many unique states there are
        if (checkElem(arrOfStates, curState, *numStates) == 0) {
            arrOfStates[*numStates].name = curState;
            *numStates = *numStates + 1;
        }

        // adds all the values to the struct
        arrInputStructs[i].state = curState;
        arrInputStructs[i].nextState = nextState;
        arrInputStructs[i].input = input;

        i++;
    }
    fclose(fsmDef);

    int numStatesVal = *numStates;

    // check to see if any "nextState" value is not considered a valid state
    bool stateExists = checkNextStateValues(numLines, arrOfStates, arrInputStructs, numStatesVal);
    if (stateExists == false) {
            printf("ERROR: One of the next state values is not a valid state in the FSM\n");
            return false;
        }

    // Sets the default nextState for inputs array in the state structs to -1
    setDefaultNextStateVal(numStatesVal, arrOfStates);

    // Checks if there is a state of zero
    bool stateZero = checkStateZero(numStatesVal, arrOfStates);

    // If no state zero, not valid FSM
    if (stateZero == false){
        printf("Sadly since there is no state zero this FSM will not work\n");
        return false;
    }

    // Adds all the information from the arrInputStructs into the State structs
    fillStructStates(numLines, arrInputStructs, numStatesVal, arrOfStates);

    return true;
}

// Now uses the FSM simulation to detect the final state after list of inputs
bool useFSM(char *inputsFile, struct State arrOfStates[], int *numStates) {

    FILE *fsmInputs = fopen(inputsFile, "r");

    // if file is empty leave function
    if (fsmInputs == NULL) {
        printf("File open is unsuccessful\n");
        return false;
    }
    printf("processing FSM definition file test1.inputs\n");

    int myState = 0;
    int step = 0;
    char theInput;
    int prevState = 0;
    printf("number lines of test1.inputs is %d\n", countLines(inputsFile));

    // loop through test1.inputs to transition through FSM with the inputs
    while (fscanf(fsmInputs, "%c\n", &theInput) != EOF) {
        for (int y = 0; y < *numStates; y++) {
            if (arrOfStates[y].name == myState) {
                prevState = myState;
                myState = arrOfStates[y].inputs[getNumOfLetter(theInput)];
                if (myState == -1){
                    printf("ERROR: The input %c does not transition to a different state from state %d\n", theInput, prevState);
                    return false;
                }
                printf("  at step %d, input %c transitions FSM from state %d to state %d\n", step, theInput, prevState, myState);
                step++;
                break;
            }
        }
    }

    fclose(fsmInputs);
    printf("after %d steps, state machine finished successfully at state %d", step, myState);
    return true;
}

int main(int argc, char* argv[]) {

    if (argc != 3){
        printf("ERROR: incorrect number of arguments %d\n", argc);
    }
    else {
        struct State arrOfStates[50];
        int numStates;
        bool fsm = createFSM(argv[1], arrOfStates, &numStates);
        printf("created it.");
        printf("the number of states are: %d\n", numStates);

        if (fsm == true)
            useFSM(argv[2], arrOfStates, &numStates);
    }
}



