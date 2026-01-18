#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <ncurses.h>

#define pi 3.14159265358979

int gh=100,gw=100; //SET THESE TWO VALUES
int grid[100][100];//TO THE SAME AS THESE VALUES FOR THE MAZE SIZE DEFAULT 100X100
float px, py, fx, fy, pa = 0, fov = pi*85/180, ws = 1.8, ts = 2.5; //MODIFY WS FOR WALK SPEED AND TS FOR TURN SPEED FOV FOR THE FIELD OF VIEW IN RADIANS
int pdir=0, m=0, f=0;
struct timespec start,passed;
unsigned int tp;

void clearmaze(){
	for(int y = 0; y < gh; y++){
		for(int x = 0; x < gw; x++){
			grid[y][x] = 1;
		}
	}
	m=0;
	pa=0;
}

bool good(int x, int y){
	int o = 0;
	if(x > 0 && y > 0 && x < gw-1 && y < gh-1){
		if(grid[y+1][x] != 0) o++;
		if(grid[y-1][x] != 0) o++;
		if(grid[y][x+1] != 0) o++;
		if(grid[y][x-1] != 0) o++;
	}
	if(o == 3 && grid[y][x] != 0) return true;
	else return false;
}

void genmaze(){
	clearmaze();
	srand(time(NULL));
	int x = 1, y = rand()%(gh-2)+1;
	bool end = false, search = false;
	px = x+0.5; py = y+0.5;
	grid[y][x] = 0;
	while(end == false){
		if(good(x+1,y) || good(x-1,y) || good(x,y+1) || good(x,y-1)){
			int dir = 4;
			while(dir == 4){int ran = rand()%4;
				if(ran == 0 && good(x+1,y)) dir = ran;
				else if(ran == 1 && good(x,y+1)) dir = ran;
				else if(ran == 2 && good(x-1,y)) dir = ran;
				else if(ran == 3 && good(x,y-1)) dir = ran;}
			switch(dir){case 0:x++;break;
				case 1:y++;break;
				case 2:x--;break;
				case 3:y--;break;}
			grid[y][x] = 0;
		}
		else{
			x = 1; y = 1;
			search = true;
			while(search == true){
				if(good(x,y)){grid[y][x] = 0; search = false;}
				else x++;
				if(x == gw-1){x = 1; y++;}
				if(y == gh-1){search = false; end = true;}
			}
		}
	}
	fx = gw-1.5;
	fy = rand()%(gh-2)+1.5;
	while(grid[(int)fy][(int)fx] != 0)fy=rand()%(gh-2)+1.5;
}

void walk(){
	clock_gettime(CLOCK_MONOTONIC_RAW,&passed);
	tp = (passed.tv_sec-start.tv_sec)*1000000+(passed.tv_nsec-start.tv_nsec)/1000;
	switch(m){
		case 0:
			int u,v;
			v = pdir;
			v--;
			if(v < 0)v+=4;
			for(int i=0;i<4;i++){
				bool g = false;
				switch(v){
					case 0:if(grid[(int)py][(int)px+1] == 0)g=true;break;
					case 1:if(grid[(int)py+1][(int)px] == 0)g=true;break;
					case 2:if(grid[(int)py][(int)px-1] == 0)g=true;break;
					case 3:if(grid[(int)py-1][(int)px] == 0)g=true;break;}
				u=i;
				if(g)break;
				v++;
				if(v > 3)v-=4;}
			if(u==0){m=2;pdir--;}
			else if(u==1)m=1;
			else if(u==2){m=3;pdir++;}
			else if(u==3){m=3;pdir+=2;}
			if(pdir > 3)pdir-=4;
			else if(pdir < 0)pdir+=4;
			break;
		case 1:
			if(f > 0.5/(0.000001*ws*tp) && (int)floor((px-floor(px))*10) == 5 && (int)floor((py-floor(py))*10) == 5){m=0;f=0;}
			else{
				switch(pdir){
					case 0:px+=0.000001*ws*tp;break;
					case 1:py+=0.000001*ws*tp;break;
					case 2:px-=0.000001*ws*tp;break;
					case 3:py-=0.000001*ws*tp;break;}f++;}
			break;
		case 2:
			if(abs(floor(pa*100)-157*pdir) > 0.0002*ts*tp)pa-=0.000001*ts*tp;
			else{pa = 1.57*pdir; m=1;}
			if(pa < 0)pa+=2*pi;
			break;
		case 3:
			if(abs(floor(pa*100)-157*pdir) > 0.0002*ts*tp)pa+=0.000001*ts*tp;
			else{pa = 1.57*pdir; m=1;}
			if(pa > 2*pi)pa-=2*pi;
			break;
	}
}

float dist(float x1, float y1, float x2, float y2){
        return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

void draw(){
        clock_gettime(CLOCK_MONOTONIC_RAW,&start);
	for(int x = 0; x < COLS; x++){
                float ang = pa-fov/2+fov*x/COLS;
		if(ang < 0 && x == COLS/2) ang = fov/COLS;
                if(ang > pi*2) ang -= pi*2;
                else if(ang < 0) ang += pi*2;
                float distance;
                float ix = 1/tan(ang), iy = tan(ang)*1;
                float sx, sy, tx, ty;
                if(ang > pi){
                        sy = floor(py)-py-0.000004;
                        sx = -(fabsf(sy)/tan(ang));}
                else{
                        sy = ceil(py)-py;
                        sx = sy/tan(ang);}
                ty = py+sy;
                tx = px+sx;
                if(ang > pi/2 && ang < pi*1.5)
                        sx -= 0.000004;
                while(ty > 0 && tx > 0 && ty < gh && tx < gw && grid[(int)ty][(int)tx] == 0){
                        if(ang > pi){
                                ty--;
                                tx -= ix;}
                        else{
                                ty++;
                                tx += ix;}
                }
                if(ang > pi/2 && ang < pi*1.5){
                        sx = floor(px)-px-0.000004;
                        sy = -(tan(ang)*fabsf(sx));}
                else{
                        sx = ceil(px)-px;
                        sy = tan(ang)*sx;}
                sy = py+sy;
                sx = px+sx;
                if(ang > pi)
                        sy -= 0.000004;
                while(sy > 0 && sx > 0 && sy < gh && sx < gw && grid[(int)sy][(int)sx] == 0){
                        if(ang > pi/2 && ang < pi*1.5){
                                sx--;
                                sy -= iy;}
                        else{
                                sx++;
                                sy += iy;}
                }
		bool dark = false;
                if(dist(px,py,tx,ty) < dist(px,py,sx,sy)){dark=true;distance = dist(px,py,tx,ty);}
                else distance = dist(px,py,sx,sy);
                if(x < COLS/2) distance = sin(fabsf((pa-pi/2)-ang))*distance;
                else distance = sin(fabsf((pa+pi/2)-ang))*distance;
		distance += 0.6;
                int ceil = (LINES-LINES/distance)/2;
                int floor = LINES-ceil;
		for(int y = 0; y < LINES; y++){
                        move(y,x);
                        if(y < ceil){attron(COLOR_PAIR(1));printw(" ");}
                        else if(y > floor){attron(COLOR_PAIR(4));printw(".");}
                        else if(dark == true){attron(COLOR_PAIR(2));printw("@");}
			else{attron(COLOR_PAIR(3));printw("&");}
                }
        }
        refresh();
}

int main(void){
	initscr();
	curs_set(0);
	start_color();
	init_color(COLOR_BLUE,0,525,0);//       WALL COLOR
	init_color(COLOR_CYAN,0,475,0);//        ALT WALL COLOR
	init_color(COLOR_YELLOW,200,200,200);// FLOOR COLOR
	init_pair(1,COLOR_BLACK,COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLUE);
	init_pair(3,COLOR_CYAN,COLOR_CYAN);
	init_pair(4,COLOR_YELLOW,COLOR_YELLOW);
	genmaze();
	while(1){
		draw();
		if(floor(px) != floor(fx) || floor(py) != floor(fy)) walk();
		else genmaze();
	}

	endwin();
	return 0;
}
