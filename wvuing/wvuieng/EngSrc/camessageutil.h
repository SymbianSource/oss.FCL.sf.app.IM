/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  General methods for message handling
*
*/


#ifndef CAMESSAGEUTIL_H
#define CAMESSAGEUTIL_H

// FORWARD DECLARATIONS
class TTime;
class MCAMessage;
class MCAMessageCreator;
class MCAMessagesWriteInterface;
class MCASettings;

/**
 *  General message handling methods.
 *
 *  General message handling methods which are to be used in both
 *  messagehandler and chatgroupmanager, to avoid duplicate code.
 *
 *  @lib CAEngine.dll
 *  @since S60 v3.1
 */
class CAMessageUtil // CSI: 51 # This is not a C-class although it begins with CA
    {
    public:

        /**
         * Appends a datestamp to writeinterface if needed.
         * New time is compared to the previous message in the interface.
         * If the time values are from different date then datestamp is created
         * Otherwise this does nothing.
         *
         * @since S60 v3.1
         * @param aNewTime Time of the new message
         * @param aWriteInterface Interface to write the date message
         * @param aMessageCreator for creating the date message
         * @param aSettingsAPI Settings interface to determine if datestamps
         *                     are required.
         */
        static void AppendDateStampL( TTime aNewTime,
                                      MCAMessagesWriteInterface& aWriteInterface,
                                      MCAMessageCreator& aMessageCreator );


        /**
         * Appends a datestamp and message to interface. Datestamp is added only
         * if the date has changed between previous and current message.
         * Ownership of the message is transferred or shared, so it doesn't
         * need to be in cleanupstack.
         *
         * @since S60 v3.1
         * @param aNewMessage New message
         * @param aWriteInterface Interface to write the date message
         * @param aMessageCreator for creating the date message
         * @param aSettingsAPI Settings interface to determine if datestamps
         *                     are required.
         * @param aSharedOwnership EFalse - ownership is transferred
         *                         ETrue - ownership is shared
         */
        static void AppendMessageWithDateStampL(
            MCAMessage& aNewMessage,
            MCAMessagesWriteInterface& aWriteInterface,
            MCAMessageCreator& aMessageCreator,
            TBool aSharedOwnership = EFalse );

    private:

        /**
         * C++ constructor & destructor
         */
        CAMessageUtil();
        ~CAMessageUtil();
    };


#endif // CAMESSAGEUTIL_H
