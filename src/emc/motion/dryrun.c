/*
Copyright: 2019
Author: Dewey Garrett <dgarrett@panix.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// debugging using stdio (use with rtpreempt only):
#define DRYRUN_DBG
#undef  DRYRUN_DBG
#ifdef  DRYRUN_DBG //{
#include <stdio.h>
#endif             //}

#include "hal.h"
#include "mot_priv.h"
#include "dryrun.h"
#include "motion_debug.h"
#include <rtapi_math.h>

// strings for translation
#define _(s) (s)

static EmcPose save_pose;
static double  save_joints[EMCMOT_MAX_JOINTS];

static int       dryrun_saved = 0;
static int last_dryrun_active = 0;
static int     dryrun_started = 0;
static int    dryrun_stopping = 0;
static int        dryrun_fini = 0;
static int          dryrun_ct = 0;

// local vars & functions -------------------------------------
static double *pcmd_p[EMCMOT_MAX_AXIS];
static void dryrun_once(void)
{
    // *pcmd_p[0] is emcmotStatus->carte_pos_cmd.tran.x
    // *pcmd_p[1] is emcmotStatus->carte_pos_cmd.tran.y
    //  etc.
    pcmd_p[0] = &(emcmotStatus->carte_pos_cmd.tran.x);
    pcmd_p[1] = &(emcmotStatus->carte_pos_cmd.tran.y);
    pcmd_p[2] = &(emcmotStatus->carte_pos_cmd.tran.z);
    pcmd_p[3] = &(emcmotStatus->carte_pos_cmd.a);
    pcmd_p[4] = &(emcmotStatus->carte_pos_cmd.b);
    pcmd_p[5] = &(emcmotStatus->carte_pos_cmd.c);
    pcmd_p[6] = &(emcmotStatus->carte_pos_cmd.u);
    pcmd_p[7] = &(emcmotStatus->carte_pos_cmd.v);
    pcmd_p[8] = &(emcmotStatus->carte_pos_cmd.w);
} // dryrun_once()

void dryrun_save(void)
{
    KINEMATICS_FORWARD_FLAGS fflags = 0;
    KINEMATICS_INVERSE_FLAGS iflags = 0;
    int joint_num;

    for (joint_num = 0; joint_num < 3+ALL_JOINTS; joint_num++) {
        save_joints[joint_num] = (&joints[joint_num])->pos_fb;
    }

    kinematicsForward(save_joints, &save_pose, &fflags, &iflags);
    dryrun_saved = 1;

    dryrun_show(1,"SAVE");
} // dryrun_save()
// -----------------------------------------------------

bool dryrun_end()
{
    return(dryrun_fini);
} // dryrun_end()

void dryrun_manage(void)
{
    static bool gave_disallow_msg = 0;
    static int               once = 1;
    if (once) {dryrun_once(); once=0;}
    dryrun_ct++; // for debugging

    // check status from prior cycle
    if (dryrun_fini) {
        dryrun_show(1,"FINI");
        dryrun_fini = 0;
    }
    if (dryrun_stopping) {
        dryrun_show(1,"STOPPING");
        dryrun_stopping = 0;
        dryrun_fini     = 1;
    };

    // Require motion enable
    if (   !GET_MOTION_ENABLE_FLAG() ) {
        *(emcmot_hal_data->is_dryrun) = 0;
        dryrun_started = 0;
        return;
    }

    // Specialcase, unhomed after starting
    if (  !checkAllHomed()
        && dryrun_started) {
        *(emcmot_hal_data->is_dryrun) = 0;
        dryrun_started = 0;
    }

    last_dryrun_active = dryrun_active();

    // Disallow dryrun startup while in coord (running prog/mdi)
    if (   *emcmot_hal_data->dryrun_start
        && !dryrun_started
        && GET_MOTION_COORD_FLAG()) {
        if (!gave_disallow_msg) {
            rtapi_print_msg(RTAPI_MSG_ERR, "Cannot start dryrun while mdi or prog"
                           "\nHint: 1) If program running, stop it first"
                           "\nHint: 2) Start dryrun from manual tab (F3)"
                           );
        }
        last_dryrun_active = 0;
        dryrun_started    = 0;
        gave_disallow_msg = 1;
        return;
    }

    // Disallow startup if not homed
    if (   *emcmot_hal_data->dryrun_start
        && !checkAllHomed() ) {
        *(emcmot_hal_data->is_dryrun) = 0;
        dryrun_started = 0;
        if (!gave_disallow_msg) {
            rtapi_print_msg(RTAPI_MSG_ERR, "Cannot start dryrun until homed");
        }
        gave_disallow_msg = 1;
        return;
    }
    // Allowed startup of dryrun
    if (   *emcmot_hal_data->dryrun_start
        && !dryrun_started) {
       dryrun_save();
    }

    // Detect disallowed ending of request
    if (   *emcmot_hal_data->dryrun_stop
        && dryrun_started
        && GET_MOTION_COORD_FLAG() ) {
        if (!gave_disallow_msg) {
            // cannot distinguish cases in motmod, so give hints:
            rtapi_print_msg(RTAPI_MSG_ERR, "Cannot stop dryrun while mdi or prog"
                            "\nHint: 1)if running program, stop it (ESC)"
                            "\nHint: 2)if mdi tab, use manual tab  (F3)"
                            );
        }
        gave_disallow_msg = 1;
        return;
    }
    gave_disallow_msg = 0;
    // Detect allowed ending of request
    if (   *emcmot_hal_data->dryrun_stop
        && dryrun_started) {
       dryrun_stopping = 1;
       SET_MOTION_COORD_FLAG(0);
       SET_MOTION_TELEOP_FLAG(1);
       dryrun_show(1,"STOP_REQUEST");
    }

    if ( *emcmot_hal_data->dryrun_start) {
        dryrun_started = 1;
    }
    if ( *emcmot_hal_data->dryrun_stop) {
        dryrun_started = 0;
    }

    *(emcmot_hal_data->is_dryrun) = dryrun_started;
} // dryrun_manage()

bool dryrun_active(void)
{
    return(   dryrun_started
           || dryrun_stopping
           || dryrun_fini
          );
} // dryrun_active()

void dryrun_restore(void)
{
    double restore_joints[EMCMOT_MAX_JOINTS];
    int joint_num,axis_num;

    if (dryrun_saved) {
        emcmotStatus->carte_pos_cmd = save_pose;
        kinematicsInverse(&emcmotStatus->carte_pos_cmd,restore_joints,&iflags,&fflags);
        for (joint_num = 0; joint_num < ALL_JOINTS; joint_num++) {
            (&joints[joint_num])->pos_cmd = restore_joints[joint_num];
            (&joints[joint_num])->pos_fb  = restore_joints[joint_num];
        }
        for (axis_num = 0; axis_num < EMCMOT_MAX_AXIS; axis_num++) {
            (&axes[axis_num])->teleop_tp.curr_pos = *pcmd_p[axis_num];
            (&axes[axis_num])->teleop_tp.pos_cmd  = *pcmd_p[axis_num];
        }

        dryrun_show(1,"RESTORE");
    } else {
        dryrun_show(1,"NO SAVED ????");
    }
} //dryrun_restore()

void dryrun_show(bool force,char*msg)
{
#ifdef DRYRUN_DBG //{
    char * mstate;
    mstate="UNKNOWN";

    // The 'force' parameter is available for local tests here:
    // Example:
    // if (   !force
    //     && !(testhere) {
    //    return;
    // }

    switch (emcmotStatus->motion_state) {
      case EMCMOT_MOTION_FREE:     mstate="FREE";    break;
      case EMCMOT_MOTION_TELEOP:   mstate="TELEOP";  break;
      case EMCMOT_MOTION_COORD:    mstate="COORD";   break;
      case EMCMOT_MOTION_DISABLED: mstate="DISABLED";break;
    }

    fprintf(stderr,"\n%1s %8d %-20s active=%d stopping=%d fini=%d mstate=%s\n"
           ,force?"F":"*"
           ,dryrun_ct,msg,dryrun_active(),dryrun_stopping,dryrun_fini
           ,mstate
           );
    fprintf(stderr,"simp: %8.4f %8.4f %8.4f cur:   %8.4f %8.4f %8.4f\n"
           ,(&axes[0])->teleop_tp.pos_cmd
           ,(&axes[1])->teleop_tp.pos_cmd
           ,(&axes[2])->teleop_tp.pos_cmd
           ,(&axes[0])->teleop_tp.curr_pos
           ,(&axes[1])->teleop_tp.curr_pos
           ,(&axes[2])->teleop_tp.curr_pos
           );
    fprintf(stderr,"abc:  %8.4f %8.4f %8.4f\n"
           ,emcmotStatus->carte_pos_cmd.a
           ,emcmotStatus->carte_pos_cmd.b
           ,emcmotStatus->carte_pos_cmd.c
           );
    fprintf(stderr,"Pose: %8.4f %8.4f %8.4f\n"
           ,emcmotStatus->carte_pos_cmd.tran.x
           ,emcmotStatus->carte_pos_cmd.tran.y
           ,emcmotStatus->carte_pos_cmd.tran.z
           );
    fprintf(stderr,"cmd:  %8.4f %8.4f %8.4f motor: %8.4f %8.4f %8.4f\n"
           ,(&joints[0])->pos_cmd
           ,(&joints[1])->pos_cmd
           ,(&joints[2])->pos_cmd
           ,(&joints[0])->motor_pos_cmd
           ,(&joints[1])->motor_pos_cmd
           ,(&joints[2])->motor_pos_cmd
           );
    fprintf(stderr,"fb:   %8.4f %8.4f %8.4f fb:    %8.4f %8.4f %8.4f\n"
           ,(&joints[0])->pos_fb
           ,(&joints[1])->pos_fb
           ,(&joints[2])->pos_fb
           ,(&joints[0])->motor_pos_fb
           ,(&joints[1])->motor_pos_fb
           ,(&joints[2])->motor_pos_fb
           );
#endif //}
    return;
} // dryrun_show()


void dryrun_process_inputs(void)
{
    int joint_num;

    for (joint_num = 0; joint_num < ALL_JOINTS; joint_num++) {
       (&joints[joint_num])->pos_fb = (&joints[joint_num])->pos_cmd; //shortcircuit
    }
} // dryrun_process_inputs

void dryrun_output_to_hal(void) {
    // required dryrun hal ouputs go here
    return;
} // dryrun_output_to_hal()

void dryrun_update_status(void)
{
    // required dryrun updates go here
    return;
} // dryrun_update_status()
