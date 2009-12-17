/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Offers information regarding a given header to the UI.
*
*/


#ifndef CCALOGGERHEADERSINTERFACE_H
#define CCALOGGERHEADERSINTERFACE_H

// INCLUDES
#include    "PublicEngineDefinitions.h"
#include	"MCALoggerHeadersInterface.h"

#include    <e32std.h>
#include	<e32base.h>

// FORWARD DECLARATIONS
class MCALoggerReadInterface;
class MCALoggerMessageHeader;
class MCALoggerMessage;
class RWriteStream;
class CCALoggerMessageHeader;
class MCAMessage;
class MCAChatInterface;
class MCASettings;

// CLASS DECLARATION

/**
*  Interface class for UI->Engine communication ( UI retrieves information ).
*  Offers information regarding a given header to the UI.
*
*  @lib CAEngine.lib
*  @since 2.5
*/
class CCALoggerHeadersInterface :   public CBase,
            public MCALoggerHeadersInterface
//                                    public MCAMessagesInterface
    {
    public:  // Constructors and destructor

        /**
         *	Two-phased constructor.
         *	@return Instance to created CCALoggerHeadersInterface
         */
        static CCALoggerHeadersInterface* NewL(
            MCAChatInterface& aChatInterface,
            MCASettings* aSettingsAPI );

        /**
         * Destructor.
         */
        virtual ~CCALoggerHeadersInterface();

    private:

        /**
         *	Part of two-phased construction.
         *  Leaves with KErrNotFound, if iHeaderArray is not filled with valid
         *  pointer.
         */
        void ConstructL();

        /**
         *	C++ default constructor.
         */
        CCALoggerHeadersInterface( MCAChatInterface& aChatInterface,
                                   MCASettings* aSettingsAPI );

    private: // New helper methods

        /**
         * Get recorded chats header.
         * @param aIndex. Returned headers index in array.
         * @return Pointer to header in index.
         * @since 2.5
         */
        CCALoggerMessageHeader* LoggerMessageHeader( TInt aIndex ) const;

        /**
         * TCleanupItem callback function for message array
         * used in MessagesInterfaceL
         * @since 3.1
         */
        static void ResetAndDestroyMessages( TAny* aObject );

    public: //Inherited from MCALoggerHeadersInterface

        /**
        * @see MCALoggerHeadersInterface.h
        * Returns the number data items ( headers count ).
        * @since 2.5
        * @return Number of data items.
        */
        TInt HeaderCount() const;

        /**
        * @see MCALoggerHeadersInterface.h
        * Returns pointer to the message identified by the parameter.
        * @param aIndex is the wanted data item index.
        * @return Reference to the IM message.
        * @since 2.5
        */
        const TDesC& Name( TInt aIndex ) const;

        /**
        * @see MCALoggerHeadersInterface.h
        * Returns the type of the chat data.
        * @param aIndex is the index to the header array.
        * @return Message type.
        * @since 2.5
        */
        TChatType ChatType( TInt aIndex ) const;

        /**
        * @see MCALoggerHeadersInterface.h
        * Returns time when history save started.
        * @param aIndex is the index to the header array.
        * @return Start time
        * @since 2.5
        */
        TTime StartTime( TInt aIndex ) const;

        /**
        * @see MCALoggerHeadersInterface.h
        * Returns time when history save ended.
        * @param aIndex is the header index.
        * @return End time.
        * @since 2.5
        */
        TTime EndTime( TInt aIndex ) const;

        /**
         * @see MCALoggerHeadersInterface.h
         * Returns idenfification string for history data.
         * @since 2.5
         */
        const TDesC& Identification( TInt aIndex ) const;

        /**
         *	Delete one history file recognized by index
         *	@see MCALoggerHeadersInterface
         *	Leaves if file from index does not exists with KErrNotFound
         *  @param aIndex Index of recorded chat to delete.
         *  @since 2.5
         */
        void DeleteHistoryFileL( TInt aIndex );

        /**
         * Read message for sending it via different MTMs.
         * @see MCALoggerHeadersInterface
         * @since 2.5
         */
        void GetMessageForSendingViaMTML( TInt aIndex, RWriteStream& aStream );

        /**
         * Get interface to access messages in one recorded chat.
         * @see MCALoggerHeadersInterface
         * @since 2.5
         */
        MCAMessagesReadInterface& MessagesInterfaceL( TInt aIndex );

        /**
         * @see MCALoggerHeadersInterface
         * @since 3.1
         */
        void DeleteRecordedChatContainerL();

        /**
         * @see MCALoggerHeadersInterface
         * @since S60 v3.2
         */
        TInt Index( const TDesC& aName ) const;


        //fix - refer Ui Spec Approved Version 1.0
        //(Instant Messaging NG 001 151006.pdf)
        //Section 10.2.10 Pg 131 -
        //"In case user has saved the image already or sent it
        //by himself, this option(save) is not available."

        /**
         * Added newly - Plz dont remove while merging
         * @see MCALoggerHeadersInterface
         * @since S60 v3.2
         */
        void ReLoggingL( TInt aIndex );

    private: // Member variables

        /// Access to containers.
        MCAChatInterface& iChatInterface;

        // Not own. Read interface to access history data
        MCALoggerReadInterface* iLogger;

        // Not own. Array of headers.
        RPointerArray<MCALoggerMessageHeader> *iHeaderArray;

        // Not own. Setting interface
        MCASettings* iSettingsAPI;
    };

#endif      // CCALOGGERHEADERSINTERFACE_H

// End of File
