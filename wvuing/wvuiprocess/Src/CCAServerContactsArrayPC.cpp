/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Array wrapper for chat lists
*
*/



// INCLUDE FILES
#include    "CCAServerContactsArrayPC.h"
#include    "CCAStorageManagerFactory.h"
#include	"ChatDebugPrint.h"
#include    "ChatDefinitions.h"
#include    "MCAStoredContacts.h"
#include    "MCAStoredContact.h"
#include    "CAUtils.h"
#include	"CCAPCUtils.h"

#include    <barsread.h>
#include    <eikenv.h>
#include 	<collate.h>

// Icon when contact already exist in phone
_LIT( KContactExistInPhoneIcon, "\t1" );

// local functions
TInt CCAServerContactsArrayCompare( const TCAServerContact& aFirst,
                                    const TCAServerContact& aSecond )
    {
    return aFirst.iIdentification.CompareC( aSecond.iIdentification,
                                            KCollationLevel, NULL );
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCAServerContactsArrayPC::CCAServerContactsArrayPC(
    TBool aShowContactExistInPhoneIcon )
        : iShowContactExistInPhoneIcon( aShowContactExistInPhoneIcon )
    {
    }


// common contruction part
void CCAServerContactsArrayPC::CommonConstructL( const TDesC& aIconTabulator )
    {
    iIconTabulator = aIconTabulator.AllocL();
    iLine = HBufC::NewL(
                KFriendsListMaxIdentificationLength + iIconTabulator->Length() );
    }


// Symbian OS default constructor can leave.
void CCAServerContactsArrayPC::ConstructL( CDesCArray* aNickNames,
                                           CDesCArray* aWVIds,
                                           const TDesC& aIconTabulator,
                                           CDesCArray* aIdentifications )
    {
    if ( ! aNickNames || ! aWVIds )
        {
        User::Leave( KErrArgument );
        }

    if ( aNickNames->MdcaCount() != aWVIds->MdcaCount() )
        {
        User::Leave( KErrArgument );
        }

    CommonConstructL( aIconTabulator );

    // create the identifications
    TInt count( aWVIds->Count() );

    TCAServerContact temp;

    MCAStoredContacts* storageContacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( aIdentifications )
            {
            if ( aIdentifications->MdcaPoint( i ).Length() > 0 )
                {
                // if the nickname is specified, display it
                temp.iIdentification.Set( aIdentifications->MdcaPoint( i ) );
                }

            }

        else
            {
            // otherwise display the wvid
            temp.iIdentification.Set( CCAPCUtils::DisplayId(
                                          aWVIds->MdcaPoint( i ) ) );
            }

        temp.iNickName.Set( aNickNames->MdcaPoint( i ) );
        temp.iWVID.Set( aWVIds->MdcaPoint( i ) );

        if ( iShowContactExistInPhoneIcon )
            {
            MCAStoredContact* contact =
                storageContacts->FindAnyContact( temp.iWVID );
            if ( contact )
                {
                temp.iExistInPhone = ETrue;
                }
            else
                {
                temp.iExistInPhone = EFalse;
                }
            }
        else
            {
            // not used in this case,
            // but just in case set some "default" value here
            temp.iExistInPhone = EFalse;
            }
        User::LeaveIfError( iServerContacts.Append( temp ) );
        }

    Sort();

    // Take the ownership here because we cannot leave anymore.
    iNickNames = aNickNames;
    if ( aIdentifications )
        {
        iIdentifications = aIdentifications;
        }
    iWVIds = aWVIds;
    }


// Symbian OS default constructor can leave.
void CCAServerContactsArrayPC::ConstructL(
    RPointerArray<MCAStoredContact>& aContacts,
    const TDesC& aIconTabulator )
    {
    CommonConstructL( aIconTabulator );

    iNickNames = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    iWVIds = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

    // create the identifications
    TInt count( aContacts.Count() );

    TCAServerContact temp;
    for ( TInt i( 0 ); i < count; ++i )
        {
        MCAStoredContact* contact = aContacts[ i ];

        TPtrC wvid( contact->UserId() );
        if ( wvid.Length() > 0 )
            {
            // contact has WVID
            TPtrC nick( contact->Identification() );

            iWVIds->AppendL( wvid );
            iNickNames->AppendL( nick );

            TInt pos( iWVIds->Count() - 1 );

            if ( nick.Length() > 0 )
                {
                // if the nickname is specified, display it
                temp.iIdentification.Set( iNickNames->MdcaPoint( pos ) );
                }
            else
                {
                // otherwise display the wvid
                temp.iIdentification.Set(
                    CCAPCUtils::DisplayId( iWVIds->MdcaPoint( pos ) ) );
                }

            temp.iNickName.Set( iNickNames->MdcaPoint( pos ) );
            temp.iWVID.Set( iWVIds->MdcaPoint( pos ) );
            temp.iExistInPhone = EFalse;
            }

        User::LeaveIfError( iServerContacts.Append( temp ) );
        }

    Sort();
    }


// Two-phased constructor.
CCAServerContactsArrayPC* CCAServerContactsArrayPC::NewL(
    CDesCArray* aNickNames,
    CDesCArray* aWVIds,
    const TDesC& aIconTabulator /* = KTab */,
    CDesCArray* aIdentifications /* = NULL */,
    TBool aShowContactExistInPhoneIcon /* = EFalse */
)
    {
    CCAServerContactsArrayPC* self =
        new( ELeave ) CCAServerContactsArrayPC( aShowContactExistInPhoneIcon );

    CleanupStack::PushL( self );
    self->ConstructL( aNickNames, aWVIds, aIconTabulator, aIdentifications );
    CleanupStack::Pop( self );

    return self;
    }

// Two-phased constructor.
CCAServerContactsArrayPC* CCAServerContactsArrayPC::NewL(
    RPointerArray<MCAStoredContact>& aContacts,
    const TDesC& aIconTabulator /*= KTab */,
    TBool aShowContactExistInPhoneIcon /*= EFalse*/ )
    {
    CCAServerContactsArrayPC* self =
        new( ELeave ) CCAServerContactsArrayPC( aShowContactExistInPhoneIcon );

    CleanupStack::PushL( self );
    self->ConstructL( aContacts, aIconTabulator );
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCAServerContactsArrayPC::~CCAServerContactsArrayPC()
    {
    iServerContacts.Close();
    delete iNickNames;
    delete iIdentifications;
    delete iWVIds;
    delete iLine;
    delete iIconTabulator;
    }

// ---------------------------------------------------------
// CCAServerContactsArray::MdcaCount() const
// Returns number of items in array
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAServerContactsArrayPC::MdcaCount() const
    {
    return iServerContacts.Count();
    }

// ---------------------------------------------------------
// CCAServerContactsArray::MdcaPoint()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPtrC CCAServerContactsArrayPC::MdcaPoint( TInt aIndex ) const
    {
    CHAT_DP( D_CHAT_LIT( "CCAServerContactsArray::MdcaPoint [%d]" ), aIndex );
    __ASSERT_DEBUG( aIndex >= 0 && aIndex < iServerContacts.Count(),
                    User::Panic( KPanicText, KErrArgument ) );


    TPtr data( iLine->Des() );

    data.Copy( ( *iIconTabulator ).Left( data.MaxLength() ) );
    data.Append( iServerContacts[ aIndex ].iIdentification );

    if ( iServerContacts[ aIndex ].iExistInPhone )
        {
        data.Append( KContactExistInPhoneIcon );
        }

    return iLine->Des();
    }


// ---------------------------------------------------------
// CCAServerContactsArray::Sort()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAServerContactsArrayPC::Sort()
    {
    TLinearOrder<TCAServerContact> linOrder( *CCAServerContactsArrayCompare );
    iServerContacts.Sort( linOrder );
    }


// ---------------------------------------------------------
// CCAServerContactsArray::AddL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAServerContactsArrayPC::AddL( const TDesC& aNickName, const TDesC& aWVId )
    {
    if ( Find( aWVId ) != KErrNotFound )
        {
        // contact with same wv id already exists
        return;
        }

    // get the copy of the descriptors
    iNickNames->AppendL( aNickName );
    iWVIds->AppendL( aWVId );
    TInt nickPos( iNickNames->MdcaCount() - 1 );
    TInt wvidPos( iWVIds->MdcaCount() - 1 );

    TCAServerContact temp;
    if ( aNickName.Length() > 0 )
        {
        // if the nickname is specified, display it
        temp.iIdentification.Set( iNickNames->MdcaPoint( nickPos ) );
        }
    else
        {
        // otherwise display the wvid
        temp.iIdentification.Set( CCAPCUtils::DisplayId(
                                      iWVIds->MdcaPoint( wvidPos ) ) );
        }

    temp.iNickName.Set( iNickNames->MdcaPoint( nickPos ) );
    temp.iWVID.Set( iWVIds->MdcaPoint( wvidPos ) );

    MCAStoredContacts* storageContacts =
        CCAStorageManagerFactory::ContactListInterfaceL();

    if ( iShowContactExistInPhoneIcon )
        {
        MCAStoredContact* contact =
            storageContacts->FindAnyContact( temp.iWVID );
        if ( contact )
            {
            temp.iExistInPhone = ETrue;
            }
        else
            {
            temp.iExistInPhone = EFalse;
            }
        }
    else
        {
        // not used in this case,
        // but just in case set some "default" value here
        temp.iExistInPhone = EFalse;
        }

    User::LeaveIfError( iServerContacts.Append( temp ) );
    Sort();
    }

// ---------------------------------------------------------
// CCAServerContactsArray::Remove()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAServerContactsArrayPC::Remove( TInt aIndex )
    {
    CHAT_DP( D_CHAT_LIT( "CCAServerContactsArray::Remove %d" ), aIndex );
    if ( ( aIndex >= 0 ) && ( aIndex < iServerContacts.Count() ) )
        {
        TInt pos;
        iWVIds->Find( iServerContacts[ aIndex ].iWVID, pos, ECmpCollated );
        iWVIds->Delete( pos );
        iWVIds->Compress();
        iNickNames->Delete( pos );
        iNickNames->Compress();
        iIdentifications->Delete( pos );
        iIdentifications->Compress( );
        iServerContacts.Remove( aIndex );
        iServerContacts.Compress();
        }
    }

// ---------------------------------------------------------
// CCAServerContactsArray::Find()
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCAServerContactsArrayPC::Find( const TDesC& aWVId ) const
    {
    TInt count( iServerContacts.Count() );
    CHAT_DP( D_CHAT_LIT( "CCAServerContactsArray::Find::%S" ), &aWVId );

    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( CAUtils::NeutralCompare( iServerContacts[i].iWVID, aWVId ) == 0 )
            {
            CHAT_DP( D_CHAT_LIT( "CCAServerContactsArray::Find::found!" ),
                     i, count );
            return i;
            }
        }
    CHAT_DP( D_CHAT_LIT( "CCAServerContactsArray::Find::not found!" ) );
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CCAServerContactsArray::WVIDArray()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const MDesCArray& CCAServerContactsArrayPC::WVIDArray() const
    {
    return *iWVIds;
    }

// ---------------------------------------------------------
// CCAServerContactsArray::WVIdL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TCAServerContact* CCAServerContactsArrayPC::WVIdL( TInt aIndex,
                                                         const TDesC& aFindBoxTxt /*= KNullDesC*/ ) const
// Simulate the findPane functionality when getting the iSelectedItem
// from the blockingArray structure
    {
    // check boundaries
    __ASSERT_ALWAYS( aIndex >= 0 || aIndex <= iServerContacts.Count(),
                     User::Leave( KErrArgument ) );

    if ( aFindBoxTxt == KNullDesC )
        {
        return &iServerContacts[ aIndex ];
        }
    else
        {
        //simulate the functionality of FindPane
        TInt count = iServerContacts.Count();
        TInt findBoxTxtLen = aFindBoxTxt.Length();
        TInt noOfFinds = -1;

        // Create custom collation method to ignore punctuations
        // index 0 gets the default method
        TCollationMethod collation =
            *Mem::CollationMethodByIndex( 0 );
        collation.iFlags |= TCollationMethod::EIgnoreNone;

        for ( TInt index = 0; index < count; index++ )
            {

            HBufC* toVerifyBuf;

            // Search the findPane text against the blocked items Nickname or Id
            if ( iServerContacts[ index ].iNickName.Length() > 0 )
                {
                // If the user has nick name
                toVerifyBuf = iServerContacts[ index ].iNickName.Left( findBoxTxtLen ).AllocLC();
                }
            else
                {
                // If not directly go to the user id
                toVerifyBuf = iServerContacts[ index ].iIdentification.Left( findBoxTxtLen ).AllocLC();
                }

            TInt idResult = toVerifyBuf->CompareC( aFindBoxTxt, KCollationLevel, &collation );

            // Popup and destory the the buffer
            CleanupStack::PopAndDestroy( toVerifyBuf );


            if ( idResult == 0 )
                {
                //there can be more than 1 item with the same findPanetext
                //so find the exact item which was selected in the UI
                noOfFinds++;

                if ( noOfFinds == aIndex )
                    {
                    //found the exact item
                    return 	&iServerContacts[ index ];
                    }
                }

            }
        }

    //cannot get here
    User::Leave( KErrArgument );
    return NULL;

    }

// ---------------------------------------------------------
// CCAServerContactsArray::NickNameArray()
// (other items were commented in a header).
// ---------------------------------------------------------
//
const MDesCArray& CCAServerContactsArrayPC::NickNameArray() const
    {
    return *iNickNames;
    }


// ---------------------------------------------------------
// CCAServerContactsArray::ReplaceL()
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCAServerContactsArrayPC::ReplaceL( CDesCArray* aNickNames,
                                         CDesCArray* aWVIds,
                                         const TDesC& aIconTabulator,
                                         CDesCArray* aIdentifications /* NULL */ )
    {
    iServerContacts.Close();
    delete iNickNames;
    iNickNames = NULL;
    delete iIdentifications;
    iIdentifications = NULL;
    delete iWVIds;
    iWVIds = NULL;
    delete iLine;
    iLine = NULL;
    delete iIconTabulator;
    iIconTabulator = NULL;

    ConstructL( aNickNames, aWVIds, aIconTabulator, aIdentifications );
    }

//  End of File
