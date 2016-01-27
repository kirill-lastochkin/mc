#include <stdio.h>
#include "fmng.h"
#include <ncurses.h>
#include <stdlib.h>

extern WINDOW *stdscr;
WINDOW *menu[2],*mng[2];

void InitScreen(void)
{
    initscr();
    cbreak();
    noecho();
    clear();
    curs_set(0);
    //�����
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_BLUE);
    init_pair(2,COLOR_BLUE,COLOR_YELLOW);
    menu[0]=newwin(5,getmaxx(stdscr)/4-1,getmaxy(stdscr)-5,1);
    menu[1]=newwin(5,getmaxx(stdscr)*3/4,getmaxy(stdscr)-5,getmaxx(stdscr)/4);
    mng[0]=newwin(getmaxy(stdscr)-6,getmaxx(stdscr)/2-1,1,1);
    mng[1]=newwin(getmaxy(stdscr)-6,getmaxx(stdscr)/2-1,1,getmaxx(stdscr)/2+1);
    //colors
    wbkgd(menu[0],COLOR_PAIR(0));
    wbkgd(menu[1],COLOR_PAIR(0));
    wbkgd(mng[0],COLOR_PAIR(1));
    wbkgd(mng[1],COLOR_PAIR(1));
    box(menu[0],'|','-');
    box(menu[1],'|','-');
    //1 menu
    wattron(menu[0],COLOR_PAIR(0)| A_BOLD);
    wmove(menu[0],0,getmaxx(menu[0])/2-2);
    wprintw(menu[0],"Menu");
    wmove(menu[0],1,1);
    wprintw(menu[0],"F1 SEE PROPERTIES");
    wmove(menu[0],2,1);
    wprintw(menu[0],"F2 QUIT");
    //2 menu
    wattron(menu[1],COLOR_PAIR(0)| A_BOLD);
    wmove(menu[1],0,getmaxx(menu[1])/2-8);
    wprintw(menu[1],"File Information");
    //������
    wattron(mng[0],COLOR_PAIR(1)| A_BOLD);
    wattron(mng[1],COLOR_PAIR(1)| A_BOLD);
    box(mng[0],'|','-');
    box(mng[1],'|','-');
    wmove(mng[0],0,getmaxx(mng[0])/2-9);
    wprintw(mng[0],"Directory content");
    wmove(mng[1],0,getmaxx(mng[0])/2-9);
    wprintw(mng[1],"Directory content");
    //��������
    wrefresh(menu[0]);
    wrefresh(menu[1]);
    wrefresh(mng[0]);
    wrefresh(mng[1]);
    //��������� ��������� ����������� ������
    keypad(menu[0],TRUE);
    keypad(menu[1],TRUE);
}

void DeleteScreen(void)
{
    delwin(menu[0]);
    delwin(menu[1]);
    delwin(mng[0]);
    delwin(mng[1]);
    endwin();
    clear();
}

//���������� ��� ��������/����� �� �������� �������
int PrintDir(int mngnum,int pos,char* name)
{
    wmove(mng[mngnum],pos,1);
    wprintw(mng[mngnum],name);
    wrefresh(mng[mngnum]);
    return 1;
}

//������� ������
//0 - ����� ������
//1 - ������ ������
//2 - ���������� � �����
void ClearPanel(int mngnum)
{
    if(mngnum==0||mngnum==1)
    {
        wclear(mng[mngnum]);
        wbkgd(mng[mngnum],COLOR_PAIR(1));
        wattron(mng[mngnum],COLOR_PAIR(1)| A_BOLD);
        box(mng[mngnum],'|','-');
        wmove(mng[mngnum],0,getmaxx(mng[0])/2-9);
        wprintw(mng[mngnum],"Directory content");
        wmove(mng[mngnum],1,1);
        wrefresh(mng[mngnum]);
    }
    if(mngnum==2)
    {
        wclear(menu[1]);
        box(menu[1],'|','-');
        wmove(menu[1],0,getmaxx(menu[1])/2-8);
        wprintw(menu[1],"File Information");
        wrefresh(menu[1]);
    }
}

//��������� ����������
//�� ���� �������: �����  ������, ����� ������� ������������ �����
//����� 1 - ���������, 0 - ������������
void SelectDir(int mngnum,int pos,int mode)
{
    char *name;
    int len,i;
    len=getmaxx(mng[mngnum]);
    name=(char*)calloc(len,sizeof(char));
    //�������� ��� �������
    for(i=1;i<(len-1);i++)
    {
        name[i-1]=mvwinch(mng[mngnum],pos,i);
    }
    //�������������� � ����� ������
    if(mode)
    {
        wattron(mng[mngnum],COLOR_PAIR(2)| A_BOLD);
    }
    else
    {
        wattron(mng[mngnum],COLOR_PAIR(1)| A_BOLD);
    }
    wmove(mng[mngnum],pos,1);
    wprintw(mng[mngnum],name);
    wattron(mng[mngnum],COLOR_PAIR(1)| A_BOLD);
    wrefresh(mng[mngnum]);
    free(name);
}

//������� ����-����� ����������� ����� ��������, ������� ����� ������
int LeftRightHandler(int x0,int mngnum,int pos)
{
    if(x0>0&&mngnum==0)
    {
        SelectDir(0,pos,0);
        SelectDir(1,1,1);
        return 1;
    }
    if(x0<0&&mngnum==1)
    {
        SelectDir(1,pos,0);
        SelectDir(0,1,1);
        return 0;
    }
    return mngnum;
}

//�������� �����-���� ����� ������������, ������� ����� ����� �������
int UpDownHandler(int y0,int mngnum,int pos)
{
    char ch;
    //������������ ������� �������
    if(!(pos==1&&y0<0))
    {
        ch=mvwinch(mng[mngnum],pos+1,1);
        //������������ ������ �������
        if(!(y0>0&&ch==' '))
        {
            SelectDir(mngnum,pos,0);
            SelectDir(mngnum,pos+y0,1);
            return pos+y0;
        }
        else return pos;
    }
    return pos;
}

//�� ����:
//����� �� ������, ����� ����� (1-3) � ������� (1-2)
int PrintInfo_g(char* info,long int data,int line,int col)
{
    if(!(line>0&&line<4&&col>0&&col<3)) return 0;
    else
    {
        if(col==1)
        {
            if(line==1)
            {
                wmove(menu[1],line,(col-1)*getmaxx(menu[1])/2+1);
                wprintw(menu[1],"File name: ");
                wprintw(menu[1],info);
            }
            if(line==2)
            {
                wmove(menu[1],line,(col-1)*getmaxx(menu[1])/2+1);
                wprintw(menu[1],"Size of file: ");
                wprintw(menu[1],"%d",data);
            }
            if(line==3)
            {
                wmove(menu[1],line,(col-1)*getmaxx(menu[1])/2+1);
                wprintw(menu[1],"inode: ");
                wprintw(menu[1],"%d",data);
            }
        }
        if(col==2)
        {
            if(line==1)
            {
                wmove(menu[1],line,(col-1)*getmaxx(menu[1])/2+1);
                wprintw(menu[1],"mode: ");
                wprintw(menu[1],"%d",data);
            }
        }
        wrefresh(menu[1]);
        return 1;
    }

}