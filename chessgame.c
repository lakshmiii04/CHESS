#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define TILE 80
#define SIZE 640

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *tex[12];

int selR = -1, selC = -1;
int whiteTurn = 1;

int gameOver = 0;
int drawGame = 0;
char statusText[128] = "WHITE TURN";
int promotionMode = 0;
int promoR, promoC;
int promoWhite;

/* -------- WINDOW TITLE -------- */
void updateWindowTitle(){
 char full[256];
 sprintf(full,"%s  |  Press D = Draw  |  Press R = Restart",statusText);
 SDL_SetWindowTitle(window,full);
}

/* Board representation */
char board[8][8] = {
 {'r','n','b','q','k','b','n','r'},
 {'p','p','p','p','p','p','p','p'},
 {'.','.','.','.','.','.','.','.'},
 {'.','.','.','.','.','.','.','.'},
 {'.','.','.','.','.','.','.','.'},
 {'.','.','.','.','.','.','.','.'},
 {'P','P','P','P','P','P','P','P'},
 {'R','N','B','Q','K','B','N','R'}
};





/* ---------- PATH CHECK (NEW) ---------- */
int pathClear(int sr,int sc,int dr,int dc){
 int rstep = (dr>sr) - (dr<sr);
 int cstep = (dc>sc) - (dc<sc);

 sr += rstep;
 sc += cstep;

 while(sr!=dr || sc!=dc){
  if(board[sr][sc] != '.')
   return 0;
  sr += rstep;
  sc += cstep;
 }
 return 1;
}




/* ---------- DRAW PIECE ---------- */
void drawPiece(int r,int c,char p){
 if(p=='.') return;

 int index=-1;
 int white=isupper(p);

 switch(tolower(p)){
  case 'p': index=white?0:1; break;
  case 'r': index=white?2:3; break;
  case 'n': index=white?4:5; break;
  case 'b': index=white?6:7; break;
  case 'q': index=white?8:9; break;
  case 'k': index=white?10:11; break;
 }

 SDL_FRect dst={c*TILE,r*TILE,TILE,TILE};
 SDL_RenderTexture(renderer,tex[index],NULL,&dst);
}

/* ---------- MOVE RULES ---------- */
int validMove(char p,int sr,int sc,int dr,int dc){
 int r=dr-sr, c=dc-sc;
 char dest=board[dr][dc];

 if(dest!='.' && isupper(dest)==isupper(p))
  return 0;

 switch(tolower(p)){
  case 'p':
   if(isupper(p)){
    if(r==-1 && c==0 && dest=='.') return 1;
    if(sr==6 && r==-2 && c==0 && dest=='.') return 1;
    if(r==-1 && abs(c)==1 && dest!='.') return 1;
   } else {
    if(r==1 && c==0 && dest=='.') return 1;
    if(sr==1 && r==2 && c==0 && dest=='.') return 1;
    if(r==1 && abs(c)==1 && dest!='.') return 1;
   }
   break;

  case 'r':
   if(r==0 || c==0)
    return pathClear(sr,sc,dr,dc);
   return 0;

  case 'b':
   if(abs(r)==abs(c))
    return pathClear(sr,sc,dr,dc);
   return 0;

  case 'n': return (abs(r)==2 && abs(c)==1)||(abs(r)==1 && abs(c)==2);

  case 'q':
   if(r==0 || c==0 || abs(r)==abs(c))
    return pathClear(sr,sc,dr,dc);
   return 0;

  case 'k': return abs(r)<=1 && abs(c)<=1;
 }
 return 0;
}

int kingExists(int white){
 char k = white?'K':'k';
 for(int r=0;r<8;r++)
  for(int c=0;c<8;c++)
   if(board[r][c]==k) return 1;
 return 0;
}

int kingInCheck(int white){
 char king = white?'K':'k';

 int kr=-1,kc=-1;

 for(int r=0;r<8;r++)
  for(int c=0;c<8;c++)
   if(board[r][c]==king){ kr=r; kc=c; }

 if(kr==-1) return 0;

 for(int r=0;r<8;r++)
  for(int c=0;c<8;c++){
   char p=board[r][c];
   if(p!='.' && (isupper(p)!=white)){
    if(validMove(p,r,c,kr,kc))
     return 1;
   }
 }
 return 0;
}

/* ---------- DRAW BOARD ---------- */
void drawBoard(){
 for(int r=0;r<8;r++)
  for(int c=0;c<8;c++){
   SDL_FRect t={c*TILE,r*TILE,TILE,TILE};
   ((r+c)%2)?
    SDL_SetRenderDrawColor(renderer,181,136,99,255):
    SDL_SetRenderDrawColor(renderer,240,217,181,255);
   SDL_RenderFillRect(renderer,&t);

   if(r==selR && c==selC){
    SDL_SetRenderDrawColor(renderer,0,255,0,120);
    SDL_RenderFillRect(renderer,&t);
   }
   drawPiece(r,c,board[r][c]);
 }
}

/* ---------- MOUSE ---------- */
void handleClick(int x,int y){
 int r=y/TILE,c=x/TILE;

 if(selR==-1){
  char p=board[r][c];
  if(p!='.' && ((whiteTurn&&isupper(p))||(!whiteTurn&&islower(p)))){
   selR=r; selC=c;
  }
 }
 else{
  if(validMove(board[selR][selC],selR,selC,r,c) && !gameOver){

   board[r][c]=board[selR][selC];
   board[selR][selC]='.';
   // -------- PAWN PROMOTION CHECK --------
char moved = board[r][c];

if(tolower(moved)=='p' && (r==0 || r==7)){
    promotionMode = 1;
    promoR = r;
    promoC = c;
    promoWhite = isupper(moved);

    sprintf(statusText,"PROMOTION! Press Q/R/B/N");
    updateWindowTitle();
    selR=selC=-1;
    return;
}

   if(!kingExists(1)){
    sprintf(statusText,"BLACK WINS BY CHECKMATE!");
    gameOver=1;
   }
   else if(!kingExists(0)){
    sprintf(statusText,"WHITE WINS BY CHECKMATE!");
    gameOver=1;
   }
   else{
    whiteTurn=!whiteTurn;

    if(kingInCheck(whiteTurn))
     sprintf(statusText,"%s IN CHECK!",whiteTurn?"WHITE":"BLACK");
    else
     sprintf(statusText,"%s TURN",whiteTurn?"WHITE":"BLACK");
   }

   updateWindowTitle();
  }

  selR=selC=-1;
 }
}

/* ---------- MAIN ---------- */
int main(){
 SDL_Init(SDL_INIT_VIDEO);

 window=SDL_CreateWindow("SDL Chess Game",SIZE,SIZE,0);
 renderer=SDL_CreateRenderer(window,NULL);

 const char *files[12]={
"assets/wp.bmp","assets/bp.bmp",
"assets/wr.bmp","assets/br.bmp",
"assets/wn.bmp","assets/bn.bmp",
"assets/wb.bmp","assets/bb.bmp",
"assets/wq.bmp","assets/bq.bmp",
"assets/wk.bmp","assets/bk.bmp"
};

for(int i=0;i<12;i++){

    SDL_Surface *bmp = SDL_LoadBMP(files[i]);
    if(!bmp){
        printf("Missing image: %s\n",files[i]);
        exit(1);
    }

    // Convert to RGBA surface (supports transparency)
    SDL_Surface *rgba = SDL_ConvertSurface(bmp, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(bmp);

    // Enable blending
    SDL_SetSurfaceBlendMode(rgba, SDL_BLENDMODE_BLEND);

    // Remove white background (color key)
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(rgba->format);
    Uint32 white = SDL_MapRGB(fmt, NULL, 255,255,255);
    SDL_SetSurfaceColorKey(rgba, true, white);

    // Create texture
    tex[i] = SDL_CreateTextureFromSurface(renderer, rgba);
    SDL_DestroySurface(rgba);

    // Enable alpha blending in texture (IMPORTANT)
    SDL_SetTextureBlendMode(tex[i], SDL_BLENDMODE_BLEND);
}

 updateWindowTitle();

 int run=1; SDL_Event e;
 while(run){
  while(SDL_PollEvent(&e)){

   if(e.type==SDL_EVENT_QUIT)
    run=0;

   if(e.type==SDL_EVENT_MOUSE_BUTTON_DOWN)
    handleClick(e.button.x,e.button.y);

   if(e.type==SDL_EVENT_KEY_DOWN){
    // ---------- PROMOTION SELECTION ----------
if(promotionMode){
    char piece='Q';

    if(e.key.key=='q'||e.key.key=='Q') piece='Q';
    else if(e.key.key=='r'||e.key.key=='R') piece='R';
    else if(e.key.key=='b'||e.key.key=='B') piece='B';
    else if(e.key.key=='n'||e.key.key=='N') piece='N';
    else continue;;

    board[promoR][promoC] = promoWhite ? piece : tolower(piece);

    promotionMode=0;
    whiteTurn=!whiteTurn;

    if(kingInCheck(whiteTurn))
        sprintf(statusText,"%s IN CHECK!",whiteTurn?"WHITE":"BLACK");
    else
        sprintf(statusText,"%s TURN",whiteTurn?"WHITE":"BLACK");

    updateWindowTitle();
    continue;
}

    // DRAW MATCH (press D)
    if(e.key.key == 'd' || e.key.key == 'D'){
        drawGame=1;
        gameOver=1;
        sprintf(statusText,"GAME DRAW AGREED");
        updateWindowTitle();
    }

    // RESTART GAME (press R)
    if(e.key.key == 'r' || e.key.key == 'R'){

        memcpy(board,(char[8][8]){
          {'r','n','b','q','k','b','n','r'},
          {'p','p','p','p','p','p','p','p'},
          {'.','.','.','.','.','.','.','.'},
          {'.','.','.','.','.','.','.','.'},
          {'.','.','.','.','.','.','.','.'},
          {'.','.','.','.','.','.','.','.'},
          {'P','P','P','P','P','P','P','P'},
          {'R','N','B','Q','K','B','N','R'}
        },sizeof(board));

        gameOver=0;
        drawGame=0;
        whiteTurn=1;
        sprintf(statusText,"WHITE TURN");
        updateWindowTitle();
    }
   }
  }

  SDL_RenderClear(renderer);
  drawBoard();
  SDL_RenderPresent(renderer);
}

for(int i=0;i<12;i++)
    SDL_DestroyTexture(tex[i]);
 SDL_Quit();
 return 0;
}