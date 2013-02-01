/*
 * Scheduler.h
 *
 *  Created on: 09.12.2012
 *      Author: pyro
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

class Scheduler {
private:

	Scheduler();
public:
	static Scheduler instance;

	Scheduler(const Scheduler&) = delete;

	virtual ~Scheduler();
};

#endif /* SCHEDULER_H_ */
