PUSH 10 ;x0
PUSH 15 ;y0
PUSH 5 ;r
CALL draw_circle:
PRNT
HLT

;EX x
;FX y
;GX x
;HX y

draw_circle:
    POP CX
    POP BX
    POP AX

    ;check if y0-r < 0
    PUSH 0
    PUSH CX
    PUSH BX
    SUB
    POP FX
    PUSH FX
    JAE y_above_0:
        PUSH 0
        POP FX
    y_above_0:

    ;check if y0+r > 0
    PUSH 32
    PUSH CX
    PUSH BX
    ADD
    POP HX
    PUSH HX
    JB y_below_32:
        PUSH 31
        POP HX
    y_below_32:

    ;check if x0+r > 31
    PUSH 32
    PUSH CX
    PUSH AX
    ADD
    POP GX
    PUSH GX
    JB x_below_32:
        PUSH 31
        POP GX
    x_below_32:

    loop_y:
        ;check if x0-r < 0
        PUSH 0
        PUSH CX
        PUSH AX
        SUB
        POP EX
        PUSH EX
        JAE x_above_0:
            PUSH 0
            POP EX
        x_above_0:

        loop_x:
            PUSH 0
            PUSH CX
            PUSH CX
            MUL
            PUSH AX
            PUSH EX
            SUB
            PUSH AX
            PUSH EX
            SUB
            MUL
            PUSH BX
            PUSH FX
            SUB
            PUSH BX
            PUSH FX
            SUB
            MUL
            ADD
            SUB

            ;if pixel in circle draw pixel
            JA not_print_pixel:
                PUSH FX
                PUSH 32
                MUL
                PUSH EX
                ADD
                POP DX
                PUSH 1
                POP [DX]
                PUSH DX
            not_print_pixel:

            PUSH GX
            PUSH EX
            PUSH 1
            ADD
            POP EX
            PUSH EX
            JBE loop_x:

        PUSH HX
        PUSH FX
        PUSH 1
        ADD
        POP FX
        PUSH FX
        JBE loop_y:

    RET




