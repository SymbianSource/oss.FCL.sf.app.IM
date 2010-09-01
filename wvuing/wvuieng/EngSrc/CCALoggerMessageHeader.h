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
* Description:  Implementation for header information of history file
*
*/


#ifndef CCALOGGERMESSAGEHEADER_H
#define CCALOGGERMESSAGEHEADER_H

#include <e32std.h>

#include "MCALoggerMessageHeader.h"
#include "PublicEngineDefinitions.h"

// CLASS DECLARATION

/**
 *  Message header for history data
 *
 *  @lib CALogger.dll
 *  @since 2.1
 */
class CCALoggerMessageHeader : public CBase, public MCALoggerMessageHeader
    {
    public: // Constructors and destructors

        /**
         * NewL for CCALoggerMessageHeader
         * Two-phase constructor
         * @param aName Virtual log file name. Used in header of history file.
         * @param aId Identification information for header of history file.
         *				This is resolved by client of CCAHFMessage.
         *				If Group chat -> Groups name
         *				If i-messages -> identification of conversation party.
         * @param aChatType I-message or group chat. EPTOPChat or EGroupChat
         * @return Instance of CCALoggerMessageHeader with header information.
         */
        static CCALoggerMessageHeader* NewL(	const TDesC& aName,
                                             const TDesC& aId,
                                             TChatType aChatType );

        /**
         * NewL for CCALoggerMessageHeader
         * Two-phase constructor. For empty header creation
         * This does not call ConstructL
         * @return Instance of CCALoggerMessageHeader with header information.
         */
        static CCALoggerMessageHeader* NewL();

        /**
         * Virtual destructor
         */
        virtual ~CCALoggerMessageHeader();

    private:	// Constructors

        /**
         * Default constructor
         */
        CCALoggerMessageHeader();

        /**
         * ConstructL for CCALoggerMessageHeader
         * Part of Two-phase construction
         * @see CCALoggerMessageHeader::NewL for parameters
         * Last parameter is default EPTOPChat because empty
         * NewL creation. Default value can be anything so
         * there is not higher reason for choosing EPTOPChat
         */
        void ConstructL(	const TDesC& aName,
                         const TDesC& aId,
                         TChatType aChatType = EPTOPChat );

    public: // inherited from MCALoggerMessageHeader

        /**
         * Internalize the contact instance data.
         * Used by MCALoggerFactory.
         * @since 2.1
         * @see MCALoggerMessageHeader::InternalizeL
         */
        void InternalizeL( RReadStream& aStream );

        /**
         * Externalize the contact instance data.
         * Used by CCALoggerWriter
         * @since 2.1
         * @see MCALoggerMessageHeader::ExternalizeL
         */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * Disk space needed for saving message in externalize.
         * @see MCALoggerMessageHeader::MessageSizeInBytesL
         */
        TInt MessageSizeInBytesL() const;

        /**
         *	Get filename
         *	@since 2.1
         *	@return Real filename of history file.
         */
        const TDesC& FilenameL();

        /**
         *	Set filename
         *	@since 2.1
         *	@param aFilename Setter of real filename of history file
         */
        void SetFilenameL( const TDesC& aFilename );

        /**
         *	Triggers endtime of logging.
         *	@since 2.5
         */
        void EndLogging();

    public: //new methods

        /**
         *	Name of history data
         *	@return Name of history data
         */
        const TDesC& Name() const;

        /**
         *	Start time of history data
         *	@return start time of history data
         */
        const TTime& StartTime() const;

        /**
         *	ChatType of history data
         *	@return chattype of history data
         */
        TChatType ChatType() const;

        /**
         *	End time of history data
         *	@return time when history logging is off
         */
        const TTime& EndTime() const;

        /**
         *	Idenfication of history data
         *	@return identification information of history data.
         */
        const TDesC& Identification() const;


    private:  // Data

        //Real file name of history file. Owns.
        HBufC* iFilename;

        //Group id for header information. Owns.
        HBufC* iGroupId;

        //Name of history file Owns.
        HBufC* iName;

        //Start time
        TTime iStartTime;

        //End time
        TTime iEndTime;

        //Chat type. I-message or group EPTOPChat or EGroupChat
        TChatType iChatType;
    };

#endif      // CCALOGGERMESSAGEHEADER_H

// End of File
