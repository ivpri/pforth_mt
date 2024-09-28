\ extracted from misc2.fth MS should be defined by platforms

decimal
create MSEC-DELAY 100000 ,   \ calibrate this for your system
: (MSEC.SPIN) ( #msecs -- , busy wait, not accurate )
    0 max   \ avoid endless loop
    0
    ?do  msec-delay @ 0
        do loop
    loop
;

: (MSEC) ( millis -- )
    dup (sleep) \ call system sleep in kernel
    IF
        ." (SLEEP) failed or not implemented! Using (MSEC.SPIN)" CR
        (msec.spin)
    ELSE
        drop
    THEN
;

defer msec

\ (SLEEP) uses system sleep functions to actually sleep.
\ Use (MSEC.SPIN) on embedded systems that do not support Win32 Sleep() posix usleep().
1 (SLEEP) [IF]
    ." (SLEEP) failed or not implemented! Use (MSEC.SPIN) for MSEC" CR
    ' (msec.spin) is msec
[ELSE]
    ' (msec) is msec
[THEN]

: MS ( msec -- , sleep, ANS standard )
    msec
;
