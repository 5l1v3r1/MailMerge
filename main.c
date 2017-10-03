#include <stdio.h>
#include <string.h>
#include "xml.h"

int isExtensionXml(char * arg);

int main(int argc, char **argv)
{

    int i;
    int necessaryFlags = 0; // number of arguments given from command line
    int everythingFine = 1; // is used error detection within command line arguments
    const char * TEMPLATE_FILENAME = NULL;
    const char * VARIABLE_FILENAME = NULL;
    char * OUTPUT_FILEPATH = NULL;

    if (argc == 7){ // ./mm inclusive!

        for (i = 1; i < argc; i++) {
            int isTemplate; // is take name of template file.
            if ((isTemplate = strcmp("-t", argv[i]) == 0) || strcmp("-v" ,argv[i]) == 0 ){

                if(!isExtensionXml(argv[i+1])) {
                    fprintf(stderr, "Input files must have xml extension! \n");
                    everythingFine = 0;
                    break;
                }

                isTemplate ? (TEMPLATE_FILENAME = argv[i+1]) : (VARIABLE_FILENAME = argv[i+1]);
                i++;
                necessaryFlags++;
            } else if (strcmp("-o", argv[i]) == 0){
                if (!argv[i+1]){
                    fprintf(stderr, "Output file name must be written! \n");
                    everythingFine = 0;
                    break;
                }
                else if (isExtensionXml(argv[i+1])){
                    fprintf(stderr, "Output file does not have extension of xml! \n");
                    everythingFine = 0;
                    break;
                }
                OUTPUT_FILEPATH = argv[i+1];
                i++;
                necessaryFlags++;
            }
            else fprintf(stderr, "Error! \n");

            if (i == argc-1 && necessaryFlags != 3) {
                fprintf(stderr, "There must be three flag (-o,-v,-t) \n");
                everythingFine = 0;
                break;
            }

        }

    } else fprintf(stderr, "Error! \n");

    if (everythingFine)
        fillTemplate(TEMPLATE_FILENAME, VARIABLE_FILENAME, OUTPUT_FILEPATH);

    return 0;


}

int isExtensionXml(char * arg){

    char extension[4];
    int i = 0;
    while (i != 3){ //substring to get extension from filename.
        extension[2-i] = arg[strlen(arg)-1-i];
        i++;
    }
    extension[i] = '\0';

    if (strcmp(extension, "xml") == 0)
        return 1;
    return 0;

}

