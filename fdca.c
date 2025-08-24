
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 200
#define HISTORY_SIZE_MAX 50

#define GREEN   "\x1b[32m"
#define GRAY    "\x1b[90m"
#define YELLOW  "\x1b[33m"
#define RESET   "\x1b[0m"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define CLEAR "cls"
void sleep_ms(int ms){ Sleep(ms); }
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#define CLEAR "clear"
void sleep_ms(int ms){ usleep(ms*1000); }

int kbhit(void){
    struct timeval tv={0,0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0,&fds);
    return select(1,&fds,NULL,NULL,&tv);
}
int getch(void){
    struct termios oldt,newt;
    int ch;
    tcgetattr(0,&oldt);
    newt=oldt;
    newt.c_lflag&=~(ICANON|ECHO);
    tcsetattr(0,TCSANOW,&newt);
    ch=getchar();
    tcsetattr(0,TCSANOW,&oldt);
    return ch;
}
#endif

int apply_rule(int rule,int left,int self,int right){
    int n=(left<<2)|(self<<1)|right;
    return (rule>>n)&1;
}

void print_history(int h[HISTORY_SIZE_MAX][MAX_SIZE],int step,int size,int steps_in_history,int window,int delay,int rule,int total_size){
    printf("FDCA (Rule %d, Size %d) ",rule,total_size);
    printf(YELLOW "[Delay: %d ms, Window: %d]%s\n\n",delay,window,RESET);
    int start_idx = steps_in_history>window ? steps_in_history-window :0;
    int rows_to_print = steps_in_history-start_idx;
    for(int s=0;s<rows_to_print;s++){
        int step_num = step-rows_to_print+s+1;
        printf("%3d ",step_num);
        for(int i=0;i<size;i++) printf(h[start_idx+s][i]?GREEN"#":GRAY".");
        printf(RESET "\n");
    }
}

void save_visible_window_manual(int h[HISTORY_SIZE_MAX][MAX_SIZE], int steps_in_history, int window, int size, int current_step){
    int start_idx = steps_in_history>window ? steps_in_history-window:0;
    int rows_to_save = steps_in_history-start_idx;

    char name[64];
    printf("Enter filename (without extension): ");
    scanf("%s",name);
    char filename[72];
    snprintf(filename,sizeof(filename),"%s.txt",name);

    FILE *fp=fopen(filename,"w");
    if(!fp){ printf("Error creating file.\n"); return; }

    for(int s=0;s<rows_to_save;s++){
        int step_num=current_step-rows_to_save+s+1;
        fprintf(fp,"%3d ",step_num);
        for(int i=0;i<size;i++)
            fputc(h[start_idx+s][i]?'#':'.',fp);
        fputc('\n',fp);
    }
    fclose(fp);
    printf("Visible window saved to %s\n",filename);
}

void save_visible_window_csv(int h[HISTORY_SIZE_MAX][MAX_SIZE], int steps_in_history, int window, int size, int current_step){
    int start_idx = steps_in_history>window ? steps_in_history-window:0;
    int rows_to_save = steps_in_history-start_idx;

    char name[64];
    printf("Enter CSV filename (without extension): ");
    scanf("%s",name);
    char filename[72];
    snprintf(filename,sizeof(filename),"%s.csv",name);

    FILE *fp=fopen(filename,"w");
    if(!fp){ printf("Error creating CSV file.\n"); return; }

    for(int s=0;s<rows_to_save;s++){
        int step_num=current_step-rows_to_save+s+1;
        fprintf(fp,"%d",step_num);
        for(int i=0;i<size;i++)
            fprintf(fp,",%d",h[start_idx+s][i]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("Visible window saved as CSV: %s\n",filename);
}

void save_full_history_csv(int h[HISTORY_SIZE_MAX][MAX_SIZE], int steps_in_history, int size){
    char name[64];
    printf("Enter CSV filename for full history (without extension): ");
    scanf("%s",name);
    char filename[72];
    snprintf(filename,sizeof(filename),"%s.csv",name);

    FILE *fp=fopen(filename,"w");
    if(!fp){ printf("Error creating CSV file.\n"); return; }

    for(int s=0;s<steps_in_history;s++){
        fprintf(fp,"%d",s);
        for(int i=0;i<size;i++)
            fprintf(fp,",%d",h[s][i]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("Full history saved as CSV: %s\n",filename);
}

void save_transposed_csv(int h[HISTORY_SIZE_MAX][MAX_SIZE], int steps_in_history, int size){
    char name[64];
    printf("Enter CSV filename for transposed format (without extension): ");
    scanf("%s",name);
    char filename[72];
    snprintf(filename,sizeof(filename),"%s.csv",name);

    FILE *fp=fopen(filename,"w");
    if(!fp){ printf("Error creating CSV file.\n"); return; }

    fprintf(fp,"Cell\\Step");
    for(int s=0;s<steps_in_history;s++)
        fprintf(fp,",%d",s);
    fprintf(fp,"\n");

    for(int i=0;i<size;i++){
        fprintf(fp,"%d",i);
        for(int s=0;s<steps_in_history;s++)
            fprintf(fp,",%d",h[s][i]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("Transposed CSV saved: %s\n",filename);
}

int main(){
    int rule,size,steps,boundary,run_mode;
    int delay_ms=200;
    int cells[MAX_SIZE]={0}, next[MAX_SIZE]={0};
    int history[HISTORY_SIZE_MAX][MAX_SIZE]={{0}};
    int steps_in_history=0, history_window=20;

    printf("Enter Wolfram rule (0-255): "); scanf("%d",&rule);
    printf("Grid size (<= %d): ",MAX_SIZE); scanf("%d",&size);
    printf("Number of steps: "); scanf("%d",&steps);
    printf("Boundary type (0=fixed,1=periodic): "); scanf("%d",&boundary);
    printf("Initial condition (0=single 1 in middle,1=random): "); int init_choice; scanf("%d",&init_choice);
    printf("Run mode (0=normal,1=pause,2=auto-delay with controls): "); scanf("%d",&run_mode);

    if(init_choice==0) cells[size/2]=1;
    else{ srand(time(NULL)); for(int i=0;i<size;i++) cells[i]=rand()%2; }

    if(run_mode==2){
        printf("Initial delay per step (ms, e.g.,200): "); scanf("%d",&delay_ms);
        printf("Controls: '+', '-', 'p', '[', ']', 's', 'c', 'f', 't', 'q'\n");
    }

    int paused=0;
    for(int step=0;step<steps;step++){
        if(steps_in_history<HISTORY_SIZE_MAX){
            for(int i=0;i<size;i++) history[steps_in_history][i]=cells[i];
            steps_in_history++;
        }else{
            for(int s=1;s<HISTORY_SIZE_MAX;s++)
                for(int i=0;i<size;i++) history[s-1][i]=history[s][i];
            for(int i=0;i<size;i++) history[HISTORY_SIZE_MAX-1][i]=cells[i];
        }

        if(run_mode==2) system(CLEAR);
        if(run_mode==2) print_history(history,step,size,steps_in_history,history_window,delay_ms,rule,size);
        else { printf("%3d ",step); for(int i=0;i<size;i++) printf(cells[i]?GREEN"#":GRAY"."); printf(RESET "\n"); }

        for(int i=0;i<size;i++){
            int left=(boundary==1)?cells[(i-1+size)%size]:(i==0?0:cells[i-1]);
            int right=(boundary==1)?cells[(i+1)%size]:(i==size-1?0:cells[i+1]);
            next[i]=apply_rule(rule,left,cells[i],right);
        }
        for(int i=0;i<size;i++) cells[i]=next[i];

        int key=-1;
        if(run_mode==1){ getchar(); getchar(); }
        else if(run_mode==2){
        #ifdef _WIN32
            if(_kbhit()) key=_getch();
        #else
            if(kbhit()) key=getch();
        #endif
            if(key!=-1){
                if(key=='+') delay_ms=delay_ms>20?delay_ms-20:20;
                if(key=='-') delay_ms+=20;
                if(key=='p'||key=='P') paused=!paused;
                if(key=='[') history_window=history_window>1?history_window-1:1;
                if(key==']') history_window=history_window<HISTORY_SIZE_MAX?history_window+1:HISTORY_SIZE_MAX;
                if(key=='s'||key=='S') save_visible_window_manual(history,steps_in_history,history_window,size,step);
                if(key=='c'||key=='C') save_visible_window_csv(history,steps_in_history,history_window,size,step);
                if(key=='f'||key=='F') save_full_history_csv(history,steps_in_history,size);
                if(key=='t'||key=='T') save_transposed_csv(history,steps_in_history,size);
                if(key=='q'||key=='Q') break;
            }
            while(paused){
            #ifdef _WIN32
                if(_kbhit()){ int k=_getch(); if(k=='p'||k=='P') paused=0; if(k=='q'||k=='Q') exit(0);}
            #else
                if(kbhit()){ int k=getch(); if(k=='p'||k=='P') paused=0; if(k=='q'||k=='Q') exit(0);}
            #endif
                sleep_ms(50);
            }
            sleep_ms(delay_ms);
        }
    }
    return 0;
}
