\ @(#) t_multitask.fth 2019/08/19 1.0
\ Test pforth multitasking extensions
\
\ Copyright 2019 Ivan Priesol

0 CONSTANT VERBOSE

\ Compatibility with e4thcom
: #INCLUDE INCLUDE ;
#INCLUDE  t_tools.fth

\ -> is already used by pforth locals so use --> here
: --> }T{ ;

: testing
    verbose IF
	source >in @ /string ." TESTING: " type cr
    THEN
    source nip >in !
; immediate


DECIMAL

TEST{

testing ms

.( 0 1 2 3 4 5 6 7 8 9 should appear with 100 ms delay: ) cr
10 0 do i . 100 ms loop cr


testing task

variable v 11 v !

: x 10 ms 22 v +! ;

t{ 0 ' x task swap value t1 --> 0 }t
t{ t1 task.done?  5 ms --> false }t
t{ t1 task.done? 10 ms --> false }t
t{ t1 task.done?  5 ms --> true  }t
t{ t1 task.done?       --> true  }t
t{ 0 t1 task.join v @  --> 0 33  }t

t{ 0 ' abort task swap to t1 --> 0 }t
t{ 1 ms t1 task.done?  --> true  }t
t{ 0 t1 task.join      --> -1    }t

: z -44 1000 ms throw ;

t{ 0 ' z task 500 ms swap to t1 --> 0 }t
t{ t1 task.done?  5 ms --> false }t
t{ t1 task.done?  5 ms --> false }t
t{ 0 t1 task.join --> -44 }t

t{ 10 20 2 ' + task swap to t1 --> 0 }t
t{ 1 t1 task.join --> 30 0 }t


testing TASK.DETACH TASK.CANCEL

: d 500 ms ;

t{ 0 ' d task 100 ms swap constant td --> 0 }t
t{ td task.done? --> false }t
t{ td task.detach --> }t

variable c 0 c !

: ct begin 1 c +! c @ 0= until ;

t{ 0 ' ct task swap constant ctt --> 0 }t

t{ 0 ' x task swap task.detach 1 ms --> 0 }t
t{ 0 ' x task swap task.detach 1 ms --> 0 }t
t{ 0 ' x task swap task.detach 1 ms --> 0 }t
t{ 0 ' x task swap task.detach 1 ms --> 0 }t
t{ 0 ' x task swap task.detach 1 ms --> 0 }t
t{ 0 ' x task swap task.detach 1 ms --> 0 }t

t{ v @ --> 33 }t
t{ c @ 50000 > --> true }t
t{ 6 ms v @ 5 ms v @ --> 99 165 }t

t{ c @ ctt task.cancel c @ < --> true }t
t{ c @ 5 ms c @ = --> true }t


TESTING SEMP...

t{ semp.mutex swap value mx1 --> 0 }t
t{ semp.mutex swap value mx2 --> 0 }t

0 v !
: mxt ms -1 mx1 semp.take drop
      v @ swap ms + v ! mx1 semp.give ;

t{ -1 mx1 semp.take --> true }t
t{ 0 mx1 semp.take --> false }t
t{ mx1 semp.count@ --> 0 }t
t{ mx1 semp.give --> }t
t{ mx1 semp.count@ --> 1 }t

t{ 111 5 2 3 ' mxt task swap task.detach --> 0 }t
t{ mx1 semp.count@ --> 1 }t
t{ 3 ms mx1 semp.count@ v @ --> 0 0 }t
t{ 6 ms mx1 semp.count@ v @ --> 1 111 }t

t{ -1 mx1 semp.take --> true }t

t{ 222 3 0 3 ' mxt task swap task.detach --> 0 }t
t{ 333 0 1 3 ' mxt task swap task.detach --> 0 }t

t{ 5 ms mx1 semp.count@ v @ --> 0 111 }t
t{ mx1 semp.give --> }t
t{ 5 ms mx1 semp.count@ v @ --> 1 666 }t
t{ mx1 semp.delete --> }t

t{ semp.mutex-r swap to mx1 --> 0 }t
t{ mx1 semp.count@ --> 1 }t
t{ 0 mx1 semp.take --> true }t
t{ mx1 semp.count@ --> 0 }t
t{ 0 mx1 semp.take --> true }t
t{ mx1 semp.count@ --> -1 }t
t{ mx1 semp.give mx1 semp.give --> }t
t{ mx1 semp.count@ --> 1 }t
t{ mx1 semp.delete --> }t

t{ 5 2 semp swap value sem1 --> 0 }t
t{ sem1 semp.count@ --> 2 }t
t{ 0 sem1 semp.take --> true }t
t{ 0 sem1 semp.take --> true }t
t{ 0 sem1 semp.take --> false }t
t{ sem1 semp.give sem1 semp.count@ --> 1 }t
t{ 8 0 do sem1 semp.give loop sem1 semp.count@ .s --> 5 }t


TESTING TASK.NOTIFY...

#INCLUDE task_notify.fth

: v+ -1 mx2 semp.take drop v +! mx1 semp.give ;

0 v !
: ntt ms task.notify-take v+ ;
-1 5 2 ' ntt task throw to t1

t{ 1 ms v @ 10 ms v @ --> 0 0  }t
t{ 44 task.notf-set t1 task.notify --> true }t
t{ 0 t1 task.join v @ --> 0 44 }t


.( Note few tests might fail sometimes due to tasks context switch delays) cr

}TEST

