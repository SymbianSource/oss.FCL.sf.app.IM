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
* Description:  Generic event and message channel for Connection UI.
*
*/



#ifndef __CCNUIGROUPCHANNEL_H
#define __CCNUIGROUPCHANNEL_H

// INCLUDES
#include "MIMPSSharedData.h"
#include "MIMPSSharedDataObserver.h"
#include "IMPSSharedDataDefs.h"

#include <E32Base.h>
#include <impspresenceconnectionuiconstsng.h>

//CONTANTS
_LIT( KGroupChannelKeyNameFormatter, "MC%dG%d" );
_LIT( KGroupChannelDataKeyNameFormatter, "DC%dG%d" );

_LIT( KGlobalChannelKeyNameFormatter, "GMC%d" );
_LIT( KGlobalChannelDataKeyNameFormatter, "GDC%d" );

_LIT( KGlobalChannelPermanentKeyIdentifier, "P" );
_LIT( KGlobalChannelTemporaryKeyIdentifier, "T" );

//Identifier is inserted in the start of key ==> index 0
const TInt KGlobalChannelTypeIdPosition = 0;


const TInt KMaxChannelKeyLength = 10;
const TInt KMaxChannelIdValue   = 10;



// FORWARD DECLARATIONS
class MCnUiGroupChannelListener;


/**
 * Notification group id for global (client group wide)
 * notifications
 * @since 2.1
 */
enum TGCSpecialGroupIds
    {
    ECnUiGlobalGroup = EIMPSConnClientPEC - 1,
    };


/**
 * Notification channel ID's
 * @since 2.1
 */
enum TGCChannelID
    {
    ECnUiClientLoginLogoutStateChannel = 0,
    ECnUiLoginLogoutEventChannel = 1,
    ECnUiSSClientReqistrationChannel = 2,
    ECnUiGlobalOperationSignalChannel = 3,
    ECnUiRemoteUiNotificationsChannel = 4
    };



// CLASS DECLARATION

/**
 * Generic event and message channel for Connection UI.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiGroupChannel ) : public CBase,
        public MIMPSSharedDataObserver
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor, constructs the
     * CCnUiGroupChannel object.
     *
     * @since 2.1
     * @param aGroupId The channel group ID.
     * @param aChannelId The channel ID.
     * @param aTemporary Is the channel temporary or permanent.
     * @return is pointer to CCnUiGroupChannel object.
     */
    static CCnUiGroupChannel* NewLC( TInt aGroupId,
    TGCChannelID aChannelId,
    TBool aTemporary = ETrue );


    /**
     * Two-phased constructor, constructs the
     * CCnUiGroupChannel object.
     *
     * @since 2.1
     * @param aGroupId The channel group ID.
     * @param aChannelId The channel ID.
     * @param aTemporary Is the channel temporary or permanent.
     * @return is pointer to CCnUiGroupChannel object.
     */
    static CCnUiGroupChannel* NewL( TInt aGroupId,
                                    TGCChannelID aChannelId,
                                    TBool aTemporary = ETrue );


    /**
     * Destructor.
     */
    ~CCnUiGroupChannel();


private: //Constructors

    /**
     * C++ default constructor.
     */
    CCnUiGroupChannel( TInt aGroupId,
                       TGCChannelID aChannelId );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TBool aTemporary );


public: //New methods

    /**
     * Writes the given message to opened channel.
     *
     * @since 2.1
     * @param aMsg The message to write.
     */
    void WriteL( TInt aMsg );

    /**
    * Writes the given message to opened channel.
    * Writes the given extra data channel data subchannel.
    *
    * @since 2.1
    * @param aMsg The message to write.
    * @param aExtraData The extra data to write.
     */
    void WriteL( TInt aMsg, const TDesC& aExtraData );

    /**
     * Reads a last message from channel.
     *
     * @since 2.1
     * @param aMsg On the return contains the readed message.
     * @return KErrNone if succesful.
     * KErrNotFound if no data previously stored to channel,
     * or it wasn't in numeric format.
     */
    TInt Read( TInt& aMsg );


    /**
     * Reads a last extra data from channel.
     *
     * @since 2.1
     * @param aExtraData On the return contains the readed message.
     * If no data previously stored to channel, returns empty
     * buffer.
     */
    void ReadL( HBufC*& aExtraData );


    /**
     * Signals the channel.
     * Increases the channel value with 1.
     *
     * @since 2.1
     * @return KErrNone if succesful.
     * KErrInUse if channel is already signalled by this object.
     */
    TInt SignalL();


    /**
     * Signals the channel.
     * Increases the channel value with 1.
     * Updates also the data subchannel value to given one.
     *
     * @since 2.1
     * @param aExtraData The extra data to store to data sub channel.
     * @return KErrNone if succesful.
     * KErrInUse if channel is already signalled by this object.
     */
    TInt SignalL( const TDesC& aExtraData );


    /**
     * Cancels any previously by this object issued signal.
     * @since 2.1
     */
    void CancelSignal();


    /**
     * Registeres to listen channel events.
     * @since 2.1
     * @param aListener The listener to notify channel events.
     * Leaves with KErrInUse, if listening already started.
     * Leaves with KErrArgument, if listener is NULL.
     */
    void ListenL( MCnUiGroupChannelListener* aListener );


    /**
     * Cancels any previously issued listen.
     * @since 2.1
     */
    void CancelListen();


    /**
     * Gets channel group ID.
     * @since 2.1
     * @return Channel group ID.
     */
    TInt GroupID();


    /**
     * Gets channel channel ID.
     * @since 2.1
     * @return Channel ID.
     */
    TGCChannelID ChannelId();

private: //helpers

    void WriteExtraDataL( const TDesC& aExtraData );

    /**
    * Handles notify from permanent keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    void HandlePermanentKeyNotifyL( const TUid aUid,
                                    const TIMPSSharedKeys aKey );

    /**
    * Handles notify from temporary keys' changes
    * @param aUid uid of the dll
    * @param aKey the shared data key
    * @param aValue the value of the key
    */
    void HandleTemporaryKeyNotifyL( const TUid aUid,
                                    const TIMPSSharedKeys aKey );


private: //data

    ///<The channel ID's, owned
    const TInt                              iGroupId;
    const TGCChannelID                      iChannelId;

    ///<Channel & sub channel name keys, owned
    TBuf<KMaxChannelKeyLength>              iChannelKey;
    TBuf<KMaxChannelKeyLength>              iChannelDataKey;

    ///<Registered listener, not owned.
    MCnUiGroupChannelListener*              iListener;

    ///<Signal flag, owned
    TBool                                   iChannelSignaled;

    // the converted channel key
    TIMPSSharedKeys							iKey;

    // the converted channel data key
    TIMPSSharedKeys							iDataKey;

    //Array of key pairs
    RArray<TIMPSSharedKeyPairs> iKeys;

    // Owns: the shared data handler
    MIMPSSharedData *iSharedData;

    // UID
    TUid 									iUid;

    };




// CLASS DECLARATION

/**
 * Listener interface to listen channel events.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiGroupChannelListener )
    {
public: // Handle events

    /**
     * Callback method to be called after channel event.
     *
     * @since 2.1
     * @param aGroupId   The originating channel group.
     * @param aChannelId The originating channel.
     * @param aChannelMsg The channel message.
     */
    virtual void HandleChannelMsg( TInt aGroupId,
                                   TGCChannelID aChannelId,
                                   TInt aChannelMsg ) = 0;


protected:  //Destructor

    /**
     * Virtual inline destructor.
     * Observer can't be destructed using this interface.
     *
     * @since 2.1
     */
    virtual ~MCnUiGroupChannelListener() {};

    };


#endif      //__CCNUIGROUPCHANNEL_H

//  End of File
