.macro nonmatching label, size
.endm
.macro enddlabel label
.endm
.macro glabel label
    .global \label
    .type \label, @function
    \label:
.endm
.macro dlabel label
    .global \label
    \label:
.endm
