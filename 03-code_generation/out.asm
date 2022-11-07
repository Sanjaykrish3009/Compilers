.data
# variables
temp: .double 0.000000
HCl.num1: .word 1
HCl.wt1: .double 1.000000
HCl.num2: .word 1
HCl.wt2: .double 35.500000
H.no: .word 1
H.wt: .double 1.000000
H.charge: .double 1.000000
i: .word 3
s: .double 1.500000
x: .word 2
y: .word 0
z: .double 0.000000
NaCl.num1: .word 1
NaCl.wt1: .double 23.500000
NaCl.num2: .word 1
NaCl.wt2: .double 35.500000
Cl.no: .word 17
Cl.wt: .double 35.500000
Cl.charge: .double -1.000000
Na.no: .word 11
Na.wt: .double 23.500000
Na.charge: .double 1.000000
# messages
msg1: .asciiz "Molar mass of NaCl = "
msg2: .asciiz "\n"
msg3: .asciiz "\n"
msg4: .asciiz "Value \nof z:"

.text
main:
L.D $f12, Cl.wt($0)
MOV.D $f6, $f12
S.D $f6, z($0)
L.D $f22,temp($0)
LW $t0, NaCl.num1($0)
L.D $f16, NaCl.wt1($0)
MTC1.D $t0, $f30
CVT.D.W $f30, $f30
MUL.D $f30,$f30,$f16
ADD.D $f22,$f22,$f30
LW $t0, NaCl.num2($0)
L.D $f16, NaCl.wt2($0)
MTC1.D $t0, $f30
CVT.D.W $f30, $f30
MUL.D $f30,$f30,$f16
ADD.D $f22,$f22,$f30
MOV.D $f8, $f22
S.D $f8, s($0)
LI $v0, 4
LA $a0, msg1
SYSCALL
L.D $f8, s($0)
LI $v0, 3
MOV.D $f12, $f8
SYSCALL
LI $v0, 4
LA $a0, msg2
LA $a0, msg3
SYSCALL
L.D $f22,temp($0)
LW $t1, HCl.num1($0)
L.D $f18, HCl.wt1($0)
MTC1.D $t1, $f30
CVT.D.W $f30, $f30
MUL.D $f30,$f30,$f18
ADD.D $f22,$f22,$f30
LW $t1, HCl.num2($0)
L.D $f18, HCl.wt2($0)
MTC1.D $t1, $f30
CVT.D.W $f30, $f30
MUL.D $f30,$f30,$f18
ADD.D $f22,$f22,$f30
MOV.D $f8, $f22
S.D $f8, s($0)
L.D $f8, s($0)
LI $v0, 3
MOV.D $f12, $f8
SYSCALL
LI $v0, 4
LA $a0, msg2
LA $a0, msg3
SYSCALL
LI $v0, 4
LA $a0, msg4
SYSCALL
L.D $f6, z($0)
LI $v0, 3
MOV.D $f12, $f6
SYSCALL
