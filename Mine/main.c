#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#define X0 0
#define Y0 1
int DEVELOP = 0;
//□■
void welcome();
void generate(int **map,int m,int n,int booms);
void draw_dy(int **map, int **flag, int m, int n, int x0, int y0);
void draw(int **map,int **flag,int m,int n);
void choose_dy(int **map, int **flag, int m, int n, int x0, int y0);
void choose(int **map,int **flag,int m,int n);
void dfs(int **map,int **flag,int m,int n,int x,int y/*,int depth*/);
int judge(int **map,int **flag,int m,int n,int booms);
typedef struct
{
	int x;
	int y;
}point;

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
void setcursortype(CURSOR_TYPE c)
{
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void gotoxy(int x, int y)
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

int getxy(char xy)
{
	HANDLE gh_std_out;
	gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);	
	if (xy == 'y' || xy == 'Y')
		return	bInfo.dwCursorPosition.Y;
	else
		return bInfo.dwCursorPosition.X;
}

int main()
{
	//printf("□■");
	//Choose level
	int m,n,booms,level;
	welcome();
	//scanf_s("%d",&level);
	level = getch() - '0';
	while(level < 1 || level > 3)
	{
		if (level == 'n' - '0' || level == 'N' - '0')
			DEVELOP = 1;
		else
			printf("非法输入！请重新输入选择的标号（1-3）：");
		level = getch() - '0';
	}
	system("cls");
	setcursortype(SOLIDCURSOR);
	switch(level)
	{
		case 1:
			m=8,n=10,booms=10;
			system("mode con: cols=45 lines=12");
			break;
		case 2:
			m=15,n=20,booms=40;
			system("mode con: cols=85 lines=19");
			break;
		case 3:
			m=25,n=30,booms=99;
			system("mode con: cols=125 lines=29");
			break;
	}
	//Generate map
	//In the map, -1 stands for a boom, other numbers greater than 0 stands for
	//how many booms are around this point.
	int i;
	int **map = (int**)malloc(m*sizeof(int*));
	for(i=0;i<m;++i)
	{
		map[i] = (int*)malloc(n*sizeof(int));
		memset(map[i],0,n*sizeof(int));
	}
	generate(map,m,n,booms);
	//Initialize flag
	//In flag, 1 stands for points that have been revealed,
	//0 stands for nothing.
	int **flag = (int**)malloc(m*sizeof(int*));
	for(i=0;i<m;++i)
	{
		flag[i] = (int*)malloc(n*sizeof(int));
		memset(flag[i],0,n*sizeof(int));
	}
	if (DEVELOP)
	{
		int **dev = (int**)malloc(m*sizeof(int*));
		for (i = 0; i < m; ++i)
		{
			int j;
			dev[i] = (int*)malloc(n*sizeof(int));
			for (j = 0; j < n; ++j)
			{
				dev[i][j] = 1;
			}
		}
		draw_dy(map, dev, m, n, 2 * n + 1, 1);
	}

	//Start the game
	int result = -1;
	draw_dy(map, flag, m, n, X0, Y0);
	do
	{
		choose_dy(map,flag,m,n,X0,Y0);
		result = judge(map,flag,m,n,booms);
	}while(result == -1);
	//End the game
	for (int **ip = flag; ip < flag + m; ip++)
	{
		for (int *subip = *ip; subip < *ip + n; subip++)
			if(*subip != -1)
				*subip = 1;
	}
	draw_dy(map, flag, m, n, X0, Y0);
	puts("");
	if(result == 1)
		printf("恭喜，您获胜了！:)\n");
	else
		printf("非常遗憾，您失败了！请再接再厉。:(\n");
	Sleep(5000);
	return 0;
}

void welcome()
{
	setcursortype(NOCURSOR);
	printf("\t**********************************\n");
	printf("\t欢迎使用2645扫雷！请选择游戏难度：\n");
	printf("\t1.初级 8x10网格 共10颗雷。\n");
	printf("\t2.中级 15x20网格 共40颗雷。\n");
	printf("\t3.高级 25x30网格 共99颗雷。\n");
	printf("\t请输入选择的标号：\n");
	printf("\t**********************************\n");
	printf("\t用space和enter来清扫道路吧！\n");
	printf("\t按n来作弊:)");
}
//Generate map
//In the map, -1 stands for a boom, other numbers greater than 0 stands for
//how many booms are around this point.
void generate(int **map,int m,int n,int booms)
{
	srand(time(0));
	int i,j;
	for(i=0;i<booms;++i)
	{
		int x=0,y=0;
		do
		{
			x = rand() % m;
			y = rand() % n;
		}while(map[x][y] == -1);
		map[x][y] = -1;
	}
	for(i=0;i<m;++i)
	{
		for(j=0;j<n;++j)
		{
			if(map[i][j]==-1)
			{
				int a,b;
				for(a=i-1;a<=i+1;++a)
				{
					for(b=j-1;b<=j+1;++b)
					{
						if(a>=0&&a<m&&b>=0&&b<n&&map[a][b]!=-1)
							++map[a][b];
					}
				}
			}
		}
	}
}

void draw_dy(int **map, int **flag, int m, int n, int x0, int y0)
{
	int i, j;
	for (i = 0; i < m; ++i)
		for (j = 0; j < n; ++j)
		{
			gotoxy(x0 + 2 * j, y0 + i);
			if (flag[i][j] == 1)
			{
				switch (map[i][j])
				{
				case -1:
					printf("※");
					break;
				case 0:
					printf("■");
					break;
				default:
					printf("%2d", map[i][j]);
					break;
				}
			}
			else if(flag[i][j] == -1)
				printf("▲");
			else
				printf("□");
		}
}

void draw(int **map,int **flag,int m,int n)//□■※
{
	int i,j;
	printf("\t  ");
	for(j=0;j<n;++j)
		printf("%2d",j+1);
	printf("\n");
	for(i=0;i<m;++i)
	{
		printf("\t%2d",i+1);
		for(j=0;j<n;++j)
		{
			if(flag[i][j]==1)
			{
				switch(map[i][j])
				{
					case -1:
						printf("※");
						break;
					case 0:
						printf("■");
						break;
					default:
						printf("%2d",map[i][j]);
						break;
				}
			}
			else
				printf("□");
		}
		printf("\n");
	}
}

void choose_dy(int **map, int **flag, int m, int n, int x0, int y0)
{
	Sleep(100);
	int i;
	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		int nx = getxy('x');
		int ny = getxy('y');
		int cx = (getxy('y') - y0);
		int cy = (getxy('x') - x0) / 2;
		
		if (!(cx < 0 || cx >= m || cy < 0 || cy >= n || flag[cx][cy] == 1))
		{
			flag[cx][cy] = 1;
			if (map[cx][cy] == 0)
				dfs(map, flag, m, n, cx, cy/*,0*/);
		}
		draw_dy(map, flag, m, n, X0, Y0);
		gotoxy(nx, ny);
	}
	if (GetAsyncKeyState(VK_RETURN) < 0)
	{
		int nx = getxy('x');
		int ny = getxy('y');
		int cx = (getxy('y') - y0);
		int cy = (getxy('x') - x0) / 2;
		if (!(cx < 0 || cx >= m || cy < 0 || cy >= n || flag[cx][cy] == 1))
		{
			if (flag[cx][cy] == 0)
			{
				flag[cx][cy] = -1;
			}
			else if (flag[cx][cy] == -1)
			{
				flag[cx][cy] = 0;
			}
		}
		draw_dy(map, flag, m, n, X0, Y0);
		gotoxy(nx, ny);
	}
	if (GetAsyncKeyState(VK_UP) < 0 || GetAsyncKeyState('W') < 0)
	{
		gotoxy(getxy('x'), getxy('y') - 1);
	}
	if (GetAsyncKeyState(VK_DOWN) < 0 || GetAsyncKeyState('S') < 0)
	{
		gotoxy(getxy('x'), getxy('y') + 1);
	}
	if (GetAsyncKeyState(VK_LEFT) < 0 || GetAsyncKeyState('A') < 0)
	{
		gotoxy(getxy('x') - 1, getxy('y'));
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0 || GetAsyncKeyState('D') < 0)
	{
		gotoxy(getxy('x') + 1, getxy('y'));
	}
}
void choose(int **map,int **flag,int m,int n)
{
	int cx=0,cy=0;
	do
	{
		printf("请输入您选择的行号和列号，中间用空格分隔：");
		scanf_s("%d %d",&cx,&cy);
		cx--,cy--;
	}while(cx<0||cx>=m||cy<0||cy>=n||flag[cx][cy]==1);
	flag[cx][cy]=1;
	if(map[cx][cy]==0)
		dfs(map,flag,m,n,cx,cy/*,0*/);
}

void dfs(int **map,int **flag,int m,int n,int x,int y/*,int depth*/)
{
	int i,j;
	for(i=x-1;i<=x+1;++i)
	{
		for(j=y-1;j<=y+1;++j)
		{
			//this is a severe logical mistake
			//if(i!=x&&j!=y&&i>=0&&i<m&&j>=0&&j<n)
			if(!(i==x&&j==y)&&i>=0&&i<m&&j>=0&&j<n)
			{
				//even if some blocks are open or repetition
				//you continually seek for other blocks in the 8 around you
				if(flag[i][j]==1)
					continue;
				//these are nosense codes
				//else if(map[i][j]==-1)
				//	return;
				//else if(depth>=1)
				//{
				//	flag[i][j]=1;
				//	return;
				//}
				else if(map[i][j]==0)
				{
					//set flag state before recursion, otherwise stack will overflow
					flag[i][j] = 1;
					dfs(map,flag,m,n,i,j/*,depth*/);					
				}
				else
				{
					//what is this fucking depth use for?
					//you stop while meeting a number
					//dfs(map,flag,m,n,i,j,depth+1);
					flag[i][j]=1;
				}
			}
		}
	}
}
//judge returns 1 for win, returns 0 for lose, returns -1 for continue.
int judge(int **map,int **flag,int m,int n,int booms)
{
	int cnt=0,i,j;
	for(i=0;i<m;++i)
	{
		for(j=0;j<n;++j)
		{
			if(flag[i][j]==1)
			{
				if(map[i][j]==-1)
					return 0;
				else
					cnt++;
			}
		}
	}
	if(cnt==m*n-booms)
		return 1;
	else
		return -1;
}
