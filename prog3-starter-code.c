#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

//Defines a struct for the nodes of a linked list to store characters
typedef struct inToPostNode {
	char data;
	struct inToPostNode *next;
}charNode;

//Defines a struct for the nodes of a linked list to store integers
typedef struct evalNode {
	int data;
	struct evalNode *next;
}intNode;

//Removes and returns an element from the character stack for converting infix to postfix
char pop(struct inToPostNode** stack) {
	char val = (*stack)->data;
	struct inToPostNode* temp = *stack;
	(*stack) = (*stack)->next;
	free(temp);
	return val;
}

//Removes and returns an element from the integer stack for evaluating a postfix expression
int popEval(struct evalNode** stack) {
	int val = (*stack)->data;
	struct evalNode* temp = *stack;
	(*stack) = (*stack)->next;
	free(temp);
	return val;
}

//Adds an element to the character stack for converting an expression from infix to postfix
void push(char stored, struct inToPostNode** stack) {
	struct inToPostNode* node = (struct inToPostNode*)malloc(sizeof(struct inToPostNode));
	node->data = stored;
	node->next = (*stack);
	(*stack) = node;
}

//Adds an element to the integer stack for evaluating a postfix expression
void pushEval(int stored, struct evalNode** stack) {
	struct evalNode* node = (struct evalNode*)malloc(sizeof(struct evalNode));
	node->data = stored;
	node->next = (*stack);
	(*stack) = node;
}

//Reads data from a file and stores the data in a 2D array of integers
int **readFile(char *fileName, int rows, int cols) {
	FILE* file;
	int **arr = malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; i++) {
		arr[i] = malloc(cols * sizeof(int));
	}

	file = fopen(fileName, "r");
	
	//Reads integers from the file into a 2D array
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			fscanf(file, "%d", &arr[i][j]);
		}
	}
	fclose(file);
	return arr;
}

//Function to clear the memory allocated to a 2D array
void clearArr(int **arr, int rows) {
	for (int i = 0; i < rows; i ++) {
		free(arr[i]);
	}
	free(arr);
}

//Function that is used to determine the presedence of an operator
int precedence(char operator)
{
    switch (operator) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    default:
        return -1;
    }
}

//Converts an infix expression to a postfix expression
void infixToPostfix(char *infix, char* postfix, int parFlag) {
	charNode *stack = NULL;
	int i = 0;
	int j = 0;

	//Checks all possible characters in the infix string
	for (i = 0; infix[i] != '\0'; i++) {
		if (isspace(infix[i])) {
			continue;
		}
		if (isdigit(infix[i])) {
			postfix[j++] = infix[i];
		}
		else if (infix[i] == '(') { 
			push('(', &stack);
		}
		else if (infix[i] == ')') { //deals with all cases involving parentheses
			while (stack != NULL && stack->data != '(') {
				postfix[j++] = pop(&stack);
			}
			if (stack == NULL && infix[i] == ')' && parFlag == 1) {
				postfix[j] = '\0';
				return;
			}
			if (stack != NULL && stack->data == '(') {
				pop(&stack);
			}
		}
		else { //used to make sure the precedence is correct
			while (stack != NULL && precedence(infix[i]) <= precedence(stack->data)) {
				postfix[j++] = pop(&stack);
			}
			push(infix[i], &stack);
		}
	}
	while (stack != NULL) { //stores the final postfix expression
		postfix[j++] = pop(&stack);
	}
	postfix[j] = '\0'; //null character added to mark the end of the string
}

//Evaluates a postfix expression
int evaluateExpression(char *expression, int *opCounter) {
	intNode *stack = NULL;
	int i;
	int val1;
	int val2;
	int result;

	for (i = 0; expression[i] != '\0'; i++) { //loops until the end of the expression
		if (isdigit(expression[i])) {
			pushEval(expression[i] - '0', &stack); //pushes numbers to the integer stack after converting them from characters using ASCII
		}
		else {
			val2 = popEval(&stack);
			val1 = popEval(&stack);
			switch (expression[i]) //used to apply the correct operation on the 2 integers
			{
				case '+':
					result = val1 + val2;
					(*opCounter)++;
					break;
				case '-':
					result = val1 - val2;
					(*opCounter)++;
					break;
				case '*':
					result = val1 * val2;
					(*opCounter)++;
					break;
				case '/':
					result = val1 / val2;
					(*opCounter)++;
					break;
			}
			pushEval(result, &stack); 
			printf("%d %c %d = %d.\n", val1, expression[i], val2, result);
		}
	}
	result = popEval(&stack); //final result is removed from stack
	free(stack);
	return result;
}

//Helper function to check for extra parentheses in an expression and updates a flag based on the result
void parenthesisCheck(char *expression, int *parFlag) {
	charNode *stack = NULL;
	for (int i = 0; expression[i] != '\0'; i++) {
		if (expression[i] == '(') {
			push('(', &stack);
		}
		if (expression[i] == ')') {
			if (stack == NULL) {
				(*parFlag) = 1;
			} else if (stack->data == '(') {
				pop(&stack);
			}
		}
	}
	while (stack != NULL) {
		pop(&stack);
	}
}

//Function that takes care of running the actual game
int playGame(char *difficulty, int *replay, int random, int **arr, int *selectChoice) {
	int correctSum = 0;
	int tempSum = 0;
	int opCounter = 0;
	char input[100];

	if (*difficulty == 'M') { //difficulty check to know what file to read in from
		random = (rand() % 24);
		printf("The numbers to use are: %d, %d, %d, %d.\n", arr[random][0], arr[random][1], arr[random][2], arr[random][3]);
		correctSum = arr[random][0] + arr[random][1] + arr[random][2] + arr[random][3];
		printf("Enter your solution: ");
		scanf("%[^\n]", input);
		getchar();
	} else if (*difficulty == 'H') {
		random = (rand() % 11);
		printf("The numbers to use are: %d, %d, %d, %d.\n", arr[random][0], arr[random][1], arr[random][2], arr[random][3]);
		correctSum = arr[random][0] + arr[random][1] + arr[random][2] + arr[random][3];
		printf("Enter your solution: ");
		scanf("%[^\n]", input);
		getchar();
	} else {
		random = (rand() % 12);
		printf("The numbers to use are: %d, %d, %d, %d.\n", arr[random][0], arr[random][1], arr[random][2], arr[random][3]);
		correctSum = arr[random][0] + arr[random][1] + arr[random][2] + arr[random][3];
		printf("Enter your solution: ");
		scanf("%[^\n]", input);
		getchar();
	}
	
	//Check for invalid symbols
	for (int i = 0; input[i] != '\0'; i++) {
		if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '(' || input[i] == ')' || input[i] == ' ' || isdigit(input[i])) {

		} else {
			printf("Error! Invalid symbol entered. Please try again.\n\n");
			return (*replay);
		}
	}
	//Check for using all 4 numbers by making sure the sum of the numbers matches up
	for (int i = 0; input[i] != '\0'; i++) {
		if (isdigit(input[i])) {
			tempSum += input[i] - '0';
		}
	}
	//Compares sums to see if number error should be displayed
	if (tempSum != correctSum) {
		printf("Error! You must use all four numbers, and use each one only once. Please try again.\n\n");
		return (*replay);
	}

	char postfix[100];
	int result;
	int parFlag = 0;
	parenthesisCheck(input, &parFlag);
	infixToPostfix(input, postfix, parFlag);
	result = evaluateExpression(postfix, &opCounter);

	//Checks for the different possible outcomes of the program and the different errors that should be displayed
	if (opCounter != 3 && parFlag != 1) { 
		printf("Error! Too many values in the expression.\n\n");
		printf("Enter: \t1 to play again,\n");
		printf("\t2 to change the difficulty level and then play again, or\n");
		printf("\t3 to exit the program.\n");
		printf("Your choice --> ");
		scanf("%d", selectChoice);
		getchar();
	} else if (parFlag == 1) {
		printf("Error! Too many closing parentheses in the expression.\n\n");
		printf("Enter: \t1 to play again,\n");
		printf("\t2 to change the difficulty level and then play again, or\n");
		printf("\t3 to exit the program.\n");
		printf("Your choice --> ");
		scanf("%d", selectChoice);
		getchar();
	} else if (result == 24 && opCounter == 3 && parFlag == 0) {
		printf("Well done! You are a math genius.\n\n");
		printf("Enter: \t1 to play again,\n");
		printf("\t2 to change the difficulty level and then play again, or\n");
		printf("\t3 to exit the program.\n");
		printf("Your choice --> ");
		scanf("%d", selectChoice);
		getchar();
	} else {
		printf("Sorry. Your solution did not evaluate to 24.\n\n");
		printf("Enter: \t1 to play again,\n");
		printf("\t2 to change the difficulty level and then play again, or\n");
		printf("\t3 to exit the program.\n");
		printf("Your choice --> ");
		scanf("%d", selectChoice);
		getchar();
	}
	if (*selectChoice == 3) {
		return 0;
	}
	return 1;
}

int main() {
	int replay = 1;
	char difficulty;
	int selectChoice = 0;
	int random;
	int **arr;
	int rows;
	srand(1);

	printf("Welcome to the game of TwentyFour Part Two!\n");
	printf("Use each of the four numbers shown exactly once, \n");
	printf("combining them somehow with the basic mathematical operators (+,-,*,/) \n");
	printf("to yield the value twenty-four.\n");
	printf("Choose your difficulty level: E for easy, M for medium, and H for hard (default is easy).\n");
	printf("Your choice --> ");
	scanf("%c", &difficulty);
	getchar();

	//Read function is called based on the difficulty selected with easy being the default
	if (difficulty == 'H') {
		arr = readFile("hard.txt", 11, 4);
		rows = 11;
	} else if (difficulty == 'M') {
		arr = readFile("medium.txt", 24, 4);
		rows = 24;
	} else {
		arr = readFile("easy.txt", 12, 4);
		rows = 12;
	}
	while (replay == 1) { //Loop that handles the repeated playing of the game
		replay = playGame(&difficulty, &replay, random, arr, &selectChoice);
		if (selectChoice == 2) {
			printf("Choose your difficulty level: E for easy, M for medium, and H for hard (default is easy).\n");
			printf("Your choice --> ");
			scanf("%c", &difficulty);
			getchar();

			clearArr(arr, rows); //Clears arrays in between changing difficulties to prevent leaks
			if (difficulty == 'H') {
				arr = readFile("hard.txt", 11, 4);
				rows = 11;
			} else if (difficulty == 'M') {
				arr = readFile("medium.txt", 24, 4);
				rows = 24;
			} else {
				arr = readFile("easy.txt", 12, 4);
				rows = 12;
			}
		}
	}
	clearArr(arr, rows);
	printf("\nThanks for playing!\nExiting...");

	return 0;
}