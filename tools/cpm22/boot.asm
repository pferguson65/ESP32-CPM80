;    CP/M Bootloader for ESP32 8080
;
        ORG     2000H
;
;   Loads CPM from the 'disk' from
;   track 0 sector 1 to track 1 sector 25.
;
        LXI     SP,1000H
;
;       CALL    PRINTIPL

; Here we want to copy the CP/M system from disk to memory.
; CP/M occupies the first two tracks of the disk minus the
; first sector which contains the bootloader.
;
        MVI     B,51                ; load 51 sectors (2 tracks * 26 sectors - this sector)
        LXI     D,0001H             ; start with track 0 sector 1
        LXI     H,3400H + 0A800H    ; destination is start of CCP+b
LOAD:
        MOV     A,D                 ; track
        OUT     16
        MOV     A,E                 ; sector
        OUT     18
        MOV     A,L                 ; dma L
        OUT     20
        MOV     A,H                 ; dma H
        OUT     21
        MVI     A,1                 ; Read sector to RAM
        OUT     22

        PUSH    B
        LXI     B,80H               ; increment RAM pointer to next block
        DAD     B
        POP     B

        INR     E                   ; increment sector
        MOV     A,E
        CPI     26
        JNZ     LOOP1

        INR     D                   ; increment track
        MVI     E,0                 ; reset sector counter to 0

LOOP1:
        DCR     B
        JNZ     LOAD

        JMP     4A00H + 0A800H      ; jump to BIOS

PRINTIPL:
        MVI     A,'i'
        OUT     2
        MVI     A,'p'
        OUT     2
        MVI     A,'l'
        OUT     2
        MVI     A,13
        OUT     2
        MVI     A,10
        OUT     2
        ret

        end
