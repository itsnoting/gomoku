#include "pisqpipe.h"
#include <windows.h>

const char *infotext="name=\"Random\", author=\"Petr Lastovicka\", version=\"3.1\", country=\"Czech Republic\", www=\"http://petr.lastovicka.sweb.cz\"";

#define MAX_BOARD 100
int board[MAX_BOARD][MAX_BOARD];
static unsigned seed;

               
void brain_init() 
{
  if(width<5 || width>MAX_BOARD || height<5 || height>MAX_BOARD){
    pipeOut("ERROR size of the board");
    return;
  }
  seed=start_time;
  pipeOut("OK");
}

void brain_restart()
{
  int x,y;
  for(x=0; x<width; x++){
    for(y=0; y<height; y++){
      board[x][y]=0;
    }
  }
  pipeOut("OK");
}

int isFree(int x, int y)
{
  return x>=0 && y>=0 && x<width && y<height && board[x][y]==0;
}

void brain_my(int x,int y)
{
  if(isFree(x,y)){
    board[x][y]=1;
  }else{
    pipeOut("ERROR my move [%d,%d]",x,y);
  }
}

void brain_opponents(int x,int y) 
{
  if(isFree(x,y)){
    board[x][y]=2;
  }else{
    pipeOut("ERROR opponents's move [%d,%d]",x,y);
  }
}

void brain_block(int x,int y)
{
  if(isFree(x,y)){
    board[x][y]=3;
  }else{
    pipeOut("ERROR winning move [%d,%d]",x,y);
  }
}

int brain_takeback(int x,int y)
{
  if(x>=0 && y>=0 && x<width && y<height && board[x][y]!=0){
    board[x][y]=0;
    return 0;
  }
  return 2;
}

unsigned rnd(unsigned n)
{
  seed=seed*367413989+174680251;
  return (unsigned)(UInt32x32To64(n,seed)>>32);
}

void brain_turn() 
{
  int x,y,i;

  i=-1;
  for (int i = 0; i < MAX_BOARD; ++i) {
	  for (int j = 0; j < MAX_BOARD; ++j) {
		  if (board[j][i] == 2 && board[j + 1][i] == 2 && board[j + 2][i] == 2) {
			  if (j - 2 >= 0 && board[j - 2][i] == 2 && isFree(j - 1, i)) {
				  do_mymove(j - 1, i);
				  return;
			  }
			  if (j + 4 <= MAX_BOARD && board[j + 4][i] && isFree(j + 3, i)) {
				  do_mymove(j + 3, i);
				  return;
			  }
			  if (board[j + 4][i] == 2 && isFree(j + 4, i)) {
				  do_mymove(j + 4, i);
				  return;
			  }
			  if (!isFree(j-1, i) && isFree(j+3,i)){
				  do_mymove(j + 3, i);
				  return;
			  }
			  else if (isFree(j-1, i)){
				  do_mymove(j - 1, i);
				  return;
			  }
		  }
		  if (board[j][i] == 2 && board[j][i+1] == 2 && board[j][i+2] == 2) {
			  if (!isFree(j, i-1) && isFree(j, i+3)) {
				  do_mymove(j, i+3);
				  return;
			  }
			  else if (isFree(j, i-1)) {
				  do_mymove(j, i-1);
				  return;
			  }
		  }
		  if (board[j][i] == 2 && board[j + 1][i + 1] == 2 && board[j + 2][i + 2] == 2) {
			  if (!isFree(j - 1, i - 1) && isFree(j + 3, i + 3)) {
				  do_mymove(j + 3, i + 3);
				  return;
			  }
			  else if (isFree(j - 1, i - 1)) {
				  do_mymove(j - 1, i - 1);
				  return;

			  }
		  }
		  if (board[j][i] == 2 && board[j+2][i] == 2 && isFree(j+1, i)) {
			  do_mymove(j+1, i);
			  return;
		  }
		  if (board[j][i] == 2 && board[j][i + 2] == 2 && isFree(j,i+1)) {
			  do_mymove(j, i + 1);
			  return;
		  }
	  }
  }
  do{
    x=rnd(width);
    y=rnd(height);
    i++;
    if(terminate) return;
  }while(!isFree(x,y));

  if(i>1) pipeOut("DEBUG %d coordinates didn't hit an empty field",i);
  do_mymove(x,y);
}

void brain_end()
{
}

#ifdef DEBUG_EVAL
#include <windows.h>

void brain_eval(int x,int y)
{
  HDC dc;
  HWND wnd;
  RECT rc;
  char c;
  wnd=GetForegroundWindow();
  dc= GetDC(wnd);
  GetClientRect(wnd,&rc);
  c=(char)(board[x][y]+'0');
  TextOut(dc, rc.right-15, 3, &c, 1);
  ReleaseDC(wnd,dc);
}

#endif
