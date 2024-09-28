\ To distinguisg words producing a flags for TASK.NOTIFY-ISR or TASK.NOTIFY
\ the shortened name TASK.NOTF is used

DECIMAL

\ Action for TASK.NOTIFY / TASK.NOTIFY-ISR
0 CONSTANT TASK.NOTF-NA   \ No action on value - just notify target task.
1 CONSTANT TASK.NOTF-INC  \ Also increment notification value.
2 CONSTANT TASK.NOTF-SET  \ Also set notification value unless another notification is pending.
3 CONSTANT TASK.NOTF-SET! \ Also set notification value regardless another notification is pending.
4 CONSTANT TASK.NOTF-SETB \ Also set notification value bits.
: TASK.NOTF-QUERY 8 OR ;  \ Modify task notify config of TASK.NOTIFY that it will return additional
                          \ value of previous notification value.

\ "Macros" to simulate light-weight semaphores
: TASK.NOTIFY-GIVE     ( tt -- ) TASK.NOTF-INC SWAP TASK.NOTIFY     ;
: TASK.NOTIFY-GIVE-ISR ( tt -- ) TASK.NOTF-INC SWAP TASK.NOTIFY-ISR ;
