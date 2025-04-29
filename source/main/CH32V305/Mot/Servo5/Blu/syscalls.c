

#include <reent.h>
#include <errno.h>
#include <stdlib.h> /* abort */
#include <string.h> /* strcpy */
#include <sys/types.h>
#include <sys/stat.h>


/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length
 *
 * @return  size: Data length
 */
__attribute__((used)) int _write(int fd, char* buf, int size)
{
  (void)fd;
  (void)buf;

  return size;
}

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
void* _sbrk(ptrdiff_t incr)
{
  extern char _end[];
  extern char _heap_end[];
  static char* curbrk = _end;

  if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

  curbrk += incr;
  return curbrk - incr;
}

void _fini() {}
void _init() {}



