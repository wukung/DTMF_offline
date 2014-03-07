/*
 * THIS IS AN UNPUBLISHED WORK CONTAINING D2 TECHNOLOGIES, INC. CONFIDENTIAL AND
 * PROPRIETARY INFORMATION.  IF PUBLICATION OCCURS, THE FOLLOWING NOTICE
 * APPLIES: "COPYRIGHT 2006-2007 D2 TECHNOLOGIES, INC. ALL RIGHTS RESERVED"
 *
 * +D2Tech+ Rev: 174  Date: 2007-09-26 15:48:50 -0700 (Wed, 26 Sep 2007) 
 *
 *
 *
 *  ========== MHz on M4KC1 (5 ms processing) ==================
 *  7.23 MHz     Just after digit is detected
 *  5.00 MHz     Normal processing
 * 
 *   ========== MHz on A9201 (5 ms processing) ==================
 *  3.65 MHz     Worst case
 *
 * 
 */

#ifndef _DTMF
#define _DTMF

#include <comm.h>

typedef struct {
    vint     pDTMFPWR;     /* -25 to -45.   -45 == default */
    uvint    tDTMFDUR;     /* 5 ms units.     7 == default */
    uvint    tDTMFSIL;     /* 5 ms units.     5 == default */
    uvint    pDTMFHITWIST; /* 0, 1, 2, 3, 4.  2 == default */
    uvint    pDTMFLOTWIST; /* 0, 1, 2, 3, 4.  2 == default */
    uvint    fDTMFFDEV;    /* 0, 1, 2, 3.     1 == default */
    uvint    tDTMFEFRAMES; /* 4 to 127.       20 == default */
} DTMF_Params;

/* Defaults for local parameters */
#define DTMF_pDTMFPWR_DEF     (-45)/* Default -45 dBm per frequency */
#define DTMF_tDTMFDUR_DEF      (7) /* Default 7 blocks (35 ms) minimum on dur */
#define DTMF_tDTMFSIL_DEF      (5) /* Default 5 blocks (25 ms) minimum off dur*/
#define DTMF_pDTMFHITWIST_DEF  (2) /* Default 8 dB msximum high twist */
#define DTMF_pDTMFLOTWIST_DEF  (2) /* Default 8 dB maximum low twist */
#define DTMF_fDTMFFDEV_DEF     (1) /* Default 2.5/3.5 accept/reject freq dev */

/*
 * Default maximum number of bad frames allowed before geting tDTMFDUR
 * good frames
 */
#define DTMF_tDTMFEFRAMES_DEF  (20)

/*
 *  The following structure reserves room in the object for
 *  internal variables.
 *
 * Note, that configurables has a special bit field format, defined below
 *   3         2         1
 *  10987654321098765432109876543210
 *  ...............xxxxxxxx......... pDTMFEFRAMES
 *  ........................xxx..... pDTMFHITWIST
 *  ...........................xxx.. pDTMFLOTWIST
 *  ..............................xx pDTMFFDEV
 */
/*
 * Internal Object:
 */
typedef struct {
    char internal_ary[0x264];
} _DTMF_Internal;

typedef struct {
    _DTMF_Internal  internal;
    vint            lowRms;
    vint            early;
    vint            detected;
    vint            digit;
    vint           *src_ptr;
    vint           *dst_ptr;
    vint           *xmit_ptr;
} DTMF_Obj;

typedef DTMF_Obj *DTMF_Handle;

/*
 *  ======== LOCAL CONSTANTS
 */

#define _DTMF_ENGCOUNT  10
#define _DTMF_MAXTRIES  1
#define _DTMF_MAXFRAMES 3
/*
 *  The thresholds are determined by the following formula:
 *  Thresh(in Vrms) = pow(10, log(4004) - 1.25) / sqrt(2)
 *  The value calculated from this formula is 225.  Additional factors
 *  that deal with frame to frame energy variations and LPF effects
 *  yield a low rejection threshold of -25 dBm, Thresh(in Vrms) = 170
 */
#define _DTMF_THRESH_LOW    180
#define _DTMF_THRESH_HIGH   180
/*
 *  DEBOUNCE_INTERVAL is the number of blocks of interdigit silence used
 *  to debounce a digit.  At the end of this interval the DTMF trailing
 *  edge is signaled
*/
#define _DTMF_DEBOUNCE_INTERVAL 3

/*
 *  MIN_FRAMES is the number of blocks of tone that must be present before
 *  the leading edge of a DTMF digit is asserted.
 */
#define _DTMF_MIN_FRAMES    5

/*
 *  HIGH_RATIO and LOW_RATIO are the minimal acceptable ratio of inband
 *  to out of band energy.  The ratio is given by the following formula:
 *      RATIO = pow(10, ratio(in dB)/20) * 2048
 *  For a minimum ratio of 9dB, RATIO = 5772
 *                        16dB, RATIO = 12922
 *                        20dB, RATIO = 20480
*/
#define _DTMF_LOW_RATIO             10000
#define _DTMF_LOW_RATIO_1ST_FRAME   5144 /* applied when signal_frames <= 2 */
#define _DTMF_HIGH_RATIO_1ST_FRAME  8000 /* applied when signal_frames <= 2 */
#define _DTMF_HIGH_RATIO            10000
#define _DTMF_HIGH_RATIO_1633       7000       /* applied when pk_h == 13 */
#define _DTMF_ELOW_RATIO            5144
/*
 *  EFRAMES when added to MIN_FRAMES results in SIG_FRAMES which is the
 *  maximum number of frames after a 6dB jump energy before a detection
 *  leading edge is asserted.
*/
#define _DTMF_ERMS      6285
#define _DTMF_HNGOVER   3
#define _DTMF_BUFSIZE   161-4

/* Shift the input up by 2 if low_reject is less than _DTMF_SHIFT_2_THRESHOLD;
 * Shift the input up by 1 if low_reject is less than _DTMF_SHIFT_1_THRESHOLD.
 */
#define _DTMF_SHIFT_2_THRESHOLD    20
#define _DTMF_SHIFT_1_THRESHOLD    25

#define _DTMF_DIV_SQRT_40_Q16 10362
#define _DTMF_DIV_SQRT_20_Q16 14654
#define _DTMF_DIV_SQRT_10_Q16 20724

/*
 *  The prototypes for all DTMF related function appear here
 */
void DTMF_init(DTMF_Handle, GLOBAL_Params *, DTMF_Params *);
void DTMF_detect(DTMF_Handle);
void DTMF_remove(DTMF_Handle);
#endif
