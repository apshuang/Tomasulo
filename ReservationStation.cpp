#include "BasicDefine.h"
#include "ReservationStation.h"


void ReservationStationLine::Reset() {
	busy = 0;
	Op = "";
	Vj = 0;
	Vk = 0;
	Vj2 = "";
	Vk2 = "";
	Qj = 0;
	Qk = 0;
	value = 0;
	valueString = "";
	state = FREE;
	destination = -1;
	remainingTime = -1;
}
ReservationStationLine::ReservationStationLine() {
	Reset();
}



ReservationStationADD::ReservationStationADD() {
	head = ADDNUM - 1;
	tail = ADDNUM - 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < ADDNUM; i++) {
		addModule[i] = ReservationStationLine();
	}
}

int ReservationStationADD::isFree() {
	if (head == ((tail + 1) % ADDNUM)) {
		return -1; //队列已满
	}
	else {
		tail += 1;
		tail %= ADDNUM;
		return tail;
	}
}



ReservationStationMULT::ReservationStationMULT() {
	head = MULTNUM - 1;
	tail = MULTNUM - 1; //这样定义循环队列保证第一次访问到第0行 
	for (int i = 0; i < MULTNUM; i++) {
		multModule[i] = ReservationStationLine();
	}
}

int ReservationStationMULT::isFree() {
	if (head == ((tail + 1) % MULTNUM)) {
		return -1; //队列已满
	}
	else {
		tail += 1;
		tail %= MULTNUM;
		return tail;
	}
}
