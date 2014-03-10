/*
 * DTMF offline test
 * Target: Perform offline test with PCM DUMP by libdtmf.a
 */
#include <stdio.h>
#include <stdlib.h>
#include "d2types.h"
#include "comm.h"
#include "dtmf.h"

const char AudioFile[] = "0306_1600_10026.raw";
//const char AudioFile[] = "0303_1539_10020.raw";
const int bufLength = 80;

DTMF_Params dtmfParams = {
    -24,
    5,
    4,
    DTMF_pDTMFHITWIST_DEF,
    DTMF_pDTMFLOTWIST_DEF,
    DTMF_fDTMFFDEV_DEF,
    DTMF_tDTMFEFRAMES_DEF
};

typedef struct {
    DTMF_Obj     dtmfObj;
    int          dtmfLe;
    int          dtmfTe;
    int          dtmfDigit;
    unsigned int dtmfRemove;
    unsigned int earlyCount;
} _VTSPR_DtmfObj;

GLOBAL_Params globals = {
    4004,
    4004
};

int main(void)
{
    _VTSPR_DtmfObj  *dtmf_ptr;
    DTMF_Obj        *dtmfObj_ptr;
    FILE  *fin;
    short tmp[bufLength];
    int   sin[bufLength];
    int   sout[bufLength];
    int   detect;
    int   detect1;
    int   detect2;
    int   digit1;
    int   digit2;
    int   lastDigit;
    int   le;
    int   te;
    int   readCnt;

    // Open Audio input file
    fin = fopen(AudioFile, "rb");
    if (NULL == fin) {
        fprintf(stderr, "Fail to open audio input file.\n");
        return -1;
    }

    // Allocate memory for DTMF object
    dtmf_ptr = (_VTSPR_DtmfObj *) calloc (1, sizeof(_VTSPR_DtmfObj));
    if (NULL == dtmf_ptr) {
        fprintf(stderr, "Fail to allocate memory for DTMF object\n");
        fclose(fin);
        return -1;
    }

    // Set initial value of DTMF object
    dtmf_ptr->dtmfDigit = -1;
    dtmf_ptr->dtmfLe = -1;
    dtmf_ptr->dtmfTe = -1;
    dtmf_ptr->dtmfRemove = 0;
    dtmf_ptr->earlyCount = 0;
    dtmfObj_ptr = &dtmf_ptr->dtmfObj;

    // Initial DTMF detector
    DTMF_init(dtmfObj_ptr, &globals, &dtmfParams);
    readCnt = 0;
    while (bufLength == fread((void *)tmp, sizeof(short), bufLength, fin)) {
        for (int i=0; i<bufLength; i++) {
            sin[i] = tmp[i];
        }
        // Process fist 5 ms
        dtmfObj_ptr->src_ptr = sin;
        dtmfObj_ptr->dst_ptr = sout;
        DTMF_detect(dtmfObj_ptr);
        digit1 = dtmfObj_ptr->digit;
        detect1 = dtmfObj_ptr->detected;

        // Process next 5 ms
        dtmfObj_ptr->src_ptr = &sin[40];
        dtmfObj_ptr->dst_ptr = &sout[40];
        DTMF_detect(dtmfObj_ptr);
        digit2 = dtmfObj_ptr->digit;
        detect2 = dtmfObj_ptr->detected;

        /*
         * Check DTMF detection.
         */
        lastDigit = dtmf_ptr->dtmfDigit;
        le = -1;
        te = -1;
        detect = (detect1 | detect2);

        if (detect) {
            // Digit is detected, check if it is a leading edge
            if (-1 == lastDigit) {
                if (detect1) {
                    le = digit1;
                    lastDigit = digit1;
                }
                else {
                    le = digit2;
                    lastDigit = digit2;
                }
                printf("Leading: Digit = 0x%x at block %d\n",
                       lastDigit, readCnt);
            }
        }
        else {
            // Digit is not detected, check if it is a tailing edge
            if (lastDigit > -1) {
                te = lastDigit;
                printf("Tailing: Digit = 0x%x at block %d\n",
                       lastDigit, readCnt);
            }
            lastDigit = -1;
        }
        // Update detection result
        dtmf_ptr->dtmfDigit = lastDigit;
        dtmf_ptr->dtmfLe = le;
        dtmf_ptr->dtmfTe = te;
        readCnt++;
    }

    free(dtmf_ptr);
    fclose(fin);
    printf("Done with block = %d!\n", readCnt);
    return 0;
}

