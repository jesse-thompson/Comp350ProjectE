// COMP350-001
// ProjectE
// 11/11/22
// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*, int*);
void executeProgram(char* name);
void terminate();
void handleInterrupt21(int,int,int,int);
void handleTimerInterrupt(int, int);
void initializeProgram(int segment);
void restoreDataSegment(int segment);
int setKernelDataSegment();
// Setting up variables for timerInterrupt
int processActive[8];
int processStackPointer[8];
int currentProcess;


void main()
{
    // Setting up for Step 2
    int processIndex;

    // Setting up for readString
    char line[80];

    // Setting up readSector
    char sectorBuffer[512];

    // Setting up readFile
    char fileBuffer [13312];
    char* fileName = "messag";
    int sectorsRead;


    // Initializing process arrays
    for (processIndex = 0; processIndex < 8; processIndex++)
    {
        processActive[processIndex] = 0;
        processStackPointer[processIndex] = 0xff00;
    }
    currentProcess = -1;


    makeInterrupt21();

    executeProgram("shell");

    makeTimerInterrupt();

    while(1);
}

void printChar(char c)
{
    interrupt(0x10, 0xe*256+c,0,0,0);
}

void printString(char* chars)
{
    int increment = 0;
    while(chars[increment] != 0x0)
    {
        interrupt(0x10, 0xe*256+chars[increment],0,0,0);
        increment++;
    }
}

void readString(char* chars)
{
    int currIndex = 0;
    chars[currIndex] = interrupt(0x16, 0,0,0,0);
    printChar(chars[currIndex]);

    while(chars[currIndex] != 0xd && currIndex < 80)
    {
        char input = interrupt(0x16, 0,0,0,0);
        if(input == 0x8)
        {
            if(currIndex >= 0)
            {
                currIndex--;
                printChar(0x8);
                printChar(' ');
                printChar(0x8);
            }
        }
        else
        {
            currIndex ++;
            chars[currIndex] = input;
            printChar(input);
        }
    }
    printChar(0xa);

    chars[currIndex+1] = 0xa;
    chars[currIndex+2] = 0x0;
}

void readSector(char* buffer, int sector)
{
    interrupt(0x13, 2*256+1, buffer, sector+1, 0x80);
}

void readFile(char* fileName, char* buffer, int* sectorsRead)
{
    //int printIndex; // Index used for printing out the characters of the directory
    int correctCharIndex; // Index used for comparing how many characters in fileName match with directory[fileEntry]
    int correctChars; // The number of matching characters when comparing fileName and directory[fileEntry]

    int sectorIndex; // Index used for reading what sectors a file is stored on

    // The following variables are used for padding out characters in fileName
    int i;
    int pad;

    int fileEntry; // fileEntry acts as an index for accessing the data inside the directory
    char directory[512];

    readSector(directory,2); // Directory is at sector 2

    // Padding out fileName with 0's
    pad = 0; // Pad is false
    for (i = 0; i < 6; ++i)
    {
        if (fileName[i] == '\r' || fileName[i] == '\n')
        {
            pad = 1;
        }
        if (pad == 1)
            fileName[i] = '\0';
    }

    for (fileEntry = 0; fileEntry < 512; fileEntry += 32)
    {
        correctChars = 0;

        // fileName has to match identically with the first 6 entries of file stored in the directory
        for (correctCharIndex = 0; correctCharIndex < 6; correctCharIndex++)
        {
            if (fileName[correctCharIndex] == directory[fileEntry + correctCharIndex])
            {
                correctChars++;
            }

            // Seeing if all 6 chars in fileName match with what is in the directory
            if (correctChars == 6)
            {
                // Now that we've found the file, we need to find what sectors the file is on
                // Starting the index at 6 since the sectors that the file are stored on also start at index 6
                for (sectorIndex = 6; sectorIndex < 32; sectorIndex++)
                {
                    if (directory[fileEntry + sectorIndex] == 0x0)
                    {
                        // This was a test for readFile in kernel.c but this won't be helpful for shell
                        printString("All sectors found. \n\r");
                        break;
                    }
                    else
                    {
                        readSector(buffer, directory[fileEntry + sectorIndex]);
                        buffer += 512;
                        *sectorsRead = *sectorsRead + 1;
                    }
                }
            }
        }
    }
}

void executeProgram(char* name)
{
    int processIndex;
    int fileIndex;
    int numSectorsRead = 0;
    char buffer[13312];
    int dataseg;
    int newProcessSegment;

    readFile(name, buffer, &numSectorsRead);

    // Step 3

    if (numSectorsRead > 0) // If numSectorsRead > 0, then the file was found
    {

        for (processIndex = 0; processIndex < 8; processIndex++) // Iterating through activeProcess to find an available index
        {
	    dataseg = setKernelDataSegment();
            if (processActive[processIndex] == 0) // Finds the first available index
            {
                // This control structure is necessary to make sure the global variables are accurate

                newProcessSegment = (processIndex + 2) * 0x1000;

                for (fileIndex = 0; fileIndex < 13312; fileIndex++)
                {
                    putInMemory(newProcessSegment, fileIndex, buffer[fileIndex]);
                }

                initializeProgram(newProcessSegment);

                processActive[processIndex] = 1;

                processStackPointer[processIndex] = 0xff00;
                restoreDataSegment(dataseg);

                break;
            }
        }
    }

    else
    {
        printChar('B');
        printChar('a');
        printChar('d');
        printChar(' ');
        printChar('a');
        printChar('r');
        printChar('g');
        printChar('u');
        printChar('m');
        printChar('e');
        printChar('n');
        printChar('t');
        printChar('\r');
        printChar('\n');

        terminate();
    }
}

void terminate()
{
    int dataseg;

    dataseg = setKernelDataSegment();
    processActive[currentProcess] = 0;
    restoreDataSegment(dataseg);

    while(1);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
    switch(ax)
    {
        case 0:
            printString(bx);
            break;
        case 1:
            readString(bx);
            break;
        case 2:
            readSector(bx, cx);
            break;
        case 3:
            readFile(bx, cx, dx);
            break;
        case 4:
            executeProgram(bx);
            break;
        case 5:
            terminate();
            break;
        default:
            printString("No interrupt function correlated with AX number");
    }
}

void handleTimerInterrupt(int segment, int sp)
{
    int i;
    int processIndex;
    int dataseg;
    int newSegment;
    int newSp;

//    printChar('T');
//    printChar('i');
//    printChar('c');
//    printChar('\r');
//    printChar('\n');

    // Step 4
    dataseg = setKernelDataSegment();

    for(i=0; i<8; i++)
        {
                putInMemory(0xb800,60*2+i*4,i+0x30);
                if(processActive[i]==1)
                        putInMemory(0xb800,60*2+i*4+1,0x20);
                else
                        putInMemory(0xb800,60*2+i*4+1,0);
        }


    if (currentProcess != -1)
    {
        processStackPointer[currentProcess] = sp;
        restoreDataSegment(dataseg);
    }
    //TODO: figure out why it's not changing processes
    //maybe due to currentProcess data set? idk


    while(1)
    {
        // Looping currentProcess back to 0 to start the process over if a process wasn't found yet
        dataseg = setKernelDataSegment();
        if (currentProcess == 7)
        {
            currentProcess = 0;
            restoreDataSegment(dataseg);
        }
        else
        {
            currentProcess++;
            restoreDataSegment(dataseg);
        }

        // Looking for active processes
        dataseg = setKernelDataSegment();
        if (processActive[currentProcess] == 1)
        {
            newSegment = ((currentProcess + 2) * 0x1000);
            newSp = processStackPointer[currentProcess];
            break;
        }
    }
    restoreDataSegment(dataseg);
    returnFromTimer(newSegment, newSp);
}
