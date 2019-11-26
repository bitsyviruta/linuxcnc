#ifndef DRYRUN_H
#define DRYRUN_H
#include <rtapi_bool.h>

void dryrun_manage(void);  // per cycle
bool dryrun_end(void);     // query
bool dryrun_active(void);  // query
void dryrun_restore(void); // restore state

void dryrun_process_inputs(void); // substitue function
void dryrun_output_to_hal( void); // substitue function
void dryrun_update_status( void); // substitue function

// for debugging (for rtpreempt, uses stdio):
void dryrun_show(bool force,char* msg);
#endif /* DRYRUN_H */
