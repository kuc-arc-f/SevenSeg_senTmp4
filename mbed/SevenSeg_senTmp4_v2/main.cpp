#include "mbed.h"
#include <stdio.h>
#include <string>

//pin
AnalogIn inSen1(dp13);

DigitalOut mPin_1( dp1);
DigitalOut mPin_2( dp2);
DigitalOut mPin_4( dp4);
DigitalOut mPin_5( dp5);
DigitalOut mPin_6( dp6);
DigitalOut mPin_9( dp9);
DigitalOut mPin_10(dp10);

DigitalOut mPin_11(dp11);
//DigitalOut mPin_13(dp13);
DigitalOut mPin_14(dp14);
DigitalOut mPin_17(dp17);
DigitalOut mPin_18(dp18);

//pin
DigitalOut mAnode[4] ={mPin_6, mPin_10, mPin_11, mPin_18 };
DigitalOut mCathod[7]=      {mPin_1, mPin_2, mPin_4, mPin_5, mPin_9, mPin_14, mPin_17 };
int mCount=0;
int   mCathod_pin[7]  ={6, 4, 2, 1, 0, 5, 3 };
int   mAnode_pin[4]   ={0, 1, 2, 3  };

Serial mPc(USBTX, USBRX);
Timer mTimer;

bool Num_Array[10][8]={
// a b c d e f g dp 
{ 1,1,1,1,1,1,0,0}, //0 :0
{ 0,1,1,0,0,0,0,0}, //1 :1
{ 1,1,0,1,1,0,1,0}, //2 :2
{ 1,1,1,1,0,0,1,0}, //3 :3
{ 0,1,1,0,0,1,1,0}, //4 :4
{ 1,0,1,1,0,1,1,0}, //5 :5
{ 1,0,1,1,1,1,1,0}, //6 :6
{ 1,1,1,0,0,0,0,0}, //7 :7
{ 1,1,1,1,1,1,1,0}, //8 :8
{ 1,1,1,1,0,1,1,0}, //9 :9   
};

int mMaxAnode=4;

int mSTAT =0;
int mSTAT_CONECT=1;
int mSTAT_DISP=2;

//const int mTmMax=5;
const int mTmMax=3;

string mResponse="";
string mReceive="";

//
void init_proc(){
  // anode
  for(int i=0;i<4; i++){
      mAnode[ i ]=0;
  }
  // cathod
  for(int j=0;j<7; j++){
      mCathod[j]=0;
  }
}

// 
void proc_uart(){
   if( mPc.readable()) {
        char c= mPc.getc();
        mResponse+= c;
   }else{
       if(mSTAT ==mSTAT_CONECT){
//printf("mResponse=%s\n" , mResponse.c_str() );                   
             if(mResponse.length() >= 10){               
               string sHead=mResponse.substr(0,4);
//               if( sHead== "tmp="){
               if( sHead== "dat="){
                   mReceive =mResponse.substr(4 );
//printf("sTmp=%s\n" , mReceive.c_str() );                   
                   mSTAT =mSTAT_DISP;
               }
               mResponse="";
           }
       }
   }
}
//
void set_anode(int pinAnode){
  for(int i=0;i <mMaxAnode; i++){
    if(mAnode_pin[i]==pinAnode){
       mAnode[i]=1;
    }else{
       mAnode[i]=0;
    }
  }
}
//
void NumPrint(int Number){
  for (int i=0; i< 7; i++){
    if(Num_Array[Number][i]==1){
       mCathod[mCathod_pin[i]]=0;
//printf("0");
    }
    wait_us(500);
    if(Num_Array[Number][i]==1){
       mCathod[mCathod_pin[i]]=1;
//printf("1");
    }
//printf(" \n");    
    mCathod[mCathod_pin[i]]=1;
  }
}


//
void proc_display(int iTemp){
  int iDeg =iTemp;
  int iDeg_10= iDeg / 10;
  int iDeg_1 = iDeg % 10;
   // dig-1
   for(int i=0;  i< 200; i++ ){
       set_anode(mAnode_pin[0]);
       NumPrint(iDeg_1);
       // dig-10
       set_anode(mAnode_pin[1]);
       NumPrint(iDeg_10);
   }
}
//
void proc_dispTime(string  sTime){
    //char sBuff[4+1];
  if(sTime.length() < 4){ return ; }
  //sBuff   =sTime.c_str();
  //sprintf(sBuff ,"%s" , sTime.c_str());
  string sOne=sTime.substr(0,1);
  int iDeg_1000= atoi( sOne.c_str() );
  sOne=sTime.substr(1,1);
  int iDeg_100 = atoi( sOne.c_str());
  sOne=sTime.substr(2,1);
  int iDeg_10  = atoi( sOne.c_str());
  sOne=sTime.substr(3,1);
  int iDeg_1   = atoi( sOne.c_str());
   // dig-1
   for(int i=0;  i< 100; i++ ){
       set_anode(mAnode_pin[0]);
       NumPrint(iDeg_1);
       // dig-10
       set_anode(mAnode_pin[1]);
       NumPrint(iDeg_10);
       // dig-100
       set_anode(mAnode_pin[2]);
       NumPrint(iDeg_100);
       // dig-1000
       set_anode(mAnode_pin[3]);
       NumPrint(iDeg_1000);
   }
}

//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//
// reading LM60BIZ
int getTempNum(){
  int iRet=0;
  float fSen  = 0;
  unsigned long reading  = 0;   
  for (int i=0; i<10; i++) {
    fSen  = inSen1;
    int  iTmp= int(fSen  * 1000); 
    reading  += iTmp; 
    wait_ms(100);
  }
  int SValue= reading / 10;
  int voltage=convert_Map(SValue, 0, 1000, 0,3300);  // V
  int iTemp = (voltage - 424) / 6.25; //電圧値を温度に変換, offset=425
  iRet= iTemp;
  
  return iRet;
}

//
int main() {
   mSTAT = mSTAT_CONECT;
   mTimer.start();
   mPc.baud(9600 );
   mPc.printf("#Start-main \n");    
   init_proc();
   int iMode=0;
   int itemp=0;
   while(1){
       float sec= mTimer.read();
       if(mSTAT == mSTAT_CONECT){
           if(sec >= mTmMax){
               mTimer.reset();
               itemp= getTempNum();
               printf("tmp=%2d\r\n" , itemp );
           }else{
               proc_uart();
               wait_ms(5);
           }
       }else if(mSTAT == mSTAT_DISP){
            string sBuff="";          
            if(iMode==0){
//printf("tmp=%2d\r\n" , itemp );
               proc_display( itemp );
               iMode=1;                       
            }else{
               if(mReceive.length() >=6){
                   sBuff=mReceive.substr(2,4);
printf("sBuff=%s\r\n" ,sBuff.c_str() );
                   proc_dispTime(sBuff);
               }
               iMode=0;                       
            }
           mSTAT =mSTAT_CONECT;
       }
   } // end_while

}
