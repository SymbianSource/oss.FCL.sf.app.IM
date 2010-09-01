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
* Description:  Shared data handler for IMPSCommonUI
*
*/

#ifndef __CWVSETTINGSUISHAREDDATAHANDLER_H
#define __CWVSETTINGSUISHAREDDATAHANDLER_H

//  INCLUDES
#include "MIMPSSharedData.h"
#include "IMPSSharedDataDefs.h"

#include <e32base.h>
#include <SharedDataClient.h>


// FORWARD DECLARATIONS
class MIMPSSharedDataObserver;

// CLASS DECLARATION

/**
*  Shared data handler
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( CIMPSSharedDataHandler ) : public CBase,
        public MIMPSSharedData,
        public MSharedDataNotifyHandler
    {
public: // New functions
    /**
    * Two-phased constructor.
    * @param aResourceId the resource of the setting page
    * @param aSelectionItemList the selection item list of the setting page
    */
    static CIMPSSharedDataHandler* NewL( MIMPSSharedDataObserver* aObserver,
    const TUid aUid,
    TBool aTemporary = EFalse );

private: // constructor and destructor

    /**
    * C++ default constructor.
    */
    CIMPSSharedDataHandler( MIMPSSharedDataObserver* aObserver,
                            const TUid aUid,
                            TBool aTemporary );

    /**
     * Symbian OS constructor.
     */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual ~CIMPSSharedDataHandler();


private: //Functions from base classes

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt SubscribeChange( const TUid aUid, const TIMPSSharedKeys aKey );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt SubscribeSet( const TUid aUid, const TIMPSSharedKeys aKey );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    void UnSubscribe( const TUid aUid, const TIMPSSharedKeys aKey );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt GetStringKey( const TIMPSSharedKeys aKey, TDes& aValue );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt GetIntKey( const TIMPSSharedKeys aKey, TInt& aValue );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt SetStringKey( const TIMPSSharedKeys aKey, const TDesC& aValue );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt SetIntKey( const TIMPSSharedKeys aKey, TInt aValue );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt Signal( const TIMPSSharedKeys aKey );

    /**
    * From MIMPSSharedData
    * @see MIMPSSharedData
    */
    TInt CancelSignal( const TIMPSSharedKeys aKey );


    /**
    * From MSharedDataNotifyHandler
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    void HandleNotifyL( const TUid aUid,
                        const TDesC& aKey,
                        const TDesC& aValue );

    /**
    * From MSharedDataNotifyHandler
    * Converts shared data keys to TIMPSSharedKeys
    * @param aKey keyword
    * @return error code
    */
    TInt ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest );

private: // new functions

    /**
    * Does the actual setting of the key
    * @see MIMPSSharedData
    */
    void DoSetIntKeyL( const TIMPSSharedKeys aKey, TInt aValue );

    /**
    * Converts the TIMPSSharedKeys to shared data keys
    * @param aKey the key from the enumeration that clients use
    * @param aSharedDataKey the shared data key
    * @return KErrNone if found, KErrNotFound if not found
    */
    TInt MapKeysToSharedData( const TIMPSSharedKeys aKey, TDes& aSharedDataKey );

    /**
    * Converts the shared data keys to TIMPSSharedKeys
    * @param aKey the key from the enumeration that clients use
    * @param aSharedDataKey the shared data key
    * @return KErrNone if found, KErrNotFound if not found
    */
    TInt MapKeysToClient( TIMPSSharedKeys& aKey, const TDesC& aSharedDataKey );

    /**
    * Creates the array of key mappings
    */
    void AppendKeyPairsL( );

    /**
    * Appends a key pair into the array of keys
    * @param aKey the key from the enumeration that clients use
    * @param aSharedDataKey the shared data key
    */
    void DoAppendKeyPairL( TIMPSSharedKeys aKey, const TDesC& aSharedDataKey );

    /**
    * Does the actual signalling
    * @param aKey the key to signal
    */
    void DoSignalL( const TIMPSSharedKeys aKey );

    /**
    * Does the actual cancelling
    * @param aKey the key to signal
    */
    void DoCancelSignalL( const TIMPSSharedKeys aKey );

private:    // Data
    // Doesn't own: observer for the shared data.
    MIMPSSharedDataObserver* iObserver;

    ///Shared data client to read common settings, owned
    RSharedDataClient    iSharedDataClient;

    // identifier of the ini file we are interested in
    TUid iUid;

    //Array of key pairs
    RArray<TIMPSSharedKeyPairs> iKeys;

    // tells if we should use temporary keys
    TBool iTemporary;
    };

#endif      // __CWVSETTINGSUISHAREDDATAHANDLER_H

// End of File
