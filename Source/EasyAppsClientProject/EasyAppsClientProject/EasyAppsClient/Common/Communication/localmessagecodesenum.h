#ifndef LOCALMESSAGECODESENUM_H
#define LOCALMESSAGECODESENUM_H

typedef enum {
    FINISHED_INSTALLATION = 2,
    FAILED_TO_INSTALL_APPLICATION = 3,
    RUNNING_COPY_DETECTED = 10,
    STILL_PROCESSING = 11, // it's a status message, means: "I'm still busy working."

    STRING_MESSAGE = 20, // this will be the first part of the message, then the second part is a string
    JSON_MESSAGE = 21
} CommunicationMessageCodes;

#define MESSAGE_TEXT_TYPE_ANOTHER_INSTANCE_PROGRAM_ARGS "AnotherInstanceProgramArgs:"

#endif // LOCALMESSAGECODESENUM_H
