/*	File: PagingProgram.c
	Author: Jonathan Carpenter
	class: CEG4350 Summer 2015
	project 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef enum {false, true} bool;

typedef struct {
  bool isPagedIn;
  int pageNum;
  int *refIndex; //Indexes of page in reference string
  int numRefs; //Number of references in refIndex
  int nextIndex; //last used reference in refIndex
  int lastTimeUsed; //last time the page was referenced
}Page;

typedef struct {
  int pageNum; //page number in frame
  int counter; //counts when last page was put in frame
  bool hasPage; //true if page is in frame else false
} Frame;

//swaps out victim page of frame if frame is occupied then brings in new page
void swapPageIn(Frame *frame, Page *pageOut, Page *pageIn, int *counter);
//Returns: number of page faults for a given paging algorithm
int getPagefaults(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages,
		int (*pageingAlgorithm)(Frame*, int, Page*, int, int*));
//Selects a unoccupied frame or oldest page in a frame then pages out and pages in
void FIFO(Frame *frames, int numFrames, Page *pages, int currentRef, int *counter);
//Selects a unoccupied frame or a frame with a page that is going to be referenced more further from the current referenced page
void Optimal(Frame *frames, int numFrames, Page *pages, int currentRef, int *counter);
//Selects a unoccupied frame or a frame with a page that has not been used for the longest period of time
void LRU(Frame *frames, int numFrames, Page *pages, int currentRef, int *counter);

void LFU(Frame *frames, int numFrames, Page *pages, int currentRef, int *counter);

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

  int (*pageingAlgorithm)(Frame*, int, Page*, int, int*);

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
    p.pageNum = index;
    p.isPagedIn = false;
    p.nextIndex = 0;
    p.numRefs = 0;
    p.lastTimeUsed = 0;
    pages[index] = p;
  }

  /* set refIndex for pages */

  //set numRefs for each page
  for(index = 0; index < numRefs; ++index) {
    tmp = pageRefs[index];
    pages[tmp].numRefs++;
  }
  //allocate memory for the reference index of each page
  for(index = 0; index < numPages; ++index) {
    pages[index].refIndex = calloc(pages[index].numRefs, sizeof(int));
  }
  //set refIndex for each page
  for(index = 0;index < numRefs; ++index) {
    tmp = pageRefs[index];
    pages[tmp].refIndex[pages[tmp].nextIndex++] = index;
  }
  //initialize currentIndex to 0 for all pages
  for(index = 0;index < numPages; ++index) {
	 pages[index].nextIndex = 0;
  }
  //create dynamic array of frames
  frames = calloc(numFrames, sizeof(Frame));
  for(index = 0; index < numFrames; ++index) {
    Frame frame;
    frame.hasPage = false;
    frame.counter = 0;
    frame.pageNum = -1;
    frames[index] = frame;
  }

  pageingAlgorithm = LRU;
  int pagefaults = getPagefaults(pageRefs, numRefs, frames, numFrames, pages, numPages, pageingAlgorithm);

  printf("Page faults: %d\n", pagefaults);

  free(pageRefs);
  free(frames);
  free(pages);

  return EXIT_SUCCESS;
}

//swaps out victim page of frame if frame is occupied then brings in new page
void swapPageIn(Frame *frame, Page *pageOut, Page *pageIn, int *counter){
	//page out victim
	if(pageOut != NULL && frame -> pageNum != -1) {
		pageOut -> isPagedIn = false;
	}

	/* page in new page */

	//check if pageIn is referenced again in the future
	if(pageIn -> nextIndex + 1 < pageIn -> numRefs) {
		pageIn -> nextIndex++;
	}
	if(counter != NULL) {
		frame -> counter = (*counter)++;
	}
	frame -> hasPage = true;
	frame -> pageNum = pageIn -> pageNum;
	pageIn -> isPagedIn = true;
}

//Implementing a priority queue would be useful for this function
int getPagefaults(int *pageReferences, int numRefs, Frame *frames, int numFrames, Page *pages, int numPages,
		int (*pageingAlgorithm)(Frame*, int, Page*, int, int*)) {
  int i, pagefaults = 0;
  int currentRef;
  int counter = 1;

  for(i = 0;i < numRefs; ++i) {
    currentRef = pageReferences[i];
    pages[currentRef].lastTimeUsed = counter++;
    //check if page is not in frame
    if(pages[currentRef].isPagedIn != true) {

      /* page fault has occurred
	 page in new page */
      pagefaults++;;
      pageingAlgorithm(frames, numFrames, pages, currentRef, &counter);
      }
    }
  return pagefaults;
}

//place page in a frame with first in first out algorithm
void FIFO(Frame *frames, int numFrames, Page *pages, int currentRef, int *counter) {
  int i, victimPage;
  double victimTime = (*counter)++;
  int freedFrame;

  for(i = 0; i < numFrames; ++i) {
    //check if frame is available
    if(frames[i].hasPage == false) {
    	swapPageIn(&frames[i], NULL, &pages[currentRef], counter);
    	return;
    }
    //check if frame has the oldest page in case we need a victim page
    if(frames[i].counter < victimTime) {
    	freedFrame = i;
    	victimTime = frames[i].counter;
    }

  }
  victimPage = frames[freedFrame].pageNum;
  swapPageIn(&frames[freedFrame], &pages[victimPage], &pages[currentRef], counter);
}

void Optimal(Frame *frames, int numFrames, Page *pages, int currentRef, int* counter) {
	int i, pageNumber;
	int nextIndex; // a page's nextIndex
	int nextPageReference; // the page's next reference
	int maxReference = -1; // page with reference occurring latest is paged out
	Page *p;
	Page *pageOut; // victim page
	Page *pageIn = &pages[currentRef]; // page to be placed in a frame
	for(i = 0; i < numFrames; ++i) {
		//if frame is free then place the page in it
		if(frames[i].hasPage == false) {
			swapPageIn(&frames[i], NULL, &pages[currentRef], NULL);
			return;
		}

		/* find page that is not going to be referenced soon */

		pageNumber = frames[i].pageNum;
		p = &pages[pageNumber];
		nextIndex = p -> nextIndex;
		nextPageReference = p -> refIndex[nextIndex];

		//if page is never referenced again swap it out and swap in new page
		if(nextPageReference < currentRef) {
			swapPageIn(&frames[i], p, pageIn, NULL);
			return;
		}
		//check if page is the one not referenced for the longest period of time
		if(nextPageReference > maxReference) {
			pageOut = p;
			maxReference = nextPageReference;
		}
	}
	swapPageIn(&frames[i], pageOut, pageIn, NULL);
}

void LRU(Frame *frames, int numFrames, Page *pages, int currentRef, int* counter) {
	int i, freedFrame, pageNumber, victimPage,  victimTime = (*counter)++;

	for(i = 0;i < numFrames; ++i) {
		//check if frame is empty
		if(frames[i].hasPage == false) {
			swapPageIn(&frames[i], NULL, &pages[currentRef], counter);
			return;
		}
		//check if frame has the page that has not been used for the longest period of time
		pageNumber = frames[i].pageNum;
		if(pages[pageNumber].lastTimeUsed < victimTime) {
			freedFrame = i;
			victimTime = pages[pageNumber].lastTimeUsed;
		}
	}
	victimPage = frames[freedFrame].pageNum;
	swapPageIn(&frames[freedFrame], &pages[victimPage], &pages[currentRef], counter);
}
