#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int miss,hit,memread,memwrite;
struct Drive** cache;
unsigned long int count;

typedef struct Drive {
    int pass;
    unsigned long int mark,order;


} Drive;


Drive** createc(int ssNum,int assoc);
void wSim(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void wsDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void clear(int ssNum, int assoc);
void rSim(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void rsDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void preR(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN);
void prDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN);
void preW(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN);
void pwDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN);
void preWW(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void preRR(unsigned long int mIndex,unsigned long int sIndex,int assoc);
void prrDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc);






int main(int argc, char** argv) {

    int cachesize=atoi(argv[1]),blocksize=atoi(argv[4]),n,ssNum,assoc,block,s;

    char instruct;
    unsigned long int addy,naddy,sMask,mIndex,sIndex,mdexN,sdexN;
    if(argv[3][0]=='f') {

        FILE* f1;
        f1=fopen(argv[5],"r");

        if(f1==NULL) {

            printf("Invalid file\n");
            return 0;

        }

        if(argv[2][0]=='d') {

            assoc=1;
            ssNum=cachesize/blocksize;

        } else if(argv[2][5]!=':') {
            ssNum=1;
            assoc=cachesize/blocksize;
        } else {

            sscanf(argv[2],"assoc:%d",&n);
            assoc=n;
            ssNum=cachesize/blocksize/n;

        }

        block=log(blocksize)/log(2);
        s=log(ssNum)/log(2);


        sMask=((1<<s)-1);

        cache=createc(ssNum,assoc);




        while(fscanf(f1, "%*x: %c %lx", &instruct, &addy)==2) {

            sIndex=(addy>>block)&sMask;
            mIndex=addy>>(block+s);


            if(instruct=='R') {

                rSim(mIndex,sIndex,assoc);


            } else if(instruct=='W') {
                wSim(mIndex,sIndex,assoc);
            }
        }

        fclose(f1);


        f1=fopen(argv[5],"r");

        if(f1==NULL) {

            printf("Invalid file\n");
            return 0;

        }
        printf("no-prefetch\n");
        printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n",memread,memwrite,hit,miss);

        clear(ssNum,assoc);


        while(fscanf(f1, "%*x: %c %lx", &instruct, &addy)==2) {
            mIndex=addy>>(block+s);
            sIndex=(addy>>block)&sMask;


            naddy=addy+blocksize;
            sdexN=(naddy>>block)&sMask;
            mdexN=naddy>>(block+s);


            if(instruct=='R') {

                preR(mIndex,sIndex,assoc,mdexN,sdexN);


            } else if(instruct=='W') {
                preW(mIndex,sIndex,assoc,mdexN,sdexN);
            }
        }

        printf("with-prefetch\n");
        printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n",memread,memwrite,hit,miss);


    } else if(argv[3][0]=='l') {

        FILE* f1;
        f1=fopen(argv[5],"r");

        if(f1==NULL) {

            printf("Invalid file\n");
            return 0;

        }

        if(argv[2][0]=='d') {
            assoc=1;
            ssNum=cachesize/blocksize;

        } else if(argv[2][5]!=':') {
            assoc=cachesize/blocksize;
            ssNum=1;

        } else {
            sscanf(argv[2],"assoc:%d",&n);

            ssNum=cachesize/blocksize/n;
            assoc=n;
        }


        s=log(ssNum)/log(2);
        block=log(blocksize)/log(2);
        sMask=((1<<s)-1);

        cache=createc(ssNum,assoc);

        while(fscanf(f1, "%*x: %c %lx", &instruct, &addy)==2) {

            sIndex=(addy>>block)&sMask;
            mIndex=addy>>(block+s);

            if(instruct=='R') {

                rsDrive(mIndex,sIndex,assoc);


            } else if(instruct=='W') {
                wsDrive(mIndex,sIndex,assoc);
            }
        }

        fclose(f1);


        f1=fopen(argv[5],"r");

        if(f1==NULL) {

            printf("Invalid file\n");
            return 0;

        }
        printf("no-prefetch\n");
        printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n",memread,memwrite,hit,miss);

        clear(ssNum,assoc);


        while(fscanf(f1, "%*x: %c %lx", &instruct, &addy)==2) {

            sIndex=(addy>>block)&sMask;
            mIndex=addy>>(block+s);

            naddy=addy+blocksize;
            sdexN=(naddy>>block)&sMask;
            mdexN=naddy>>(block+s);


            if(instruct=='R') {

                prDrive(mIndex,sIndex,assoc,mdexN,sdexN);


            } else if(instruct=='W') {
                pwDrive(mIndex,sIndex,assoc,mdexN,sdexN);
            }


        }

        printf("with-prefetch\n");
        printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n",memread,memwrite,hit,miss);

    } else {
        printf("Incorrect Policy");
    }
    return 0;
}
Drive** createc(int ssNum,int assoc) {
    int i=0;
    int j=0;
    cache=(Drive**)malloc(ssNum*sizeof(Drive*));
    for(i=0; i<ssNum; i++) {
        cache[i]=(Drive*)malloc((assoc)*sizeof(Drive));
    }

    for(i=0; i<ssNum; i++) {
        for(j=0; j<assoc; j++) {
            cache[i][j].pass=0;

        }
    }

    return cache;
}
void rSim(unsigned long int mIndex,unsigned long int sIndex,int assoc) {

    int i=0;
    int j=0;
    int m;

    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {

            count++;
            cache[sIndex][i].order=count;
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            miss++;
            memread++;

            return;
        } else {

            if(cache[sIndex][i].mark==mIndex) {
                hit++;
                return;
            }

            if(i==(assoc-1)) {
                m=0;
                miss++;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;

                return;
            }
        }
    }
    return;
}
void wSim(unsigned long int mIndex,unsigned long int sIndex,int assoc) {

    int i;
    int j=0;
    int m;

    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {

            count++;
            cache[sIndex][i].order=count;
            cache[sIndex][i].pass=1;
            miss++;
            memread++;
            memwrite++;
            cache[sIndex][i].mark=mIndex;


            return;
        } else {

            if(cache[sIndex][i].mark==mIndex) {

                memwrite++;
                hit++;
                return;
            }

            if(i==(assoc-1)) {
                m=0;
                miss++;
                memwrite++;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }


                cache[sIndex][m].pass=1;

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].mark=mIndex;
                return;
            }

        }
    }
    return;
}

void clear(int ssNum, int assoc) {
    int i = 0;
    int j = 0;
    for(i=0; i<ssNum; i++) {
        for(j=0; j<assoc; j++) {
            cache[i][j].order=0;
            cache[i][j].mark=0;
            cache[i][j].pass=0;


        }
    }
    count=0;
    miss=0;
    hit=0;

    memwrite=0;
    memread=0;
}

void preWW(unsigned long int mIndex,unsigned long int sIndex,int assoc) {
    int m;
    int i=0;
    int j = 0;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {

            memread++;
            cache[sIndex][i].mark=mIndex;
            count++;
            cache[sIndex][i].order=count;
            cache[sIndex][i].pass=1;



            return;
        } else {

            if(cache[sIndex][i].mark==mIndex) {

                return;
            }

            if(i==(assoc-1)) {
                m=0;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;

                return;
            }

        }
    }

    return;

}
void preRR(unsigned long int mIndex,unsigned long int sIndex,int assoc) {
    int m;
    int i=0;
    int j;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {

            memread++;

            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            count++;
            cache[sIndex][i].order=count;

            return;
        } else {

            if(cache[sIndex][i].mark==mIndex) {

                return;
            }

            if(i==(assoc-1)) {

                memread++;

                m=0;
                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }


                cache[sIndex][m].mark=mIndex;
                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                return;
            }
        }
    }
}

void preW(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN) {
    int m;
    int i=0,j=0;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            miss++;
            memread++;
            cache[sIndex][i].pass=1;
            mIndex=cache[sIndex][i].mark;
            memwrite++;
            count++;
            count=cache[sIndex][i].order;


            preWW(mdexN,sdexN,assoc);
            return;
        } else {

            if(mIndex==cache[sIndex][i].mark) {
                memwrite++;
                hit++;

                return;
            }

            if(i==(assoc-1)) {
                m=0;
                miss++;
                memwrite++;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }


                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;
                preWW(mdexN, sdexN,assoc);
                return;
            }

        }
    }

    return;

}
void preR(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN) {

    int m;
    int i=0;
    int j = 0;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            miss++;
            memread++;
            cache[sIndex][i].order=count;
            cache[sIndex][i].pass=1;
            count++;

        mIndex=cache[sIndex][i].mark;


            preRR(mdexN,sdexN,assoc);
            return;
        } else {

            if(mIndex==cache[sIndex][i].mark) {
                hit++;
                return;
            }

            if(i==(assoc-1)) {
                m=0;
                memread++;
                miss++;

                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][m].order>=cache[sIndex][j].order) {
                        m=j;
                    }
                }



                mIndex=cache[sIndex][m].mark;
                count++;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].order=count;
                preRR(mdexN,sdexN,assoc);
                return;
            }

        }
    }

    return;
}


void rsDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc) {
    int j, m;
    int i=0;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            count++;
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            cache[sIndex][i].order=count;
            miss++;
            memread++;





            return;
        } else {

            if(mIndex==cache[sIndex][i].mark) {

                count++;
                cache[sIndex][i].order=count;
                hit++;
                return;
            }

            if(i==(assoc-1)) {
                m=0;

                memread++;
                miss++;
                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][m].order>=cache[sIndex][j].order) {
                        m=j;
                    }
                }
                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;
                return;
            }
        }
    }
    return;
}
void wsDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc) {
    int m;
    int i;
    int j;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            memread++;
            miss++;
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            memwrite++;
            count++;

            cache[sIndex][i].order=count;

            return;
        } else {

            if(mIndex==cache[sIndex][i].mark) {

                count++;
                cache[sIndex][i].order=count;
                hit++;
                memwrite++;
                return;
            }

            if(i==(assoc-1)) {
                memwrite++;
                m=0;
                miss++;
                memread++;

                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }


                cache[sIndex][m].pass=1;

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].mark=mIndex;
                return;
            }

        }
    }

    return;

}

void pwwDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc) {

    int i=0,j=0,m;

    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            count++;

            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            memread++;
            cache[sIndex][i].order=count;

            return;
        } else {

            if(mIndex==cache[sIndex][i].mark) {

                return;
            }

            if(i==(assoc-1)) {
                m=0;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][m].order>=cache[sIndex][j].order) {
                        m=j;
                    }
                }

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;

                return;
            }




        }


    }

    return;



}
void prrDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc) {
    int m;
    int i=0;
    int j=0;


    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            memread++;
            count++;

            cache[sIndex][i].order=count;



            return;
        } else {





            if(cache[sIndex][i].mark==mIndex) {

                return;
            }

            if(i==(assoc-1)) {
                m=0;
                memread++;


                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }

                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;

                return;
            }




        }


    }
}



void pwDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN) {

    int i,j=0;
    int m;

    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            miss++;
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            memread++;
            memwrite++;
            count++;

            cache[sIndex][i].order=count;
            preWW(mdexN,sdexN,assoc);
            return;
        } else {


            if(cache[sIndex][i].mark==mIndex) {

                count++;
                cache[sIndex][i].order=count;
                hit++;
                memwrite++;
                return;
            }

            if(i==(assoc-1)) {
                m=0;
                miss++;

                memwrite++;
                memread++;
                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][j].order<=cache[sIndex][m].order) {
                        m=j;
                    }
                }


                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;

                preWW(mdexN, sdexN,assoc);
                return;
            }




        }


    }
    return;



}
void prDrive(unsigned long int mIndex,unsigned long int sIndex,int assoc,unsigned long int mdexN,unsigned long int sdexN) {


    int i=0,j=0,m;

    for(i=0; i<assoc; i++) {
        if(cache[sIndex][i].pass==0) {
            cache[sIndex][i].pass=1;
            cache[sIndex][i].mark=mIndex;
            count++;

            cache[sIndex][i].order=count;
            miss++;
            memread++;
            preRR(mdexN,sdexN,assoc);
            return;
        } else {





            if(mIndex==cache[sIndex][i].mark) {

                count++;
                cache[sIndex][i].order=count;
                hit++;
                return;
            }

            if(i==(assoc-1)) {

                memread++;
                miss++;
                m=0;
                for(j=0; j<assoc; j++) {

                    if(cache[sIndex][m].order>=cache[sIndex][j].order) {
                        m=j;
                    }
                }



                count++;
                cache[sIndex][m].order=count;
                cache[sIndex][m].pass=1;
                cache[sIndex][m].mark=mIndex;
                preRR(mdexN,sdexN,assoc);
                return;
            }




        }


    }

    return;


}








