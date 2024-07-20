#ifndef TZXDESCRIPTION_H
#define TZXDESCRIPTION_H

#define TZX_HEADER_LENGTH   10

// TZX block list
#define TZX_ID10    0x10    // Standard speed data block
#define TZX_ID11    0x11    // Turbo speed data block
#define TZX_ID12    0x12    // Pure tone
#define TZX_ID13    0x13    // Sequence of pulses of various lengths
#define TZX_ID14    0x14    // Pure data block
#define TZX_ID15    0x15    // Direct recording block
//#define TZX_ID18  0x18    // CSW recording block
#define TZX_ID19    0x19    // Generalized data block
#define TZX_ID20    0x20    // Pause (silence) ot 'Stop the tape' command
#define TZX_ID21    0x21    // Group start
#define TZX_ID22    0x22    // Group end
#define TZX_ID23    0x23    // Jump to block
#define TZX_ID24    0x24    // Loop start
#define TZX_ID25    0x25    // Loop end
#define TZX_ID26    0x26    // Call sequence
#define TZX_ID27    0x27    // Return from sequence
#define TZX_ID28    0x28    // Select block
#define TZX_ID2A    0x2A    // Stop the tape is in 48K mode
#define TZX_ID2B    0x2B    // Set signal level
#define TZX_ID30    0x30    // Text description
#define TZX_ID31    0x31    // Message block
#define TZX_ID32    0x32    // Archive info
#define TZX_ID33    0x33    // Hardware type
#define TZX_ID35    0x35    // Custom info block
#define TZX_ID4B    0x4B    // Kansas City block (MSX/BBC/Acorn/...)
#define TZX_ID5A    0x5A    // Glue block (90 dec, ASCII Letter 'Z')
#define TZX_EOF     0xFF    // End of file

enum TZXFileStage {
    GET_FILE_HEADER,
    GET_ID,
    PROCESS_ID,
    GET_AY_HEADER,
    GET_UEF_HEADER,
    GET_CHUNK_ID,
    PROCESS_CHUNK_ID
};

enum TZXHandlerStage {
    READ_PARAM,
    PILOT,
    SYNC1,
    SYNC2,
    DATA,
    HEADER,
    NAME,
    GAP,
    SYNC_LAST,
    NAME_LAST,
    PAUSE
};

#endif
