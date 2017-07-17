

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>



void main(int argc, char **argv){
  int numFrames= atoi(argv[1]), pagefaults= 0;
  int refStr[5000];      //this will be the list of the frames
  int refCount= 0, fault;
  int idx=0;
  while(1) {      //get the sequence of frames
      int num;
      if( 0 > scanf("%d", & num)){
        break;
      }
      refStr[idx]= num;   //add each frame to an array for later use
      refCount+=1;        //count the number of frames in the sequence
      idx++;
  }


  //now we have the list of frames in order

  //initialize variables for Optimal alg
  int pagefault=0;
  int Oframes[numFrames];
  int Ocounts[101];
  int Ocount=0;
  /*OPTIMAL ALGORITHM*/
  printf("Optimal:\n");
  idx=0;
  int frameidx=0;
  while(idx< refCount){
    if(Ocount < numFrames){     //check if the frame is full
      Oframes[Ocount]= refStr[idx]; //if it is not, the page can just be added
      frameidx++;
      pagefault++;              //increment the numerof page faults
      Ocount++;
    }
    else{
      fault=1;
      for(int k=0; k < numFrames; k++){   //loop trough the frames already loaded
        if(refStr[idx]== Oframes[k]){     //if the frame is already loaded, no page fault is needed
          fault=0;                        //ignore the algorithm
          break;
        }
      }
      if(fault){
        pagefault++;
        for(int i=0; i < refCount; i++){    //initialize counters to 0
          Ocounts[i]=0;
        }
        for(int i =0; i< numFrames; i++){    //loop through the current frames
          for(int j=idx; j < refCount; j++){  //loop from current sequence position to count how far away in the
            if(refStr[j]==Oframes[i]){        //sequence each frame is
              j= refCount;                    //when the frame is found, stop searching
            }
            Ocounts[Oframes[i]]++;            //increment counter to count positions until next frame use
          }
        }
        int victim=0;                         //initialize victim to 0
        for(int i =0; i< 101; i++){           //loop throug the counts
          if(Ocounts[i]> Ocounts[victim]){    //check if the current counter is larger than te victim's
            victim = i;                       //if so, update the victim
          }
        } //now we have the victim
        for(int i=0; i < numFrames; i++){     //loop through the current frames to find the victim
          if(Oframes[i]==victim){
            Oframes[i]= refStr[idx];          //when the victim is found, replace it with the new frame
            break;
          }
        }
      }
    }
    idx++;
  }
  printf("-frames:");
  for(int i=0; i < numFrames; i++){
    printf("  %d", Oframes[i]);
  }
  printf("\n");
  printf("-page faults:   %d\n", pagefault);

  /*LRU Algorithm*/
  //implementation is almost the same as Optimal, except line 126
  printf("LRU:\n");
  pagefault=0;
  int LRUframes[numFrames];
  int LRUcounts[101];
  int LRUcount=0;
  idx=0;
  frameidx=0;
  pagefault=0;
  while(idx< refCount){
    //printf("Working");
    if(LRUcount < numFrames){
      LRUframes[frameidx]= refStr[idx];
      frameidx++;
      pagefault++;
      LRUcount++;
    //  printf("No fault: %d\n", refStr[idx]);
    }
    else{
      int fault=1;
      /*for(int i=0; i < numFrames; i++){
        printf("  %d", LRUframes[i]);
      }
      printf("\n");*/
      fault=1;
      for(int k=0; k < numFrames; k++){
        if(refStr[idx]== LRUframes[k]){
          fault=0;
          break;
        }
      }
      if(fault){
        pagefault++;
        for(int i=0; i < refCount; i++){
          LRUcounts[i]=0;
        }
        for(int i =0; i< numFrames; i++){    //loop through the current frames
          for(int j=idx; j >=0; j--){   //start at current sequence position and loop backwards to find least recently used
            if(refStr[j]==LRUframes[i]){
              j= -1;
            }
            LRUcounts[LRUframes[i]]++;
          }
          //  printf("Fram: %d, Count: %d, i= %d\n", LRUframes[i], LRUcounts[LRUframes[i]], i);
        }
        int victim=0;
        for(int i =0; i< 101; i++){
          if(LRUcounts[i]> LRUcounts[victim]){
            victim = i;
          }
        } //now we have the victim
        for(int i=0; i < numFrames; i++){
          if(LRUframes[i]==victim){
            LRUframes[i]= refStr[idx];
            break;
          }
        }
      }
    }
    idx++;
  }
  printf("-frames:");
  for(int i=0; i < numFrames; i++){
    printf("  %d", LRUframes[i]);
  }
  printf("\n");
  printf("-page faults:   %d\n", pagefault);

  /*CLOCK ALGORITHM*/
  printf("Clock:\n");
  int clock[101], clockFrames[numFrames];
  int pointer=0, Ccount=0;
  for(int i=0; i < 101; i++){     //initialize the clock references to 1
    clock[i]=1;
  }
  idx=0;
  pagefault=0;
  while(idx < refCount){
    int victim =0;
    if(Ccount < numFrames){       //check if the frames are full
      clockFrames[Ccount]= refStr[idx];
      pagefault++;
      Ccount++;
    }
    else{
      int fault=1;              //check if the frame is already loaded
      for(int k=0; k < numFrames; k++){
        if(refStr[idx]== clockFrames[k]){
          fault=0;
          break;
        }
      }

      if(fault){
        pagefault++;
          int j=pointer;
          while(fault){
            if(clock[clockFrames[j]]== 0){      //check if the reference is 1 or 0
              victim= clockFrames[j];           //if the ref is 0, that is the victim
              pointer =j;                       //point to start the next search
              break;
            }
            else{
              clock[clockFrames[j]]= 0;         //if the ref was 1, change it to 0
            }
            j++;
            if(j == numFrames){                 //if all the frames have been checked
              j=0;                              //check from the beginning
            }
          }
          }
          for(int i=0; i < numFrames; i++){   //loop through the frames
            if(clockFrames[i]==victim){       //find the victim
              clockFrames[i]= refStr[idx];    //replace the frame with the new one from the sequence
              clock[clockFrames[i]]=1;        //switch the ref to 1, to give it a second chance
              break;
            }
          }
        }
          idx++;
      }

  printf("-frames:");
  for(int i=0; i < numFrames; i++){
    printf("  %d", clockFrames[i]);
  }
  printf("\n");
  printf("-page faults:   %d\n", pagefault);
}
