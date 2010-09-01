/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Identification of unique CSP session
* 
*/


#ifndef CIMPSSESSIONIDENT_H
#define CIMPSSESSIONIDENT_H

//  INCLUDES
#include <e32base.h>
#include <s32file.h>


// CLASS DECLARATION


// Identification of unique CSP session.
// Notice that this class does not store any data and thus this
// can be used only for passing CSP identification in function calls.
// References to data members are temporary only and may not be valid
// after function call, and thus each class entity should make an own
// copy of data if it needs that data later on.
class TImpsSessionIdent
    {
    public:
        /**
        * Constructor
        * @param aSID CSP session id
        * @param aSAP SAP address
        * @param aUserId WV user id
        */
        IMPORT_C TImpsSessionIdent( const TDesC& aSID,
                                    const TDesC& aSAP, const TDesC& aUserId );

    public:  // New methods

        /**
        * Accessors
        */
        inline TPtrC SID() const;
        inline TPtrC SAP() const;
        inline TPtrC UserId() const;

        /**
        * check if all fields are zero length
        * @return ETrue if all is zero
        */
        inline TBool IsZero() const;

    private:    // Data
        const TDesC&           iSID;
        const TDesC&           iSAP;
        const TDesC&           iUserId;

    };


inline TPtrC TImpsSessionIdent::SID() const
    {
    return iSID;
    }

inline TPtrC TImpsSessionIdent::SAP() const
    {
    return iSAP;
    }

inline TPtrC TImpsSessionIdent::UserId() const
    {
    return iUserId;
    }

inline TBool TImpsSessionIdent::IsZero() const
    {
    return ( !iSAP.Length() && !iSID.Length() && !iUserId.Length() ) ? ETrue : EFalse;
    }


#endif      // ?INCLUDE_H   

// End of File

