#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Util/Serialisable.hpp"
#include "Tasks/TaskInterface.hpp"
#include "Tasks/AbortTask.hpp"
#include "Tasks/GotoTask.hpp"
#include "Tasks/OrderedTask.hpp"
#include "TaskStats/TaskStats.hpp"
#include "GlideSolvers/GlidePolar.hpp"
#include "TaskEvents.hpp"
#include "TaskBehaviour.hpp"


class Waypoints;
class TaskVisitor;

class TaskManager: 
 public TaskInterface,
 public Serialisable
{
public:
/** 
 * Constructor for task manager
 * 
 * @param te Task events callback object
 * @param tb Task behaviour options
 * @param gp Glide polar used for task calculations
 * @param wps Waypoint system for use by AbortTask
 * 
 * @return Initialised object
 */
  TaskManager(const TaskEvents &te,
              const TaskBehaviour &tb,
              GlidePolar &gp,
              const Waypoints &wps);

/** 
 * Sets active taskpoint sequence for active task
 * 
 * @param index Sequence number of task point
 */
    virtual void setActiveTaskPoint(unsigned index);

/** 
 * Accessor of current task point of active task
 * 
 * @return TaskPoint of active task point
 */
    virtual TaskPoint* getActiveTaskPoint();

  enum TaskMode_t {
    MODE_NULL=0,
    MODE_ORDERED,
    MODE_ABORT,
    MODE_GOTO
  };

  /** 
   * Reset the ordered task (as if never flown)
   * 
   */
  void reset();

/** 
 * Set active task to abort mode.
 * 
 */
  void abort();

/** 
 * Sets active task to ordered task (or goto if none exists) after
 * goto or aborting.
 * 
 */
  void resume();

/** 
 * Sets active task to go to mode, to specified waypoint
 * 
 * @param wp Waypoint to go to
 */
  void do_goto(const Waypoint & wp);

/** 
 * Updates internal state of task given new aircraft.
 * Only essential calculations are performed here;
 * other calculations and housekeeping may be performed
 * by update_idle
 * 
 * @param state_now Current aircraft state
 * @param state_last Aircraft state at last update 
 * @return True if internal state changed
 */
  virtual bool update(const AIRCRAFT_STATE &state_now, 
                      const AIRCRAFT_STATE &state_last);

/** 
 * Updates internal state of task to produce
 * auxiliary information or to perform slow house-keeping
 * functions that are non-essential.
 * 
 * @param state Current aircraft state
 * 
 * @return True if internal state changed
 */
  virtual bool update_idle(const AIRCRAFT_STATE &state);

/** 
 * Accessor for statistics of active task
 * 
 * @return Statistics of active task
 */
  virtual const TaskStats& get_stats() const;

/** 
 * Add taskpoint to ordered task.  It is the
 * user's responsibility to ensure the task is
 * valid (has a start/intermediate/finish).
 * 
 * @param new_tp New taskpoint to add
 * 
 * @return True if operation successful
 */
  bool append(OrderedTaskPoint *new_tp);

/** 
 * Insert taskpoint to ordered task.  It is the
 * user's responsibility to ensure the task is
 * valid (has a start/intermediate/finish).
 * 
 * @param new_tp New taskpoint to insert
 * @param position Sequence before which to insert new task point
 * 
 * @return True if operation successful
 */
  bool insert(OrderedTaskPoint *new_tp, unsigned position);

/** 
 * Remove taskpoint from ordered task.  It is the
 * user's responsibility to ensure the task is
 * valid (has a start/intermediate/finish).
 * 
 * @param position Sequence number of taskpoint to remove
 * 
 * @return True if operation successful
 */
  bool remove(unsigned position);

/** 
 * Check whether ordered task is valid
 * 
 * @return True if task is valid
 */
  bool check_task() const;


#ifdef DO_PRINT
  virtual void print(const AIRCRAFT_STATE &location);
#endif

private:
  const TaskStats null_stats;

  TaskMode_t set_mode(const TaskMode_t mode);

  AbstractTask* active_task;

  TaskMode_t mode;


  /** @link aggregation */
  OrderedTask task_ordered;

  /** @link aggregation */
  GotoTask task_goto;
  
  /** @link aggregation */
  AbortTask task_abort;
  
  /** @link aggregation */
  TaskAdvance task_advance;
  
  const TaskBehaviour &task_behaviour;
  
public:
  void Accept(BaseVisitor& visitor) const;
};
#endif //TASKMANAGER_H
