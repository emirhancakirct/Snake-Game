
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#define STARTINGROW 5
#define STARTINGCOL 10

//struct
struct TROPHY{
    int row;
    int col;
    char value;
	int trophy_expiration;
};


//prototypes
void createBorder();
void dimensions();
void End_Game(int endcondition);
void set_dir(int ch);
void create_trophy();
void gameStart();
void trophy_expire(int);
void set_dir(int);
void enable_keyboardsignal();
void End_Game_handler(int);
//void run_every_second_check_trophy();

//Variables
int grow;
int dir;
static int windowRow = 0;
static int windowCol = 0;
int length;
static size_t rear;
static size_t front;
int game = 1;
struct TROPHY trophy = {0,0,'0',0};
int frontrow;
int frontcol;
int backrow;
int backcol;
time_t t;

void movesnake()
{
	if(grow < 1)
	{
		move(backrow,backcol);
		addch(' ');
		refresh();
	}
	else
	{
		grow -= 1;
		length += 1;
        if(length >= (((2 * (windowRow)) + (2* windowCol)) / 2))
        {
			End_Game(2);
        }
	}
	switch(dir)
	{
		case 1:
			frontcol += 1;
			break;
		case 2:
			frontcol -= 1;
			break;
		case 3:
			frontrow += 1;
			break;
		case 4:
			frontrow -= 1;
			break;
	}
	move(frontrow,frontcol);
	addch('o');
	refresh();
	usleep(100000);
}
int main()
{
   
	initscr(); //turns on curses
    curs_set(0); //turns off cursor
    noecho();               // do not echo the user input to the screen
	clear();

	signal(SIGIO, set_dir);
	enable_keyboardsignal();
	signal(SIGALRM, trophy_expire);
	
	keypad(stdscr,TRUE);    // enables working with the arrow keys
    srand((unsigned) time(&t));
	
    gameStart();
}
void gameStart()
{
	halfdelay(5);
    dimensions(); //gets screen dimensions
	//calls createBorder function... takes in window columns and rows
    createBorder(windowCol, windowRow);
	refresh(); //updates screen
	int placement[windowRow][windowCol];
    dir = 1;
    grow = 0;
    length = 5;
    rear = 1;
    front = 0;    
	
	//create snake
	for (int i = 0; i < 5; i++)
    {
		move(STARTINGROW,STARTINGCOL+i);
		addch('o');
		placement[STARTINGROW][STARTINGCOL+i] = 1;
		frontrow = STARTINGROW;
		frontcol = STARTINGCOL+i;
		
	}
	
	for (int i = 0; i< windowRow; i++)
	{
		placement[i][0] = 5;
		placement[i][windowCol-1] = 5;
	}
	for (int i = 0; i< windowCol; i++)
	{
		placement[1][i] = 5;
		placement[windowRow-1][i] = 5;
	}
	backrow = STARTINGROW;
	backcol = STARTINGCOL;
	while(placement[trophy.row][trophy.col] > 0)
	{
		create_trophy();
	}
    
	refresh();
    while(game)
	{
		int row = frontrow;
		int col = frontcol;
		movesnake();
		if (placement[frontrow][frontcol] > 0)
		{
			End_Game(1);
		}
		if ((trophy.row == frontrow) && (trophy.col == frontcol))
		{
			grow = 1;
			create_trophy();
		}
        grow = 0;
        mvaddch(trophy.row, trophy.col, trophy.value);
        //run_every_second_check_trophy();

		placement[frontrow][frontcol] = dir;
		placement[row][col] = dir;
		int old_back_dir= placement[backrow][backcol];
		placement[backrow][backcol] = 0;
		if(grow < 1)
		{
			switch(old_back_dir)
			{
				case 1:
					backcol += 1; 
					break;
				case 2:
					backcol -= 1; 
					break;
				case 3:
					backrow += 1; 
					break;
				case 4:
					backrow -= 1; 
					break;
			}
		}
	}
    grow = 0;
}
void set_dir(int signum)
{	
	int ch = getch();
	switch(ch)
	{
		case KEY_UP:
			dir = 4;
			break;
		case KEY_DOWN:
			dir = 3;
			break;
		case KEY_RIGHT:
			dir = 1;
			break;
		case KEY_LEFT:
			dir = 2;
			break;
	}
}



//function to draw the pit border....
void createBorder(int x, int y) {
    addstr("Snake...\n");
    //Nested for loop to draw the border
    // - character = x axis 
    // | charcater = y axis 
    for (int i = 1; i < y; i++) {
        for (int j = 0; j < x; j++) {
            move(i, j);
            if (i == 1 || i == y -1)
                addch('-');
            else if (j == 0 || j == x - 1)
                addch('|');
        }
    }
}

//function that gets the screen dimension (from book)
void dimensions()
{
    struct winsize wbuf;
    if (ioctl(0, TIOCGWINSZ, &wbuf) != -1 ) {
        windowRow = wbuf.ws_row; //assigns global row
        windowCol = wbuf.ws_col; //assigns global col
    }
}

void End_Game(int endcondition)
{
    game = 0;
    nocbreak();
    cbreak();
	signal(SIGIO, End_Game_handler);
    if(endcondition == 2)
    {
        clear();
        mvaddstr(10, 14, "You Win! Play again? Y or N");
        refresh();
		pause();
    }
    else {
        clear();
        mvaddstr(10, 14, "You Lost! Try again? Y or N");
        refresh();
		pause();
    }
	endwin();
}

void trophy_expire(int signum)
{
	signal(SIGALRM, trophy_expire);
	move(trophy.row, trophy.col);
    addch(' ');
	create_trophy();
	
	
}
// Creates a trophy struct with randomized position and value
void create_trophy() 
{
	trophy.trophy_expiration = (rand() % 10) + 1;
	trophy.row = (rand() % (windowRow-3)) + 2;
    trophy.col = (rand() % (windowCol-2)) + 1;
	trophy.value = ((rand() % 9) + 1) + '0';
	struct timeval expire = {trophy.trophy_expiration,0};
	struct itimerval time;
	time.it_value = expire;
	setitimer(ITIMER_REAL, &time, NULL);
	mvaddch(trophy.row, trophy.col, trophy.value);
	refresh();
}


int snake_speed()
{

	int biggerTerminalSpeedBonus;
	if(windowCol > 200)                                          
		biggerTerminalSpeedBonus = 50000;
	else if(windowCol > 100)
		biggerTerminalSpeedBonus = 25000;
	else 
		biggerTerminalSpeedBonus = 10000;
	
	if (length == 3)
		return biggerTerminalSpeedBonus;
}

void enable_keyboardsignal()
{
	int fd_flags;
	fcntl(0, F_SETOWN, getpid());
	fd_flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));
}
void End_Game_handler(int signum)
{
	int ch = getch();
	switch(ch)
	{
		case 'n':
			nocbreak();
            clear();
            refresh();
            game = 1;
            gameStart();
			break;
		case 'y':
			mvaddstr(10, 14, "game ended");
            refresh();
            endwin();
			break;
	}
	endwin();
}
