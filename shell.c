// Craig Kimball
// TJ Bourget
// Jesse Thompson
// Sean Tammelleo


void takeCommand(char* command);

void main()
{
    while (1)
    {
        char commandInput[80];
        char fileBuffer[13312];
        int bufferIndex;
        int sectorsRead;

        char fileName[6];
        int commandIndex;

        // Initializing variables for repeated use of type commands
        sectorsRead = 0;

        for (bufferIndex = 0; bufferIndex < 13312; bufferIndex++)
        {
            fileBuffer[bufferIndex] = '\0';
        }

        // calls handleInterrupt21() case 0: printString()
        syscall(0, "SHELL> ",0,0);
        // calls handleInterrupt21() case 1: readString()
        syscall(1, commandInput,0,0);

        // commandInput[5] through commandInput[10] is the fileName
        for (commandIndex = 0; commandIndex < 6; commandIndex++)
        {
            fileName[commandIndex] = commandInput[commandIndex + 5];
        }


        // checking if the user wants to use the type command
        // this if statement is a little scuffed, but it'll work
        if (commandInput[0] == 't' &&
            commandInput[1] == 'y' &&
            commandInput[2] == 'p' &&
            commandInput[3] == 'e' &&
            commandInput[4] == ' ')
        {
            syscall(3, fileName, fileBuffer, &sectorsRead);
            if (sectorsRead > 0) //if 0, file was not found and no sectors were read
            {
                // calls handleInterrupt21() case 0: printString()
                syscall(0, fileBuffer, 0, 0);
            }
            else
            {
                // calls handleInterrupt21() case 0: printString()
                syscall(0, "Error: File not found", 0, 0);
            }

        }

        // checking if the user wants to execute a program
        // Again, it's a bit scuffed here
        else if (commandInput[0] == 'e' &&
                 commandInput[1] == 'x' &&
                 commandInput[2] == 'e' &&
                 commandInput[3] == 'c' &&
                 commandInput[4] == ' ')
        {
            // calls handleInterrupt21() case 4: executeProgram()
            syscall(4, fileName, 0, 0);
        }

        else
        {
            // calls handleInterrupt21() case 0: printString()
            syscall(0, "Error: not a valid command.");
        }

        //Choose command to run

        // calls handleInterrupt21() case 0: printString()
        syscall(0, "\r\n");
    }
}