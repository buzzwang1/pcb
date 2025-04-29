#ifndef ITPMLOG_CI_H
#define ITPMLOG_CI_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef ITPM_ENABLED

/* Size in Elements of the ITPMLog Buffer   */
/* RAM usage: ITPMLOG_nNumberOfElements * 8 */
#define ITPMLOG_nNumberOfElements       20

/* LogForEver: Continue logging, even when ringbuffer is full.
               In that case, old values will be overwritten.
*/
#define ITPMLOG_nLogForEver              1

#endif // ITPM_ENABLED

#ifdef __cplusplus
}
#endif

#endif

