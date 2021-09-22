#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

#include <AL/al.h>
#include <AL/alc.h>

#define NUM_BUFFERS 12
#define NUM_SOURCES 13

// These index the buffers and sources.
#define RAIN       0
#define NEWS       1
#define PICK_UP    2
#define ARGUE      3
#define KITCHEN    4
#define CHARLA     5
#define WINNER     6
#define OPEN_BAG   7
#define GRABACION  8
#define OPEN_DOOR  9
#define SAFE      10
#define DISGUST   11
#define BREATH    12


// Buffers to hold sound data.
ALuint Buffers[NUM_BUFFERS];
// Sources are points of emitting sound.
ALuint Sources[NUM_SOURCES];

// Position of the source sounds.
ALfloat SourcesPos[NUM_SOURCES][3];
// Velocity of the source sounds.
ALfloat SourcesVel[NUM_SOURCES][3];

// Position of the Listener.
ALfloat ListenerPos[] = {0.0, 0.0, 0.0};
// Velocity of the Listener.
ALfloat ListenerVel[] = {0.0, 0.0, 0.0};
// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
ALfloat ListenerOri[] = {0.0, 0.0, -1.0, 0.0, -1.0, 0.0};

void cambiar(int pos, char *ruta, ALenum *format, ALvoid **data, ALsizei *size, ALboolean *loop, ALsizei *freq){
    alutLoadWAVFile(ruta, format, data, size, freq, loop);
    alBufferData(Buffers[pos], *format, *data, *size, *freq);
    SourcesPos[pos][0] = 0;
    SourcesPos[pos][1] = 0;
    SourcesPos[pos][2] = 0;
    alSourcei (Sources[pos], AL_BUFFER,   Buffers[pos]      );
    alSourcef (Sources[pos], AL_PITCH,    1.0f            );
    alSourcef (Sources[pos], AL_GAIN,     1.0f            );
    alSourcefv(Sources[pos], AL_POSITION, SourcesPos[pos]);
    alSourcefv(Sources[pos], AL_VELOCITY, SourcesVel[pos]);
    alSourcei (Sources[pos], AL_LOOPING,  AL_FALSE        );
    alSourcePlay(Sources[pos]);
}

void help(){
    printf("===========================================================\n");
    printf("                Welcome to hotel mistery!                  \n");
    printf("===========================================================\n");
    printf("You are solving a murderer in the hotel 'Gucci Praga Hotel'\n");
    printf("===========================================================\n");
    printf("To move around the rooms use the folowing commands         \n");
    printf("          left, right, up, down                            \n");
    printf("===========================================================\n");
    printf("To interact with the environment use the folowing commands \n");
    printf("look around                                                \n"
           "see progress                                               \n"
           "open inventory                                             \n"
           "open (name of the object)                                  \n"
           "grab (name of the item)                                    \n"
           "talk                                                       \n"
           "watch (name of the item)                                   \n");
    printf("===========================================================\n");
    printf("To see commands list againt type help                      \n");
    printf("To quit the game write quit                                \n");
    printf("===========================================================\n");
}

void descomponer(char palabras[2][20], char ins[20]){
    int i, rep = 0, tam = 0;
    char temp[20];
    strcpy(temp, "");
    for(i = 0; i < (int)strlen(ins) + 1 && rep < 2; ++i){
        if((ins[i] == ' ' || ins[i] == '\0') && tam){
            temp[tam++] = '\0';
            strcpy(palabras[rep++], temp);
            tam = 0;
        }
        else if(!(ins[i] == ' ' || ins[i] == '\0'))
            temp[tam++] = ins[i];
    }
}   

int main(int argc, const char * argv[])
{
    ALenum format;
    ALsizei size;
    ALsizei freq;
    ALvoid* data;
    ALboolean loop = AL_FALSE;
    ALboolean enumeration;
    ALCcontext *context;
    ALCdevice *device;
    const ALCchar *defaultDeviceName = argv[1];
    
    int sourceState;
    ALfloat sourcePos[] = {0.0, 0.0, 0.0};

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if(enumeration == AL_FALSE)
        fprintf(stderr, "enumeration extension not available\n");
    
    if(!defaultDeviceName)
        defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    
    device = alcOpenDevice(defaultDeviceName);
    if(!device)
    {
        fprintf(stderr, "unable to open default device\n");
        return -1;
    }
    
    alGetError();
    
    context = alcCreateContext(device, NULL);
    if(!alcMakeContextCurrent(context))
    {
        fprintf(stderr, "failed to make default context\n");
        return -1;
    }
    
    alListenerfv(AL_POSITION, ListenerPos);
    alListenerfv(AL_VELOCITY, ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    
    // Bind the buffer with the source.
    alGenSources(NUM_SOURCES, Sources);

    // Load wav data into a buffer.
    alGenBuffers(NUM_BUFFERS, Buffers);

    // Load in the WAV file from disk
    alutLoadWAVFile("sonidos/rain.wav", &format, &data, &size, &freq, &loop);
    
    // Copy the new wav data into the buffer
    alBufferData(Buffers[0],format,data,size,freq);

    // Attach buffer 0 to our source
    SourcesPos[RAIN][0] = 0;
    SourcesPos[RAIN][1] = 0;
    SourcesPos[RAIN][2] = 0;
    alSourcei (Sources[RAIN], AL_BUFFER,   Buffers[RAIN]      );
    alSourcef (Sources[RAIN], AL_PITCH,    1.0f            );
    alSourcef (Sources[RAIN], AL_GAIN,     1.0f            );
    alSourcefv(Sources[RAIN], AL_POSITION, SourcesPos[RAIN]);
    alSourcefv(Sources[RAIN], AL_VELOCITY, SourcesVel[RAIN]);
    alSourcei (Sources[RAIN], AL_LOOPING,  AL_TRUE         );



    help();
        
    char c[20];
    char palabras[2][20];
    int inventory[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //0:key, 1:paper, 2:recorder, 3:tv, 4:safe, 5:poison, 6:black_towel, 7:plate, 8:blancket, 9:door, 10:pasword
    char progress[4][30] = {"", "", "", ""};
    int hab = 0;
    int flag = 0;
    int pos = 0;
    while(strcmp(c, "quit")){
        strcpy(palabras[0], "");
        strcpy(palabras[1], "");
        fgets(c, 20, stdin);
        c[(int)strlen(c) - 1] = '\0';
        descomponer(palabras, c);
        alSourceStop(Sources[pos]);
        alSourceStop(Sources[RAIN]);
        flag = 1;
        printf("================================================================================\n");
        if(!strcmp(palabras[0], "help")){
            flag = 0;
            help();
        }
        else if(!(strcmp(palabras[0], "look") || strcmp(palabras[1], "around"))){
            flag = 0;
            if(hab == 0){
                //First room
                printf("================================================================================\n");
                printf("You are on the crime scene, where the body was found. There is a bed where just \n"
                       "a few minutes ago was the body. On the left you found a desk "); 
                if(!inventory[1]) printf("with a piece of paper on it"); 
                printf(".\nOn the wall lays a tv ");
                if(inventory[3]){
                    printf("that is turn on.\n");
                    //News soudn
                    cambiar(NEWS, "sonidos/news.wav", &format, &data, &size, &loop, &freq);
                    pos = NEWS;
                }
                else printf("that is turn off.\n");
                printf("There is a door on the left of the room that takes you into the hall.           \n");
                printf("================================================================================\n");
            }
            else if(hab == 1){
                printf("================================================================================\n");
                printf("You are on the hall of the second floor. On the left is the room 202. On the    \n"
                       "right is the room where the body was. There is also the elevator that takes you \n"
                       "to reception. There is also a strange painting on the wall.\n");
                printf("================================================================================\n");
            }
            else if(hab == 2){
                printf("================================================================================\n");
                printf("You are on the room 202. It's dirty an fill with stuff.\n");
                if(!inventory[2])
                    printf("You noticed some strange device. It appers to be a recorder.\n");
                printf("On the right is the hall.\n");
                printf("================================================================================\n");
                //Insert disgust sound
                cambiar(DISGUST, "sonidos/disgusting.wav", &format, &data, &size, &loop, &freq);
                pos = DISGUST;
            }
            else if(hab == 3){
                printf("================================================================================\n");
                printf("In the reception you see the manager of the hotel. You notice that his inicials \n"
                       "are GM. You heard an argue to the left where the kitchen is. To the right is the\n"
                       "library, where nothing seems to happen. U also see the elevator that takes you  \n"
                       "back to the first floor.\n");
                printf("================================================================================\n");
                //Insert argue sound
                cambiar(ARGUE, "sonidos/argue.wav", &format, &data, &size, &loop, &freq);
                pos = ARGUE;
            }
            else if(hab == 4){
                printf("================================================================================\n");
                printf("On the kitchen u see a person that is in shock. U see a pile of dishes that are\n"
                       "still dirty. ");
                if(!inventory[6])
                    printf("You can see that there is a black towell on the floor.\n");
                else{
                    if(!inventory[5])
                    printf("Where before was a towell, you sea a bottle of some strange liquid\n");
                }
                printf("on the right is the reception.\n");
                printf("================================================================================\n");
                //Insert kitchen sound (fries)
                cambiar(KITCHEN, "sonidos/kitchen.wav", &format, &data, &size, &loop, &freq);
                pos = KITCHEN;
            }
            else if(hab == 5){
                printf("================================================================================\n");
                printf("The library is empty you heard only the noice of your breath.\n"
                       "There are some tables filled with books and bookshelfs.   \n"
                       "On one of the tables somebody forgot a plate that have a strange smell.         \n");
                if(!inventory[8])
                    printf("Under the same table is some strage box that is coverd with a blancket\n");
                else if(!inventory[4])
                    printf("The strange box was a safe that needs some kind of code\n");
                else{
                    if(!inventory[0])
                        printf("On the safe is a key.\n");
                    else
                        printf("The safe is empty.\n");
                }
                printf("On the left is the reception.\n");
                printf("================================================================================\n");
                //insert respiración sound
                cambiar(BREATH, "sonidos/breath.wav", &format, &data, &size, &loop, &freq);
                pos = BREATH;
            } 
        }
        else if(!(strcmp(palabras[0], "see") || strcmp(palabras[1], "progress"))){
            flag = 0;
            int i, ver = 0;
            for(i = 0; i < 4; ++i){
                if(strcmp(progress[i], "")){
                    ++ver;
                    printf("%s\n", progress[i]);
                }
            }
            if(ver == 4){
                printf("CONGRATULATIONS YOU COMPLATE THE GAME\n"
                       "write quit to close the game.\n");
                //insert win noice
                cambiar(WINNER, "sonidos/winner.wav", &format, &data, &size, &loop, &freq);
                pos = WINNER;
            }
        }
        else if(!(strcmp(palabras[0], "open") || strcmp(palabras[1], "inventory"))){
            flag = 0;
            printf("you got\n");
            //insert Abrir bag
            cambiar(OPEN_BAG, "sonidos/bag.wav", &format, &data, &size, &loop, &freq);
            pos = OPEN_BAG;
            if(inventory[0]) 
                printf("key\n");
            if(inventory[1])
                printf("paper\n");
            if(inventory[2])
                printf("recorder\n");
            if(inventory[5])
                printf("bottle\n");
            if(inventory[10])
                printf("pasword\n");
        }
        else if(!strcmp(palabras[0], "open")){
            flag = 0;
            if(hab == 0){
                printf("There is nothing to open");
            }
            else if(hab == 1){
                if(!strcmp(palabras[1], "door") && inventory[0]){
                    printf("Door of room 202 opened.\n");
                    inventory[9] = 1;
                    //Insert open door
                    cambiar(OPEN_DOOR, "sonidos/door.wav", &format, &data, &size, &loop, &freq);
                    pos = OPEN_DOOR;
                    
                }
                else
                    printf("%s can not be open.\n", palabras[1]);
            }
            else if(hab == 2){
                printf("There is nothing to open");
            }
            else if(hab == 3){
                printf("There is nothing to open here.\n");
            }
            else if(hab == 4){
                printf("There is nothing to open here.\n");
            }
            else if(hab == 5){
                if(!strcmp(palabras[1], "safe")){
                    if(!inventory[4] && inventory[10]){
                        printf("You succesfully use the pasword and open the safe.\n");
                        inventory[4] = 1;
                        //INsert caja metalica
                        cambiar(SAFE, "sonidos/safe.wav", &format, &data, &size, &loop, &freq);
                        pos = SAFE;
                    }
                    else if(!(inventory[4] || inventory[10]))
                        printf("You seems to dont have the pasword of the safe'\n");
                    else 
                        printf("You already open the safe");
                }
                else
                    printf("%s can not be open.\n", palabras[1]);
            } 
        }
        else if(!strcmp(palabras[0], "grab")){
            flag = 0;
            //INsert pickup sound
            cambiar(PICK_UP, "sonidos/shining.wav", &format, &data, &size, &loop, &freq);
            pos = PICK_UP;
            if(hab == 0){
                //First room
                if(!strcmp(palabras[1], "paper")){
                    if(!inventory[1]){
                        inventory[1] = 1;
                        printf("You grab the paper.\n");
                    }
                    else
                        printf("Paper already on inventory.\n");
                }
                else
                    printf("you can not grab %s.\n", palabras[1]);
            }
            else if(hab == 1){
                printf("There is nothing to grab here.\n");
            }
            else if(hab == 2){
                if(!strcmp(palabras[1], "recorder")){
                    if(!inventory[2]){
                        printf("Recorder succesfully taken.\n");
                        inventory[2] = 1;
                    }
                    else
                    printf("Recorder already on inventory.\n");
                }
                else
                    printf("You can not grab %s.\n", palabras[1]);
            }
            else if(hab == 3){
                printf("There is nothing to grab here.\n");
            }
            else if(hab == 4){
                if(!strcmp(palabras[1], "towell")){
                    if(!inventory[6]){
                        printf("Towell succesfully removed.\n");
                        inventory[6] = 1;
                    }
                    else
                        printf("You already remove the towell.\n");
                }
                else if(!strcmp(palabras[1], "bottle") && inventory[6]){
                    if(!inventory[5]){
                        printf("Bottle was taken.\n");
                        inventory[5] = 1;
                    }
                    else
                        printf("You have the botte on inventory");
                }
                else
                    printf("You can not grab %s.\n", palabras[1]);
            }
            else if(hab == 5){
                if(!strcmp(palabras[1], "blancket")){
                    if(!inventory[8]){
                        printf("Blancket succesfully remove.\n");
                        inventory[8] = 1;
                    }
                    else
                        printf("You already remove the blancket");
                }
                else if(!strcmp(palabras[1], "key") && inventory[4]){
                    if(!inventory[0]){
                        printf("Key succesfully taken.\n");
                        inventory[0] = 1;
                    }
                    else
                        printf("Key already on inventory.\n");
                }
                else
                    printf("You can not grab %s.\n", palabras[1]);
            }
        }
        else if(!strcmp(palabras[0], "talk")){
            flag = 0;
            if(hab == 0){
                printf("I probablly should watch the tv.\n");
            }
            else if(hab == 1){
                if(!inventory[10])
                    printf("That painting seems to have something, i probably should watch it.\n");
                if(!inventory[0])
                    printf("The room 202 is closed I need the key.\n");
            }
            else if(hab == 2){
                printf("This place is gross. Who could stay in here?.\n"
                       "I should find something in here.\n");
            }
            else if(hab == 3){
                printf("You talk with GM.\n");
                if(!inventory[2] && !inventory[5]){
                    printf("GM: It's a shame what happen. He was a good man.\n");
                    printf("you: Nothing more you wanna say?.\n");
                    printf("GM: No. BY the way my name is Gerard Moreno. I'm here for anything you need.\n");
                    //Insert charla
                    cambiar(CHARLA, "sonidos/talking.wav", &format, &data, &size, &loop, &freq);
                    pos = CHARLA;
                }
                else if(inventory[2] && !inventory[5])
                    printf("You: I nned tp find the murder weapon. Probably on the kitchen.\n");
                else{
                    printf("you: It seems you know more than u say.\n");
                    printf("GM: He was just pretending to be a good man. He don't deserve to live.\n");
                    printf("you: That's not your decision. It's the law who decides. Take him.\n");
                    strcpy(progress[1], "Murderer: Mark Grant.");
                    //Insert charla
                    cambiar(CHARLA, "sonidos/talking.wav", &format, &data, &size, &loop, &freq);
                    pos = CHARLA;
                }
            }
            else if(hab == 4){
                if(!inventory[5])
                    printf("Servant: I don't know nothing. I swear!.\n");
                else{
                    printf("Servant: huff. Someone blackmails me it identifies himself as MG.\n" 
                           "Servant: I give the posion to the guest.\n");
                    printf("Servant: you should go to the library. There he drinks the poison.\n");
                    strcpy(progress[2], "Murder weapon: poison.");
                }
                //Insert charla
                cambiar(CHARLA, "sonidos/talking.wav", &format, &data, &size, &loop, &freq);
                pos = CHARLA;
            }
            else if(hab == 5){
                printf("That liquid on the plate is strange.\n");
                if(!inventory[8])
                    printf("There is something under the blancket.\n");
                else{
                    if(!inventory[4]){
                        printf("It needs some kind of code.");
                        if(!inventory[10])
                            printf("I Should go to the second floor.\n");
                        else
                            printf("Lucky I discovered some strange numbers I will try.\n");
                    }
                }
            }
            printf("I should watch thinks in my inventory.\n");
        }
        else if(!strcmp(palabras[0], "watch")){
            flag = 0;
            if(inventory[1] && !strcmp(palabras[1], "paper")){
                printf("The paper is some kind of blackmail addresed to the victim.\n"
                       "It is signed by someone whose initials are MG.\n");
            }
            else if(inventory[10] && !strcmp(palabras[1], "pasword"))
                printf("It's some kind of pasword maybe for a safe.\n");
            else if(inventory[5] && !strcmp(palabras[1], "bottle"))
                printf("It's some kind of liquid. Maybe poison but why?.\n");
            else if(inventory[0] && !strcmp(palabras[1], "key"))
                printf("It seems a key to a room. Maybe it's for room 202.\n");
            else if(inventory[2] && !strcmp(palabras[1], "recorder")){
                printf("Recorder: I'm tyred. This life has no sense since the lawyer destroy my family\n"
                       "Recorder: it's his fault. Only because on that time we have no money to offer \n"
                       "Recorder: I cannot wait. I have to change my name to Gerard Moreno.\n"
                       "Recorder: but I'm Mark Grant. The son of the ex owner of the multimillionaire enterprice Grant\n"
                       "Recorder: Now, I'm just the manager of this *** Hotel.\n"
                       "Recorder: yesterday he cames. Know is my time. I'm gonna blackmail the servant\n"
                       "Recorder: I will give him poison to murder that *** lawyer.\n");
                printf("You: so it was the manager who would have imagined.\n");
                strcpy(progress[3], "Motive: revenge");
                //insert grabacion
                cambiar(GRABACION, "sonidos/recorder.wav", &format, &data, &size, &loop, &freq);
                pos = GRABACION;
            }
            else if(hab == 0 && !strcmp(palabras[1], "tv")){
                inventory[3] = 1;
                printf("Today in hotel 'Gucci Praga Hotel' was found the dead body of\n"
                       "Mr. Gonzalo Ruth. A famous layer that has solved lot's of cases.\n"
                       "The group of lawyers cries his dead.\n");
                printf("You: So Gonzalo Ruth. That's his names.\n");
                strcpy(progress[0], "Deceased: Gonzalo Ruth.");
                //News sound
                cambiar(NEWS, "sonidos/news.wav", &format, &data, &size, &loop, &freq);
                pos = NEWS;
            }
            else if(hab == 1  && !strcmp(palabras[1], "painting")){
                printf("On that painting is a series of numbers. 0 1 1 2 3 5 8.\n"
                       "Fibonnacci hmm? Why are 3 numbers that are different. I probably should note this.\n");
                inventory[10] = 1;
            }
            else if(hab == 5 && !strcmp(palabras[1], "plate"))
                printf("It seems poisoned food. Maybe this was the murder weapon.\n");
            else
                printf("I dont know what i was specting.\n");
        }
        else if(!strcmp(palabras[0], "left")){
            flag = 0;
            if(hab == 0){
                printf("You enter the hall.\n");
                hab = 1;
            }
            else if(hab == 1 && inventory[9]){
                printf("You enter the room 202.\n");
                hab = 2;
            }
            else if(hab == 3){
                printf("You enter the kitchen.\n");
                hab = 4;
            }
            else if(hab == 5){
                printf("You enter the reception.\n");
                hab = 3;
            }
            else{printf("You cant go that way.\n");}
        }
        else if(!strcmp(palabras[0], "right")){
            flag = 0;
            if(hab == 2){
                printf("You enter the hall.\n");
                hab = 1;}
            else if(hab == 1){
                printf("You enter the room.\n");
                hab = 0;
            }
            else if(hab == 3){
                printf("You enter the library.\n");
                hab = 5;
            }
            else if(hab == 4){
                printf("You enter the reception.\n");
                hab = 3;
            }
            else{printf("You cant go that way.\n");}
        }
        else if(!strcmp(palabras[0], "up")){
            flag = 0;
            if(hab == 3){
                printf("You enter the hall.\n");
                hab = 1;
            }
            else{printf("You cant go that way.\n");}
        }
        else if(!strcmp(palabras[0], "down")){
            flag = 0;
            if(hab == 1){
                printf("You enter the reception.\n");
                hab = 3;
            }
            else{printf("You cant go that way.\n");}
        }
        else if(!strcmp(c, "quit")){
            flag = 0;
            printf("I hope u enjoy.\n");
        }
        if(flag)
            printf("Sorry but I don't understand.\n");
        printf("================================================================================\n");
    }
    
    alDeleteSources(NUM_SOURCES, Sources);
    alDeleteBuffers(NUM_BUFFERS, Buffers);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    
    return 0;
}
