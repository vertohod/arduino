#include "TZXFile.h"

#include "Types.h"

TZXFile::TZXFile(File& file) : mFile(file) {

}

bool TZXFile::readHeader() {
    byte buffer[TZX_HEADER_LENGTH];
    mFile.read(&buffer[0], TZX_HEADER_LENGTH);

    return false;
}
