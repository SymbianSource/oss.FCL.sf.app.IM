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
* Description:  Publish and subscribe handler for IMPSCommonUI
*
*/

#ifndef __CIMPSPUBSUBHANDLER_H
#define __CIMPSPUBSUBHANDLER_H

//  INCLUDES
#include <e32base.h>

#include "MIMPSSharedData.h"
#include "MIMPSPropertyNotificationObserver.h"
#include "IMPSSharedDataDefs.h"

//#include <e32property.h>    // RProperty

// FORWARD DECLARATIONS
class MIMPSSharedDataObserver;
class CIMPSPropertyObserver;

// CLASS DECLARATION

/**
*  Shared data handler
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.6
*/
NONSHARABLE_CLASS( CIMPSPubSubHandler ) : public CBase,
        public MIMPSSharedData,
        public MIMPSPropertyNotificationObserver
    {
public: // New functions
    /**
    * Two-phased constructor.
    * @param aResourceId the resource of the setting page
    * @param aSelectionItemList the selection item list of the setting page
    */
    static CIMPSPubSubHandler* NewL( MIMPSSharedDataObserver* aObserver, const TUid aUid );

private: // constructor and destructor

    /**
    * C++ default constructor.
    */
    CIMPSPubSubHandler( MIMPSSharedDataObserver* aObserver );

    /**
     * Symbian OS constructor.
     */
    void ConstructL( const TUid aUid );

    /**
    * Destructor.
    */
    virtual ~CIMPSPubSubHandler();


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
    * From MIMPSPropertyNotificationObserver
    * @see MIMPSPropertyNotificationObserver
    */
    void HandlePropertyNotificationEventL( TUid aCategory, TUint aKey );

    /**
    * From MSharedDataNotifyHandler
    * Converts shared data keys to TIMPSSharedKeys
    * @param aKey keyword
    * @return error code
    */
    TInt ConvertSharedDataKey( const TDesC& aSrc, TIMPSSharedKeys& aDest );

private: // new functions

    /**
    * Does the actual subscribing
    * @param aUid the uid of the property
    * @param aKey the key of the property
    */
    void DoSubscribeChangeL( const TUid aUid, const TIMPSSharedKeys aKey );

    /**
    * Does the actual subscribing
    * @param aUid the uid of the property
    * @param aKey the key of the property
    */
    void DoSubscribeSetL( const TUid aUid, const TIMPSSharedKeys aKey );

    /**
    * Does the actual signalling
    * @param aKey the key to signal
    */
    void DoSignalL( const TIMPSSharedKeys aKey );

    /**
    * Does the actual cancelling
    * @param aKey the key to cancel the signal from
    */
    void DoCancelSignalL( const TIMPSSharedKeys aKey );

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



private:    // Data
    // Doesn't own: observer for the shared data.
    MIMPSSharedDataObserver* iObserver;

    // identifier of the ini file we are interested in
    TUid iUid;

    // Owns: array of property observers
    RPointerArray<CIMPSPropertyObserver> iPropertyObservers;

    //Array of key pairs
    RArray<TIMPSSharedKeyPairs> iKeys;
    };

#endif      // __CIMPSPUBSUBHANDLER_H

// End of File
