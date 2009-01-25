/*    epocprinting.cpp
 *
 * An empty Symbian OS implementation of PuTTY printing support
 *
 * Copyright 2002 Petteri Kangaslampi
 *
 * See license.txt for full copyright and license information.
*/

extern "C" {
#include "putty.h"
}

printer_enum *printer_start_enum(int * /*nprinters_ptr*/) {
    return NULL;
}

char *printer_get_name(printer_enum * /*pe*/, int /*i*/) {
    return NULL;
}

void printer_finish_enum(printer_enum * /*pe*/) {
}

printer_job *printer_start_job(char * /*printer*/) {
    return NULL;
}

void printer_job_data(printer_job * /*pj*/, void * /*data*/, int /*len*/) {
}

void printer_finish_job(printer_job * /*pj*/) {
}
