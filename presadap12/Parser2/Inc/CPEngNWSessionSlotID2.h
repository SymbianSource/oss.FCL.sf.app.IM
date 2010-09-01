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
* Description:  NWSessionSlotID.
*
*/

#ifndef __CPENGNWSESSIONSLOTID2_H
#define __CPENGNWSESSIONSLOTID2_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS
class CPEngNWSessionSlotID2Imp;



// CLASS DECLARATION
/**
 * NWSessionSlotID.
 *
 * Class encapsulating the NWSessionSlotID.
 *
 * @since 3.0
 * @lib PEngManager2.lib
 */
class CPEngNWSessionSlotID2 : public CBase
    {

        //-----------------------------------------------------------------------
    public: /* Construction */

        /**
         * Instantiates NWSessionSlotID object.
         *
         * Initially all NWSessionSlotID sub parts are
         * zero length and no wild fields.
         *
         * @return New NWSessionSlotID instance.
         */
        IMPORT_C static CPEngNWSessionSlotID2* NewL();
        IMPORT_C static CPEngNWSessionSlotID2* NewLC();


        /**
         * Cloness CPEngNWSessionSlotID2 object.
         *
         * @return New CPEngNWSessionSlotID2 copy instance.
         */
        IMPORT_C CPEngNWSessionSlotID2* CloneL() const;


        /**
         * Destructor.
         * Virtual by CBase.
         */
        ~CPEngNWSessionSlotID2();


    private:

        CPEngNWSessionSlotID2();



        //-----------------------------------------------------------------------
    public: /* NWSessionSlotID */

        /**
         * Sets the NWSessionSlotID sub-part.
         *
         * All sub-parts must be properly defined before
         * the CPEngNWSessionSlotID2 instance can be used
         * to identify the NWSessionSlot.
         *
         * @param aAddress Service address to use.
         * @param aUserId User id to use.
         * @param aAppId AppId to use.
         */
        IMPORT_C void SetServiceAddressL( const TDesC& aAddress );
        IMPORT_C void SetUserIdL( const TDesC& aUserId );
        IMPORT_C void SetAppIdL( const TDesC& aAppId );
        IMPORT_C void SetAllL( const TDesC& aAddress,
                               const TDesC& aUserId,
                               const TDesC& aAppId );



        /**
         * Fragment access.
         *
         * Return pointer descriptor presenting
         * requested NWSessionSlotID sub-part.
         *
         * Pointed data remains valid untill new data
         * is assigned with any Set*() method or
         * NWSessionSlotID is reinitialized with UnPackL().
         *
         * @return Requested sub-part.
         */
        IMPORT_C TPtrC ServiceAddress() const;
        IMPORT_C TPtrC UserId() const;
        IMPORT_C TPtrC AppId() const;




        //-----------------------------------------------------------------------
    public: /* Match support */

        /**
         * Sets the NWSessionSlotID sub-part as wild.
         *
         * If NWSessionSlotID is wanted to be used as
         * match criteria when e.g. listing NWSessionSlotIDs,
         * one or more sub-parts can be defined as "wild".
         * Wild sub-part matches to any data in the matched
         * NWSessionSlotIDs corresponding sub-part.
         */
        IMPORT_C void SetServiceAddressMatchAnyL();
        IMPORT_C void SetUserIdMatchAnyL();
        IMPORT_C void SetAppIdMatchAnyL();


        /**
         * Tests is certain sub-part defined as wild.
         *
         * @return ETrue if requested sub-part is defined as wild.
         * Else EFalse.
         */
        IMPORT_C TBool IsServiceAddressWild() const;
        IMPORT_C TBool IsUserIdWild() const;
        IMPORT_C TBool IsAppIdWild() const;


        /**
         * Tests is one or more of the sub-parts defined as wild.
         *
         * @return ETrue if one or more sub-parts is defined as wild.
         * Else EFalse.
         */
        IMPORT_C TBool IsWild() const;


        /**
         * Pattern-matches NWSessionSlotIDs.
         *
         * Matching uses common pattern matching between
         * corresponding subparts. Any of sub-parts can be
         * defined as wild, in which case that sub part
         * matches to any data.
         *
         * @param aIdToMatch The NWSessionSlotID to match.
         * @return KErrNone if NWSessionSlotIDs match.
         * Else KErrNotFound.
         */
        IMPORT_C TInt MatchFullId( const CPEngNWSessionSlotID2& aIdToMatch ) const;



        /**
         * Pattern-matches NWSessionSlotIDs base parts.
         * (ServiceAddress and UserId).
         *
         * @param aIdToMatch The NWSessionSlotID which base part to match.
         * @return KErrNone if NWSessionSlotIDs match.
         * Else KErrNotFound.
         */
        IMPORT_C TInt MatchBasePart( const CPEngNWSessionSlotID2& aIdToMatch ) const;


        /**
         * Pattern-matches NWSessionSlotIDs AppId parts.
         * (AppId).
         *
         * @param aIdToMatch The NWSessionSlotID which AppId part to match.
         * @return KErrNone if NWSessionSlotIDs match.
         * Else KErrNotFound.
         */
        IMPORT_C TInt MatchAppIdPart( const CPEngNWSessionSlotID2& aIdToMatch ) const;



        //-----------------------------------------------------------------------
    public: /* Serialized presentation */



        /**
         * Packs NWSessionSlotIDs state to buffer and
         * returns buffer to caller. Caller is responsible
         * of deleting the buffer. PackLC() leaves the created
         * buffer to CleanupStack.
         *
         * @return NWSessionSlotID state buffer.
         */
        IMPORT_C HBufC8* PackL() const;
        IMPORT_C HBufC8* PackLC() const;


        /**
         * Initializes NWSessionSlotID state from a package
         * previously created with PackL() or PackLC().
         *
         * If given aPack isn't a valid NWSessionSlotID
         * package, leaves with KErrCorrupt.
         *
         * @param aPack NWSessionSlotID state package.
         */
        IMPORT_C void UnPackL( const TDesC8& aPack );



        //-----------------------------------------------------------------------
    public: /* Non exported access to implementation */

        /**
         * Presence Engine internal access
         * to NWSessionSlotID implementation.
         *
         * @return Implementation.
         */
        CPEngNWSessionSlotID2Imp* Implementation();
        const CPEngNWSessionSlotID2Imp* Implementation() const;



        //-----------------------------------------------------------------------
    private: /* Data */


        //OWN: Implementation
        CPEngNWSessionSlotID2Imp*     iImp;

    };


#endif //__CPENGNWSESSIONSLOTID2_H


//End of file




