
#include <stdio.h>
#include "ddr2lib.h"

/* syntax: readDDR2 address
   address is a 16 bits DDR2 register address
*/
int main(int argc, const char* argv[])
{
    int ret = 0;
    ddr2_addr_t addr;
    uint16_t val;
    
    if ( argc == 2 )
    {
        /* read input address */
        if ( ddr2_parse_address(argv[1], &addr) )
        {
            ret = ddr2_read(addr, &val);
            if ( ret >= 0 )
            {
				fprintf (stdout, "value: 0x%X\n", val);

                if ( 4 == addr )
                {
                    if ( (val & 0x7F) > 3 ) {
                        /* temperature is greater than 85 °C */
                        fprintf(stdout, "temperature > 85 °C\n");
                    }
                    else {
                        /* temperature is lower than 85 °C */
                        fprintf(stdout, "temperature < 85 °C\n");
                    }
                }
            }
            else
            {
                /* error: fail to read DDR2 */
                fprintf(stderr, "error: fail to read from DDR2 (code %d)\n", ret);
            }
        }
        else
        {
            /* error: input is not a 16 bits address */
            fprintf(stderr, "error: not a valid DDR2 register address.\n");
        }
    }
    else
    {
        /* print usage */
        fprintf(stdout, "syntax: readDDR2 <adress>\nadress is a 16 bits DDR2 register address.\n");
    }
    
    return ret;
}

