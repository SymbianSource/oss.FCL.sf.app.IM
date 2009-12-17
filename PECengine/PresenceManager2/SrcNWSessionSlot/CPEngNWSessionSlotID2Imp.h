/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  NWSessionSlotID implementation.
*
*/

#ifndef CPENGNWSESSIONSLOTID2IMP_H
#define CPENGNWSESSIONSLOTID2IMP_H

//  INCLUDES
#include <E32Base.h>



// CONSTANTS
_LIT( KPEngMatchSymbolWildAny, "*" );
enum TPEngNWSSIDMatchMode
    {
    EPEngMMExact = 0,
    EPEngMMWildAny = 1
    };


// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CPEngSessionSlotId;



/**
 * NWSessionSlotID implementation.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngNWSessionSlotID2Imp ) : public CBase
    {
public: /* Construction */


    /**
     * Instantiates CPEngNWSessionSlotID2Imp object.
     *
     * @return New CPEngNWSessionSlotID2Imp instance.
     */
    static CPEngNWSessionSlotID2Imp* NewL();


    /**
     * Cloness CPEngNWSessionSlotID2Imp object.
     *
     * @return New CPEngNWSessionSlotID2Imp copy instance.
     */
    CPEngNWSessionSlotID2Imp* CloneL() const;


    /**
     * Reset.
     */
    void Reset();


    /**
     * Destructor.
     */
    ~CPEngNWSessionSlotID2Imp();



protected:

    /**
     * C++ constructor.
     */
    CPEngNWSessionSlotID2Imp();

    /**
     * CSymbian OS constructor..
     */
    void ConstructL();


    //-----------------------------------------------------------------------
public: /* NWSessionSlotID implementation */

    /**
     * Sets the NWSessionSlotID sub-part.
     *
     * @param aAddress Service address to use.
     * @param aUserId User id to use.
     * @param aAppId AppId to use.
     * @param aMatchMode The match mode for sub-part.
     */
    void SetServiceAddressL( const TDesC& aAddress, TPEngNWSSIDMatchMode aMatchMode );
    void SetUserIdL( const TDesC& aUserId, TPEngNWSSIDMatchMode aMatchMode );
    void SetAppIdL( const TDesC& aAppId, TPEngNWSSIDMatchMode aMatchMode );
    void SetAllL( const CPEngSessionSlotId& aBasePart, const TDesC& aAppId );


    /**
     * Fragment access.
     *
     * Return pointer descriptor presenting
     * requested NWSessionSlotID sub-part.
     *
     * @return Requested sub-part.
     */
    TPtrC ServiceAddress() const;
    TPtrC UserId() const;
    TPtrC AppId() const;


    /**
     * Base part access
     */
    const CPEngSessionSlotId& BasePart() const;





    //-----------------------------------------------------------------------
public: /* Match support */

    /**
     * Tests is certain sub-part defined as wild.
     *
     * @return ETrue if requested sub-part is defined as wild.
     * Else EFalse.
     */
    TBool IsServiceAddressWild() const;
    TBool IsUserIdWild() const;
    TBool IsAppIdWild() const;
    TBool IsWild() const;


    /**
     * Tests is all NWSessionSlotID sub-parts defined.
     *
     * @return If any of sub parts isn't defined or
     * is defined as wild, returns EFalse. Else ETrue.
     */
    TBool IsFullyDefined() const;


    /**
     * Pattern-matches NWSessionSlotIDs.
     *
     * @param aIdToMatch The NWSessionSlotID to match.
     * @return KErrNone if NWSessionSlotIDs match.
     * Else KErrNotFound.
     */
    TInt MatchFullId( const CPEngNWSessionSlotID2Imp& aIdToMatch ) const;


    /**
     * Pattern-matches NWSessionSlotIDs base parts.
     * (ServiceAddress and UserId).
     *
     * @param aIdToMatch The NWSessionSlotID which base part to match.
     * @return KErrNone if NWSessionSlotIDs match.
     * Else KErrNotFound.
     */
    TInt MatchBasePart( const CPEngNWSessionSlotID2Imp& aIdToMatch ) const;
    TInt MatchBasePart( const CPEngSessionSlotId& aBasePartToMatch ) const;



    /**
     * Pattern-matches NWSessionSlotIDs AppId parts.
     * (AppId).
     *
     * @param aIdToMatch The NWSessionSlotID which AppId part to match.
     * @return KErrNone if NWSessionSlotIDs match.
     * Else KErrNotFound.
     */
    TInt MatchAppIdPart( const CPEngNWSessionSlotID2Imp& aIdToMatch ) const;
    TInt MatchAppIdPart( const TDesC& aAppId ) const;

    //-----------------------------------------------------------------------
public: /* Serialized presentation */

    /**
     * Packs NWSessionSlotIDs state to buffer and
     * returns buffer to caller.
     * @return NWSessionSlotID state buffer.
     */
    HBufC8* PackFullLC() const;


    /**
     * Initializes NWSessionSlotID state from a package
     * previously created with PackFullLC().
     *
     * @param aPack NWSessionSlotID state package.
     */
    void UnPackFullL( const TDesC8& aPack );


    /**
     * Externalizes the data.
     *
     * @param aStream Stream where to write.
     */
    void ExternalizeL( RWriteStream& aStream ) const;


    /**
     * Internalizes the data.
     *
     * @param aStream Stream where to read from.
     */
    void InternalizeL( RReadStream& aStream );



private: //Data

    //OWN: NWSessionSlotID sub-parts
    CPEngSessionSlotId* iBasePart;
    HBufC* iAppId;

    //OWN: NWSessionSlotID part wild definitions
    TInt iServiceAddressMatchMode;
    TInt iUserIdMatchMode;
    TInt iAppIdMatchMode;

    };

#endif  //CPENGNWSESSIONSLOTID2IMP_H

// End of File
