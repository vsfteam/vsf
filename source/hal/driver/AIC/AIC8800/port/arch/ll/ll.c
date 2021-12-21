/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/

#include "ll.h"

/*****************************************************************************/
/* Variables                                                                 */
/*****************************************************************************/

static unsigned int critical_sect_count = 0U;
static vsf_arch_prio_t critical_ctxt_saved = 0U;

/*****************************************************************************/
/* Functions                                                                 */
/*****************************************************************************/

void critical_section_start(void)
{
    if (0U == critical_sect_count) {
        critical_ctxt_saved = vsf_disable_interrupt();
    }
    critical_sect_count++;
}

void critical_section_end(void)
{
    critical_sect_count--;
    if (0U == critical_sect_count) {
        vsf_set_interrupt(critical_ctxt_saved);
    }
}
