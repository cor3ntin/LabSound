#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <opusfile.h>
#include <QIODevice>

namespace lab {

namespace detail {

size_t readOggVorbis(void* dst, size_t size1, size_t size2, void* fh)
{
    QIODevice* file = reinterpret_cast<QIODevice*>(fh);
    size_t len = size1 * size2;
    return file->read((char*)dst, len);
}

int seekOggVorbis( void *fh, ogg_int64_t to, int type ) {
    QIODevice* file = reinterpret_cast<QIODevice*>(fh);
    bool success = false;
    switch( type ) {
        case SEEK_CUR:
            success = file->seek(file->pos() + to);
            break;
        case SEEK_END:
            success = file->seek(file->size() - to);
            break;
        case SEEK_SET:
            success = file->seek(to);
            break;
    }
    return success ? 0 : -1;
}

int closeOggVorbis(void* fh) {
    QIODevice* file = reinterpret_cast<QIODevice*>(fh);
    file->close();
    return 0;
}

long tellOggVorbis( void *fh ) {
     QIODevice* file = reinterpret_cast<QIODevice*>(fh);
     return file->pos();
}



int readOggOpus(void *fh, unsigned char* dst, int len) {
    QIODevice* file = reinterpret_cast<QIODevice*>(fh);
    return file->read((char*)dst, len);
}


int seekOggOpus( void *fh, opus_int64 to, int type ) {
    return seekOggVorbis(fh, to, type);
}

opus_int64 tellOggOpus( void *fh ) {
     QIODevice* file = reinterpret_cast<QIODevice*>(fh);
     return file->pos();
}

const auto& closeOggOpus = closeOggVorbis;

}
}
