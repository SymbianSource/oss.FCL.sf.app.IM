/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message containing text
*
*/


#ifndef CCATEXTMESSAGE_H
#define CCATEXTMESSAGE_H

#include <e32base.h>

#include "CCAMessageBase.h"


// CLASS DECLARATION

/**
 *  Message containing text
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class CCATextMessage : public CCAMessageBase
    {

    public: // Construction

        /**
         * Construction
         * @return Pointer to new created instance of CCATextMessage
         */
        static CCATextMessage* NewL( TInt aOpCode,
                                     const TDesC& aSender,
                                     const TDesC& aRecipient,
                                     const MDesCArray* aRecipients,
                                     const MDesCArray* aScreenNames,
                                     const TDesC& aData );


        /**
         * Construction. This is only skeleton creation. Used with internalize
         * @return Pointer to new created instance of CCATextMessage
         */
        static CCATextMessage* NewL();


        /**
         * Construction
         * @return Pointer to new created instance of CCATextMessage
         */
        static CCATextMessage* NewLC( TInt aOpCode,
                                      const TDesC& aSender,
                                      const TDesC& aRecipient,
                                      const MDesCArray* aRecipients,
                                      const MDesCArray* aScreenNames,
                                      const TDesC& aData );

        /**
         * Destruction
         */
        ~CCATextMessage();

    private: // Construction

        /**
         * Constructor
         */
        CCATextMessage();

        /**
         * Constructor
         */
        void ConstructL( const TDesC& aData );

    private: // From MCAMessage

        /**
         * @see MCAMessage
         */
        TInt SizeInBytes() const;

        /**
         * @see MCAMessage
         */
        TContentType ContentType() const;

    public: // New methods

        /**
         * Set this message as failed. Change content to show failed status.
         */
        void SetFailedL();

    private: // Member variables

        const HBufC* iForwardPart;
    };

#endif      // CCATEXTMESSAGE_H

// End of File
