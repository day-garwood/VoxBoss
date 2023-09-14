#include "vb.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
vb_speaker s;
char* text=malloc(1024);
if(!text)
{
printf("Error allocating memory.");
getchar();
return 0;
}
vb_result rc=vb_speaker_initialise(&s, NULL);
if(rc!=vbr_ok)
{
printf("Error initialising speech subsystem: %d.", rc);
getchar();
free(text);
return 0;
}
rc=vb_speaker_start(&s);
if(rc!=vbr_ok)
{
printf("Error starting speech subsystem: %d.", rc);
getchar();
free(text);
return 0;
}
while(1)
{
printf("Please enter some text.\n");
fgets(text, 1023, stdin);
if(text[0]==10) break;
vb_speak(&s, text, 0);
printf("Speaking...Please press enter to continue.\n");
getchar();
}
vb_speaker_cleanup(&s);
free(text);
return 0;
}
