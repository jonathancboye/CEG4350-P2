//File: PagingProgram.c
//Author: Jonathan Carpenter
//class: CEG4350 Summer 2015
//project 2

#include <stdlib.h>
#include <time.h>

typedef enum {false, true} bool;

typedef struct {
  bool isPagedIn;
  int *refIndex; //Indexs of page in reference string
  int numRefs; //Number of references in refIndex
  int currentIndex; //last used reference in refIndex
  double lastUsedTime; //last time the page was referenced
}Page;

typedef struct {
  int pageNum;
  double pagedInTime;
  bool hasPage;
} Frame;

//FIFO paging algorithm
//Returns: number of page faults
int FIFO(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages);

int pageIn_Optimal(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages);

//Selects a unoccupied frame or oldest page in a frame then pages out and pages in
void pageIn_FIFO(Frame *frames, int numFrames, Page *pages, int currentRef);



int main(int argc, char *argv[]) {

  int index; //index for looping
  int maxPageReferenced = 0; //largest number in page reference sequence
  int tmp; //temporary value

  int numRefs; //number of page references
  int *pageRefs; //dynamic array of page references

  int numFrames; //number of frames
  Frame *frames; //dynamic arrray of frames

  int numPages; //number of pages
  Page *pages; //dynamic array of pages



  int p = sizeof(Page); //debugging
  int f = sizeof(Frame); //debugging
  int integer = sizeof(int); // debugging




  //check command line arguments
  if(argc < 3) {
    printf("Needs command line arguments: ./program <int1> <sequence>\n");
    printf("Where: <int> - number of frames\n");
    printf("<sequence> - a sequence of numbers repesenting a reference string\n");
    return EXIT_FAILURE;
  }

  numFrames = atoi(argv[1]);   //get number of frames from command line
  numRefs = argc - 2;

  //create dynamic array of page references
  pageRefs = calloc(numRefs, sizeof(int));
  for(index = 2; index < argc; ++index) {
    tmp = atoi(argv[index]);
    if(maxPageReferenced < tmp){
      maxPageReferenced = tmp;
    }
    pageRefs[index - 2] = tmp;
  };


  //maxPageReferenced + 1 because we want the largest referenced number to be
  //addressible like pages[maxPageReferenced]
  numPages = maxPageReferenced + 1;

  //create dynamic array of pages
  pages = calloc(numPages, sizeof(Page));
  for(index = 0;index < numPages; ++index) {
    Page p;
    p.isPagedIn = false;
    p.currentIndex = 0;
    pages[index] = p;
  }

  /*set refIndex for pages*/

  //set numRefs for each page
  for(index = 0; index < numRefs; ++index) {
    tmp = pageRefs[index];
    pages[index].numRefs++;
  }
  //allocate memory for the reference index of each page
  for(index = 0; index < numPages; ++index) {
    pages[index].refIndex = calloc(pages[index].numRefs, sizeof(int));
  }
  //set refIndex for each page
  for(index = 0;index < numRefs; ++index) {
    tmp = pageRefs[index];
    pages[tmp].refIndex[pages[tmp].currentIndex++];
  }


  //create dynamic array of frames
  frames = calloc(numFrames, sizeof(Frame));
  for(index = 0; index < numFrames; ++index) {
    Frame frame;
    frame.hasPage = false;
    frame.pagedInTime = 0;
    frame.pageNum = -1;
    frames[index] = frame;
  }

  int pagefaults = FIFO(pageRefs, numRefs, frames, numFrames, pages, numPages);
  printf("Page faults: %d\n", pagefaults);

  free(pageRefs);
  free(frames);
  free(pages);
}

void  pageIn_FIFO(Frame *frames, int numFrames, Page *pages, int currentRef) {
  int i;
  double victimTime = (double)clock();
  int freedFrame;

  for(i = 0; i < numFrames; ++i) {
    //check if frame is available
    if(frames[i].hasPage == false) {
      freedFrame = i;
      break;
    } else {
      //check if frame has the oldest page incase we need a victim page
      if(frames[i].pagedInTime != 0 && frames[i].pagedInTime < victimTime) {
	freedFrame = i;
      }
    }
  }

  //page out victim
  int victimPage = frames[freedFrame].pageNum;
  if(victimPage != -1) {
    pages[victimPage].isPagedIn = false;
    frames[freedFrame].hasPage = false;
    frames[freedFrame].pagedInTime = 0;
    frames[freedFrame].pageNum = -1;
  }

  //page in new page
  frames[freedFrame].hasPage = true;
  frames[freedFrame].pagedInTime = clock();
  frames[freedFrame].pageNum = currentRef;
  pages[currentRef].isPagedIn = true;
}

//Implementing a priority queue would be useful for this function
int FIFO(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages) {
  int i, pagefaults = 0;
  int currentRef;
  for(i = 0;i < numRefs; ++i) {
    currentRef = pageReferences[i];
    if(pages[currentRef].isPagedIn != true) {

      /* page fault has occured
	 page in new page */

      pagefaults++;
      pageIn_FIFO(frames, numFrames, pages, currentRef);
    }
  }
  return pagefaults;
}

int pageIn_Optimal(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages){
  return 0;
}
