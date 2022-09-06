#include <iostream>
#include <stdlib.h>

using namespace std;

typedef struct zoneNode *nodeptr;
typedef struct zoneNode {
	int addr;
	int size;
	nodeptr next;
} zonenode; //every zones

typedef struct Dynamicalzone *dptr;
typedef struct Dynamicalzone {
	nodeptr head;
	nodeptr back;
	nodeptr assign;
	int start, end;
} dynamicalzone; //zones manager

nodeptr newNode(int addr, int size) {
	nodeptr p = (nodeptr)malloc(sizeof(zoneNode));
	p->addr = addr;
	p->size = size;
	p->next = NULL;
	return p;
} //newZone

dptr spawnZoneManager() {
	dptr p = (dptr)malloc(sizeof(dynamicalzone));
	p->head = NULL;
	p->back = NULL;
	p->assign = NULL;
	return p;
} //newZoneManager

nodeptr check(dptr zm) {
	nodeptr p = zm->head;
	// choose to insert
	while(p) { 
		if(p->addr <= zm->assign->addr && p->size + p->addr >= zm->assign->size + zm->assign->addr) return p;
		p = p->next; // if required addr >= addr that is free and 
		     // required size <= this size, return current node 
		     // if retrieve, reverse the result;
	}
	return NULL; // if not, return NULL
}

bool createProcess(dptr zm) { //Create the process existed
	nodeptr p = check(zm); // Judge whether it can insert 
	if(!p) {
		printf("Reason is seeking failed, ");
		return false;
	}
	else {
		if(zm->assign->addr == p->addr) {
			p->addr = zm->assign->addr + zm->assign->size;
			p->size -= zm->assign->size;
			if(p->size == 0 && p->next) {
				*p = *p->next;
			} else if(p->size == 0 && !zm->head->next) {
				zm->head = NULL;
			} else if(p->size == 0 && !p->next){
				nodeptr px = zm->head;
				while(px->next->next) px = px->next;
				px->next = NULL;
			} 
		} else {
			nodeptr px = newNode(zm->assign->size + zm->assign->addr, p->size - zm->assign->addr - zm->assign->size + p->addr);
			p->size = zm->assign->addr - p->addr;
			px->next = p->next;
			p->next = px;
		}
		nodeptr backp = zm->back;
		while(backp) backp = backp->next;
		backp = zm->assign;
		return true;
	}  
}

void firstFitAlgorithm(dptr zm, int size, int flag) {
	nodeptr p = zm->head;
	while(p && flag) { // Distribution by FirstFit
		if(p->size > size) {
			p->addr = p->addr + size;
			p->size = p->size - size;
			printf("Distribute successful!\n");
			return ;
		} else if(p->size == size && !zm->head->next) { // judge whether it is the last node in the linkedlist
			zm->head = NULL;
			printf("Distribute successful!\n");
			return ; 
		} else if(p->size == size && !p->next) { // judge whether it is the tail node
			nodeptr px = zm->head;
			while(px->next->next) px = px->next;
			px->next = NULL;
			printf("Distribute successful!\n");
			return ; 
		} else if(p->size == size) { // normal situation
			*p = *p->next;
			printf("Distribute successful!\n");
			return ; 
		}
		p = p->next;
	} 
	if(flag){ 
		printf("Distribute Failed! Reason is the size can't fit the zone!\n");
		return ;
	}
	int last = zm->start;
	if(!zm->head && !flag && zm->end - zm->start >= size) { // Due to the empty free space, the algorithm need to create the new node
		zm->head = newNode(zm->start, size);
		printf("Retrieve successful!\n");
		return ;
	}
	p = zm->head;
	while(p &&!flag) { // Retrieve by FirstFit
		if(p->addr - last >= size) {
			p->addr = p->addr - size;
			p->size = p->size + size;
			printf("Retrieve successful!\n");
			return ; 
		}
		if(!p->next && p->addr + p->size + size <= zm->end) {
			p->size = p->size + size;
			printf("Retrieve successful!\n");
			return ; 
		}
		last = p->addr + p->size;
		p = p->next;
	}
	printf("Retrieve Failed! Reason is the size can't fit the zone!\n");
} // In 'r', we can't estimate the hard drive storaged chips!

void bestFitAlgorithm(dptr zm, int size, int flag) { 
	if(flag) { // Distribution by BestFit
		int minSize = size;
		nodeptr p = zm->head;
		while(p) {
			if((minSize >= p->size && minSize != size && size <= p->size) || (minSize == size && p->size > minSize)) minSize = p->size;
			if(p->size == size && !zm->head->next) { // judge whether it is the last node in the linkedlist
				zm->head = NULL;
				printf("Distribute successful!\n");
				return ; 
			} else if(p->size == size && !p->next) { // judge whether it is the tail node
				nodeptr px = zm->head;
				while(px->next->next) px = px->next;
				px->next = NULL;
				printf("Distribute successful!\n");
				return ; 
			} else if(p->size == size) { // normal situation
				*p = *p->next;
				printf("Distribute successful!\n");
				return ; 
			}
			p= p->next;
		}
		if(minSize == size) {
			printf("Distribute Failed! Reason is the size can't fit the zone!\n");
			return ;
		}
		p = zm->head;
		while(p) {
			if(minSize == p->size) {
				p->addr = p->addr + size;
				p->size = p->size - size;
				printf("Distribute successful!\n");
				return ;
			}
			p= p->next;
		}
		printf("Distribute Failed! Reason is the size too big to not do fit the zone!\n");
		return ;
	} else { // Retrieve by BestFit
		int minSize = size;
		nodeptr p = zm->head;
		int last = zm->start;
		if(!zm->head && zm->end - zm->start >= size) { // Due to the empty free space, the algorithm need to create the new node
			zm->head = newNode(zm->start, size);
			printf("Retrieve successful!\n");
			return ;
		}
		while(p) {
			if((minSize >= p->addr - last && minSize != size && size <= p->addr - last) || (minSize == size && p->addr - last > minSize)) 
				minSize = p->addr - last; 
			if(p->addr - last == size) {
				p->addr = p->addr - size;
				p->size = p->size + size;
				printf("Retrieve successful!\n");
				return ; 
			}
			if(!p->next && p->addr + p->size + size == zm->end) {
				p->size = p->size + size;
				printf("Retrieve successful!\n");
				return ; 
			}
			last = p->addr + p->size;
			p = p->next;
		}
		
		if(minSize == size) {
			printf("Retrieve Failed! Reason is the size can't fit the zone!\n");
			return ;
		}
		last = zm->start;
		p = zm->head;
		while(p) {
			if(minSize == p->addr - last) {
				p->addr -= size;
				p->size += size;
				printf("Retrieve successful!\n");
				return ;
			}
			if(!p->next && p->addr + p->size + size <= zm->end) {
				p->size = p->size + size;
				printf("Retrieve successful!\n");
				return ; 
			}
			last = p->addr + p->size;
			p = p->next;
		}
		printf("Retrieve Failed! Reason is the size too small to not do fit the zone!\n");
		return ;
	}
}

void worstFitAlgorithm(dptr zm, int size, int flag) {
	if(flag) {
		nodeptr p = zm->head;
		int maxSize = size;
		while(p) { // Find the maxSize
			if(maxSize < p->size) maxSize = p->size;
			p = p->next;
		}
		p = zm->head; // restart the for(p ; p < zm->head; p++)
		if(maxSize == size) {
			while(p) {
				if(p->size == size && !zm->head->next) { // judge whether it is the last node in the linkedlist
					zm->head = NULL;
					printf("Distribute successful!\n");
					return ; 
				} else if(p->size == size && !p->next) { // judge whether it is the tail node
					nodeptr px = zm->head;
					while(px->next->next) px = px->next;
					px->next = NULL;
					printf("Distribute successful!\n");
					return ; 
				} else if(p->size == size) { // normal situation
					*p = *p->next;
					printf("Distribute successful!\n");
					return ; 
				}
				p = p->next;
			}
			printf("Distribute Failed! Reason is the size can't fit the zone!\n");
			return ;
		}
		while(p) { // Operate the maxsize node
			if(maxSize == p->size) {
				p->addr = p->addr + size;
				p->size = p->size - size;
				printf("Distribute successful!\n");
				return ;
			}
			p = p->next;
		}
		printf("Distribute Failed! Reason is the size too big to not do fit the zone!\n");
		return ;
	} else {
		if(!zm->head && zm->end - zm->start >= size) { // Due to the empty free space, the algorithm need to create the new node
			zm->head = newNode(zm->start, size);
			printf("Retrieve successful!\n");
			return ;
		}
		int maxSize = size;
		nodeptr p = zm->head;
		int last = zm->start;
		while(p) {
			if(p->addr - last > maxSize) maxSize = p->addr - last;
			if(!p->next && maxSize < zm->end - p->addr - p->size) maxSize = zm->end - p->addr - p->size;
			last = p->addr + p->size;
			p = p->next;
		} // Find the maxSize
		last = zm->start;
		p = zm->head;
		if(maxSize == size) {
			while(p) {
				if(p->addr - last == size) {
					p->addr = p->addr - size;
					p->size = p->size + size;
					printf("Retrieve successful!\n");
					return ;
				}
				if(!p->next && p->addr + p->size + size == zm->end) {
					p->size = p->size + size;
					printf("Retrieve successful!\n");
					return ; 
				}
				last = p->addr + p->size;
				p = p->next;
			}
			printf("Retrieve Failed! Reason is the size can't fit the zone!\n");
			return ;
		} // Judge whether the program exists the "size", if not, return 0, else return 1;
		while(p) {
			if(!p->next && maxSize == p->addr - last && zm->end >= p->addr + p->size + size) {
				p->size = p->size + size;
				printf("Retrieve successful!\n");
				return ; 
			} else if(maxSize == p->addr - last) {
				p->addr -= size;
				p->size += size;
				printf("Retrieve successful!\n");
				return ;
			}
			
			last = p->addr + p->size;
			p = p->next;
		} // Operate the maxSize 
		printf("Retrieve Failed! Reason is the size too small to not do fit the zone!\n");
		return ;
	}
}

void createZone(dptr zm) {
	while(true) {
		printf("-----------------------------------------\n");
		printf("Set the process running\n");
		printf("-----------------------------------------\n");
		int addr, size;
		printf("Please input the addr you need to set:(int) ");
		scanf("%d", &addr);
		printf("Please input the size you need to set:(int) ");
		scanf("%d", &size);
		zm->assign = newNode(addr, size);
		if(createProcess(zm)) printf("Create process successful!\n");
		else printf("Create process Failed!!\n");		
		
		printf("Do you want to insert again!(y/n)");
		char ch;
		scanf("\n%c", &ch);
		if(ch == 'n') break;
		else if(ch != 'y') {
			printf("Input Error!\n");
			break;		
		}
	}
}

void busyZoneDisplay(dptr zm) {
	nodeptr p = zm->head;
	printf("-----------------------------------------\n");
	printf("Current zone situation:\n");
	printf("addr		size		situation\n");
	if(!zm->head) {
		printf("%d		%d	   	busy\n", zm->start, zm->end - zm->start);
		printf("-----------------------------------------\n");
	} else {
		int last = zm->start;
		while(p) {
			if(p->addr - last > 0) printf("%d		%d	   	busy\n", last, p->addr - last);
			last = p->addr + p->size;
			p = p->next;
		}
		if(zm->end - last > 0) printf("%d		%d	   	busy\n", last, zm->end - last);
		printf("-----------------------------------------\n");
	}
}

void freeZoneDisplay(dptr zm) {
	printf("-----------------------------------------\n");
	printf("Current zone situation:\n");
	printf("addr		size		situation\n");
	nodeptr p = zm->head;
	while(p){
		printf("%d		%d	   	free\n", p->addr, p->size);
		p = p->next;
	}
	printf("-----------------------------------------\n");
}

void ui(dptr zm, int flag) {
	printf("-----------------------------------------\n");
	printf("Dynamical zone managing machine\n");
	printf("-----------------------------------------\n");
	while(true) {
		printf("f) Display the free zones\n");
		printf("b) Display the busy zones\n");
		printf("n) Display Nothing");
		printf("Check current zone situation:(f/b/n)");
		char c1;
		scanf("\n%c", &c1);
		if(c1 == 'f') freeZoneDisplay(zm);
		else if(c1 == 'b') busyZoneDisplay(zm);
		else if(c1 != 'n') {
			printf("Input Error!\n");
			continue;
		}
		
		printf("Please input d or r to judge whether you need to distribute or retrieve:(d/r)"); 
		// distribute or retrieve
		char c;
		scanf("\n%c", &c);
		if(c == 'd') {
			int size;
			printf("Please input the size of process required to distribute:(int)");
			scanf("%d", &size);
			if(flag == 1) firstFitAlgorithm(zm, size, 1);
			else if(flag == 2) bestFitAlgorithm(zm, size, 1);
			else if(flag == 3) worstFitAlgorithm(zm, size, 1); 
			else {
				printf("Input Error!!\n");
				return ;
			}
		} else if(c == 'r') {
			int size;
			printf("Please input the size of process required to retrieve:(int)");
			scanf("%d", &size);
			if(flag == 1) firstFitAlgorithm(zm, size, 0);
			else if(flag == 2) bestFitAlgorithm(zm, size, 0);
			else if(flag == 3) worstFitAlgorithm(zm, size, 0); 
			else {
				printf("Input Error!!\n");
				return ;
			}
		} else {
			printf("Input Error!!\n");
		}
		char ch;
		printf("Do you want to try again?(y/n)");
		scanf("\n%c", &ch);
		if(ch == 'n') break;
		else if(ch != 'y') {
			printf("Input Error!!\n");
			return ;
		}
	}
}

int main() {
		int addr, size;
		printf("-----------------------------------------\n");
		printf("Dynamical zone setting\n");
		printf("-----------------------------------------\n");
		printf("Please input the addr you want to set:(int) ");
		scanf("%d", &addr);
		printf("Please input the size you want to set:(int) ");
		scanf("%d", &size);
		dptr zm = spawnZoneManager();
		zm->head = newNode(addr, size);
		zm->start = addr;
		zm->end = addr + size;
		
		createZone(zm);
		
		int choose1;
		printf("Please input the way how to manage the zone:\n");
		printf("1) FirstFitAlgorithm\n");
		printf("2) BestFitAlgorithm\n");
		printf("3) WorstFitAlgorithm\n");
		printf("You choose? (1/2/3)");
		scanf("%d", &choose1);
		ui(zm, choose1);
		
		return 0;
}
