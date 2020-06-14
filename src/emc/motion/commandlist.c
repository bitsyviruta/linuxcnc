ABORT = 1,	/* abort all motion */
AF_ENABLE,	/* enable/disable adaptive feedrate */
CLEAR_PROBE_FLAGS,	/* clears probeTripped flag */
COORD,		/* set mode to coordinated motion */
DISABLE_WATCHDOG,        /* enable watchdog sound, parport . En motion-logger.c*/
DISABLE,		/* disable servos for active joints */
ENABLE_WATCHDOG,         /* enable watchdog sound, parport */
ENABLE,		/* enable servos for active joints */
FEED_SCALE,	/* set scale factor for feedrate */
FH_ENABLE,	/* enable/disable feed_hold */
FORWARD,		/* run forward motion */
FREE,		/* set mode to free (joint) motion */
FS_ENABLE,	/* enable/disable scaling feedrate */
JOG_ABS,		/* absolute jog */
JOG_CONT,	/* continuous jog */
JOG_INCR,	/* incremental jog */
JOINT_ABORT,             /* abort one joint */
JOINT_ACTIVATE,          /* make joint active */
JOINT_DEACTIVATE,        /* make joint inactive */
JOINT_DISABLE_AMPLIFIER, /* disable amp outputs */
JOINT_ENABLE_AMPLIFIER,  /* enable amp outputs */
JOINT_HOME,              /* home a joint or all joints */
JOINT_UNHOME,            /* unhome a joint or all joints*/
OVERRIDE_LIMITS,	/* temporarily ignore limits until jog done */
PAUSE,		/* pause motion */
PROBE,		/* go to pos, stop if probe trips, record trip pos */
RAPID_SCALE,	/* set scale factor for rapids */
RESUME,		/* resume motion */
REVERSE,		/* run reverse motion */
RIGID_TAP,	/* go to pos, with sync to spindle speed,  then return to initial pos */
SET_ACC,		/* set the max accel for moves (tooltip) */
SET_AOUT,	/* sets or unsets a AIO, this can be imediate or synched with motion */
SET_AXIS_ACC_LIMIT,      /* set the max axis acc */
SET_AXIS_LOCKING_JOINT,  /* set the axis locking joint */
SET_AXIS_POSITION_LIMITS, /* set the axis position +/- limits */
SET_AXIS_VEL_LIMIT,      /* set the max axis vel */
SET_CIRCLE,	/* queue up a circular move */
SET_DEBUG,       /* sets the debug level */
SET_DOUT,        /* sets or unsets a DIO, this can be imediate or synched with motion */
SET_JOINT_ACC_LIMIT,     /* set the max joint accel */
SET_JOINT_BACKLASH,      /* set the joint backlash */
SET_JOINT_COMP,          /* set a compensation triplet for a joint (nominal, forw., rev.) */
SET_JOINT_HOMING_PARAMS, /* sets joint homing parameters */
SET_JOINT_MAX_FERROR,    /* maximum following error, input units */
SET_JOINT_MIN_FERROR,    /* minimum following error, input units */
SET_JOINT_MOTOR_OFFSET,  /* set the offset between joint and motor */
SET_JOINT_POSITION_LIMITS, /* set the joint position +/- limits */
SET_JOINT_VEL_LIMIT,     /* set the max joint vel */
SET_LINE,	/* queue up a linear move */
SET_MAX_FEED_OVERRIDE,
SET_NUM_JOINTS,	/* set the number of joints */
SET_NUM_SPINDLES, /* set the number of spindles */
SET_OFFSET, /* set tool offsets */
SET_PROBE_ERR_INHIBIT,
SET_SPINDLESYNC, /* syncronize motion to spindle encoder */
SET_TELEOP_VECTOR,	/* Move at a given velocity but in WCC, not in joint space like JOG_* */
SET_TERM_COND,	/* set termination condition (stop, blend) */
SET_VEL_LIMIT,	/* set the max vel for all moves (tooltip) */
SET_VEL,		/* set the velocity for subsequent moves */
SET_WORLD_HOME,	/* set pose for world home */
SETUP_ARC_BLENDS,
SPINDLE_BRAKE_ENGAGE,	/* engage the spindle brake */
SPINDLE_BRAKE_RELEASE,	/* release the spindle brake */
SPINDLE_DECREASE,	/* spindle slower */
SPINDLE_INCREASE,	/* spindle faster */
SPINDLE_OFF,	/* stop the spindle */
SPINDLE_ON,	/* start the spindle */
SPINDLE_ORIENT,          /* orient the spindle */
SPINDLE_SCALE,	/* set scale factor for spindle speed */
SS_ENABLE,	/* enable/disable scaling the spindle speed */
STEP,		/* resume motion until id encountered */
TELEOP,		/* set mode to teleop */
UPDATE_JOINT_HOMING_PARAMS, /* updates some joint homing parameters */


	switch (emcmotCommand->command) {
	case EMCMOT_ABORT:
	    /* abort motion */
	    /* can happen at any time */
	    /* this command attempts to stop all machine motion. it looks at
	       the current mode and acts accordingly, if in teleop mode, it
	       sets the desired velocities to zero, if in coordinated mode,
	       it calls the traj planner abort function (don't know what that
	       does yet), and if in free mode, it disables the free mode traj
	       planners which stops joint motion */
	case EMCMOT_JOINT_ABORT:
	    /* abort one joint */
	    /* can happen at any time */
	case EMCMOT_FREE:
	    /* change the mode to free mode motion (joint mode) */
	    /* can be done at any time */
	    /* this code doesn't actually make the transition, it merely
	       requests the transition by clearing a couple of flags */
	    /* reset the emcmotDebug->coordinating flag to defer transition
	       to controller cycle */
	case EMCMOT_COORD:
	    /* change the mode to coordinated axis motion */
	    /* can be done at any time */
	    /* this code doesn't actually make the transition, it merely
	       tests a condition and then sets a flag requesting the
	       transition */
	    /* set the emcmotDebug->coordinating flag to defer transition to
	       controller cycle */
	case EMCMOT_TELEOP:
        /* call switch_to_teleop_mode() */
	case EMCMOT_SET_NUM_JOINTS:
	    /* set the global NUM_JOINTS, which must be between 1 and
	       EMCMOT_MAX_JOINTS, inclusive.
	       Called  by task using [KINS]JOINTS= which is typically
	       the same value as the motmod num_joints= parameter */
	case EMCMOT_SET_NUM_SPINDLES:
	    /* set the global NUM_SPINDLES, which must be between 1 and
	       EMCMOT_MAX_SPINDLES, inclusive and less than or equal to
	       the number of spindles configured for the motion module
	       (motion_num_spindles) */
	case EMCMOT_SET_WORLD_HOME:
		/* set pose for world home. 
	       emcmotStatus->world_home = emcmotCommand->pos;*/
	case EMCMOT_SET_JOINT_HOMING_PARAMS:
		/* sets joint homing parameters. 
	       call set_joint_homing_params() (homing.c)*/
	case EMCMOT_UPDATE_JOINT_HOMING_PARAMS:
        /* updates some joint homing parameters. 
	       call update_joint_homing_params() (homing.c)*/
	case EMCMOT_OVERRIDE_LIMITS:
	    /* this command can be issued with joint < 0 to re-enable
	       limits, but they are automatically re-enabled at the
	       end of the next jog */
	case EMCMOT_SET_JOINT_MOTOR_OFFSET:
		/* set the offset between joint and motor.
	       joint->motor_offset = emcmotCommand->motor_offset */
	case EMCMOT_SET_JOINT_POSITION_LIMITS:
	    /* set the position limits for the joint.
	       can be done at any time.
	       joint->min_pos_limit = emcmotCommand->minLimit
	       joint->max_pos_limit = emcmotCommand->maxLimit */
	case EMCMOT_SET_JOINT_BACKLASH:
	    /* set the backlash for the joint.
	       can be done at any time 
	       joint->backlash = emcmotCommand->backlash;*/


	    /*
	       Max and min ferror work like this: limiting ferror is
	       determined by slope of ferror line, = maxFerror/limitVel ->
	       limiting ferror = maxFerror/limitVel * vel. 
           If ferror < minFerror then 
				OK 
		   else 
				if ferror < limiting ferror then 
					OK
	       		else ERROR */
	case EMCMOT_SET_JOINT_MAX_FERROR:
	    /* joint->max_ferror = emcmotCommand->maxFerror */
	case EMCMOT_SET_JOINT_MIN_FERROR:
	    /* joint->min_ferror = emcmotCommand->minFerror*/

	case EMCMOT_JOG_CONT:
	    /* do a continuous jog, implemented as an incremental jog to the
	       limit.  When the user lets go of the button an abort will
	       stop the jog. */

	case EMCMOT_JOG_INCR:
	    /* do an incremental jog */
	        /* set target position */
	        /* set velocity of jog */
	        /* use max joint accel */
	        /* lock out other jog sources */
	        /* and let it go */
	case EMCMOT_JOG_ABS:
	    /* do an absolute jog */
	case EMCMOT_SET_TERM_COND:
	    /* sets termination condition for motion emcmotDebug->coord_tp (stop, blend)*/
	case EMCMOT_SET_SPINDLESYNC:
		/* syncronize motion to spindle encoder */
	case EMCMOT_SET_LINE:
		/* queue up a linear move */
	    /* emcmotDebug->coord_tp up a linear move */
	    /* requires motion enabled, coordinated mode, not on limits */
	case EMCMOT_SET_CIRCLE:
		/* queue up a circular move */
	    /* emcmotDebug->coord_tp up a circular move */
	    /* requires coordinated mode, enable on, not on limits */
	case EMCMOT_SET_VEL:
	    /* set the velocity for subsequent moves */
	    /* can do it at any time */
	case EMCMOT_SET_VEL_LIMIT:
	    /* set the absolute max velocity for all subsequent moves */
	    /* can do it at any time */
	case EMCMOT_SET_JOINT_VEL_LIMIT:
	    /* set joint max velocity */
	    /* can do it at any time */
	case EMCMOT_SET_JOINT_ACC_LIMIT:
	    /* set joint max acceleration */
	    /* can do it at any time */
	case EMCMOT_SET_ACC:
	    /* set the max acceleration */
	    /* can do it at any time */
	case EMCMOT_PAUSE:
	    /* pause the motion */
	    /* can happen at any time */
	case EMCMOT_REVERSE:
	    /* run motion in reverse*/
	    /* only allowed during a pause */
	case EMCMOT_FORWARD:
	    /* run motion in forward*/
	    /* only allowed during a pause */
	case EMCMOT_RESUME:
	    /* resume paused motion */
	    /* can happen at any time */
	case EMCMOT_STEP:
	    /* resume paused motion until id changes */
	    /* can happen at any time */
	case EMCMOT_FEED_SCALE:
	    /* override speed */
	    /* can happen at any time */
	case EMCMOT_RAPID_SCALE:
	    /* override rapids */
	    /* can happen at any time */
	case EMCMOT_FS_ENABLE:
	    /* enable/disable overriding speed */
	    /* can happen at any time */
	case EMCMOT_FH_ENABLE:
	    /* enable/disable feed hold */
	    /* can happen at any time */
	case EMCMOT_SPINDLE_SCALE:
	    /* override spindle speed */
	    /* can happen at any time */
	case EMCMOT_SS_ENABLE:
	    /* enable/disable overriding spindle speed */
	    /* can happen at any time */
	case EMCMOT_AF_ENABLE:
	    /* enable/disable adaptive feedrate override from HAL pin */
	    /* can happen at any time */
	case EMCMOT_DISABLE:
	    /* go into disable */
	    /* can happen at any time */
	    /* reset the emcmotDebug->enabling flag to defer disable until
	       controller cycle (it *will* be honored) */
	case EMCMOT_ENABLE:
	    /* come out of disable */
	    /* can happen at any time */
	    /* set the emcmotDebug->enabling flag to defer enable until
	       controller cycle */
	case EMCMOT_JOINT_ACTIVATE:
	    /* make joint active, so that amps will be enabled when system is
	       enabled or disabled */
	    /* can be done at any time */
	case EMCMOT_JOINT_DEACTIVATE:
	    /* make joint inactive, so that amps won't be affected when system
	       is enabled or disabled */
	    /* can be done at any time */
	case EMCMOT_JOINT_ENABLE_AMPLIFIER:
	    /* enable the amplifier directly, but don't enable calculations */
	    /* can be done at any time */
	case EMCMOT_JOINT_DISABLE_AMPLIFIER:
	    /* disable the joint calculations and amplifier, but don't disable
	       calculations */
	    /* can be done at any time */
	case EMCMOT_JOINT_HOME:
	    /* home the specified joint */
	    /* need to be in free mode, enable on */
	    /* this just sets the initial state, then the state machine in
	       homing.c does the rest */
	case EMCMOT_JOINT_UNHOME:
        /* unhome the specified joint, or all joints if -1 */
	case EMCMOT_CLEAR_PROBE_FLAGS:
		/* clears emcmotStatus probe flag */
	case EMCMOT_PROBE:
		/* go to pos, stop if probe trips, record trip pos */
	    /* most of this is taken from EMCMOT_SET_LINE */
	    /* emcmotDebug->coord_tp up a linear move */
	    /* requires coordinated mode, enable off, not on limits */
	case EMCMOT_RIGID_TAP:
		/* go to pos, with sync to spindle speed,  then return to initial pos */
	    /* most of this is taken from EMCMOT_SET_LINE */
	    /* emcmotDebug->coord_tp up a linear move */
	    /* requires coordinated mode, enable off, not on limits */
	case EMCMOT_SET_DEBUG:
		/* sets the debug level */
	case EMCMOT_SET_AOUT:
		/* sets or unsets a AIO, this can be inmediate or synched with motion */
	case EMCMOT_SET_DOUT:
		/* sets or unsets a DIO, this can be imediate or synched with motion */
	case EMCMOT_SPINDLE_ON:
		/* start the spindle */
	case EMCMOT_SPINDLE_OFF:
		/* stop the spindle */
	case EMCMOT_SPINDLE_ORIENT:
		/* orient the spindle 
			- el husillo debe existir (spindle_num =< emcmotConfig->numSpindles)
			- detecta una operacion de orientacion en curso
			- so far like spindle stop, except opening brake
			- mirror in spindle status */
	case EMCMOT_SPINDLE_INCREASE:
		/* spindle faster
			- el husillo debe existir (spindle_num =< emcmotConfig->numSpindles)
			- el incremento de velocidad se produce en pasos fijos de 100
			- FIXME - make the step a HAL parameter */
	case EMCMOT_SPINDLE_DECREASE:
		/* spindle slower
			- el husillo debe existir (spindle_num =< emcmotConfig->numSpindles)
			- el incremento de velocidad se produce en pasos fijos de 100
			- FIXME - make the step a HAL parameter */
	case EMCMOT_SPINDLE_BRAKE_ENGAGE:
		/* engage the spindle brake 
			- el husillo debe existir (spindle_num =< emcmotConfig->numSpindles) */
	case EMCMOT_SPINDLE_BRAKE_RELEASE:
		/* release the spindle brake 
			- el husillo debe existir (spindle_num =< emcmotConfig->numSpindles) */
	case EMCMOT_SET_JOINT_COMP:
		/* set a compensation triplet for a joint (nominal, forw., rev.) */
    case EMCMOT_SET_OFFSET:
		/* set tool offsets */
	case EMCMOT_SET_AXIS_POSITION_LIMITS:
	    /* set the position limits for axis */
	    /* can be done at any time */
    case EMCMOT_SET_AXIS_VEL_LIMIT:
	    /* set the max axis vel */
	    /* can be done at any time */
    case EMCMOT_SET_AXIS_ACC_LIMIT:
 	    /* set the max axis acc */
	    /* can be done at any time */
    case EMCMOT_SET_AXIS_LOCKING_JOINT:
		/* set the axis locking joint */
    case EMCMOT_SET_MAX_FEED_OVERRIDE:
    case EMCMOT_SETUP_ARC_BLENDS:
    case EMCMOT_SET_PROBE_ERR_INHIBIT:

