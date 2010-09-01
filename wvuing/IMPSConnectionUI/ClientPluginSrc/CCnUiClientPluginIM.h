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
* Description:  IM client plug-in.
*
*/

#ifndef __CCNUICLIENTPLUGINIM_H
#define __CCNUICLIENTPLUGINIM_H

//  INCLUDES
#include <E32Base.h>
#include <f32file.h>
#include "CCnUiClientPluginBase.h"
#include "MIMPSSharedData.h"

// FORWARD DECLARATIONS
class CPEngNWSessionSlotID2;
class CPEngAttributeListStore2;
class MPEngAttributeList2;
class MPEngPresenceAttrModel2;
class RResourceFile;
class MIMPSSharedData;

// CLASS DECLARATION
/**
 * IM client plug-in.
 *
 * IM project has responsibility to maintain / change implementation
 * of this class as they need.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientPluginIM ) : public CCnUiClientPluginBase

    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiClientPluginIM* NewL( CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CCnUiClientPluginIM();

private:

    /**
     * C++ constructor.
     */
    CCnUiClientPluginIM();

    /**
     * Symbian OS constructor.
     */
    void ConstructL( CPEngNWSessionSlotID2& aNWSessionSlotID );



private: //From MCnUiClientPlugin

    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    TCnUiConnectionMode CurrentConnectionModeSettingL();


    /**
     * From MCnUiClientPlugin
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void ReWakeAutomaticConnectionModeL();


    /**
     * From MCnUiClientPlugin
     * Overloaded from CCnUiClientPluginBase
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void ConnectionOpenInitL( TBool aSapConnectionOpen, const CIMPSSAPSettings& aSap );

    /**
     * From MCnUiClientPlugin
     * Overloaded from CCnUiClientPluginBase
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void LoginCancelledL();

public: //Template methods from CCnUiClientPluginBase

    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    TBool DoVerifySapCapabilities( TPEngWVCspServicesTree2 aCurrenServices );


    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    CDesCArray* DoClientKnownCntListsLC();


    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    void DoStartExplicitPostLoginSyncL();

    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    void DoHandleExplicitPostLoginSyncStepCompleteL( TStepId aStepId, TInt& aStepStatus );


    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    void DoStartPreLogoutPublishL();


    /**
     * Template method from CCnUiClientPluginBase
     * @see CCnUiClientPluginBase
     * @since 2.1
     */
    void DoHandlePreLogoutPublishStepCompleteL( TStepId aStepId, TInt& aStepStatus );

private: // New helpers

    /**
     * Gets IM publish level value.
     *
     * @since 2.1
     * @return IM setting value for attribute publishing
     */
    static TInt IMAttributePublishLevelL();

    /**
     * Updates IM attribute-lists if needed
     * @param aStatus Request status
     * @since 2.1
     */
    void UpdateIMAttributeListsL( TInt aPublishLevel, MDesCArray& aContactLists );

    /**
     * Gets default attribute-list
     * @since 2.1
     * @return Default attribute-list
     */
    static MPEngAttributeList2* DefaultAttributeListLC(
        CPEngAttributeListStore2& aAttributeListFactory );


    /**
     * Read IM Attributes from resource and set them
     * @since 3.0
     * @param aLockedOwnAttributes
     *        On return, contains list of locked own
     *        attributes that should be published and unlocked.
     */
    void ReadIMAttributesL( RPointerArray<MPEngPresenceAttrModel2>& aLockedOwnAttributes );

    /**
     * Update local IM Attributes as given und issues NWE publish if needed
     * @since 2.1
     * @param aStatus Request status
     */
    void IssueIMAttributeUpdateAndPublishIfNeededL( TInt aPublishLevel );

    /**
     * Opens correct resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.0
     */
    void OpenResourceFileLC( RResourceFile& aResourceFile );



    //Ease of IM Branding
    /**
     * Opens correct variation resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.2
     */
    void OpenVariationResourceFileLC( RResourceFile& aResourceFile );


    /**
     * Resource reader for non-Cone situations
     * @param aResourceId The integer-valued resource id to read
     * @return The value of the integer
     */
    TInt ReadResourceIntValueL( TInt aResourceId );

    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<MPEngPresenceAttrModel2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

    /**
    * Sets presence authorization to all
    * @since 3.0
    */
    void SetAuthorizationToAllL( MDesCArray& aContactLists );

    /**
    * Sets presence authorization to none
    * @since 3.0
    */
    void SetAuthorizationToNoneL( MDesCArray& aContactLists );

    /**
    * Sets presence authorization to friends
    * @since 3.0
    */
    void SetAuthorizationToFriendsL(
        MDesCArray& aContactLists );

    /**
    * Attaches attribute list to contact lists
    * @since 3.0
    */
    void AttachAttributeListL(
        MDesCArray& aContactLists,
        MPEngAttributeList2& aAttributeList );

    /**
    * Detaches attribute list from contact lists
    * @since 3.0
    */
    void DetachAttributeListL(  MDesCArray& aContactLists );

    /**
     * Get first status message for online state.
     * @return first status message from saved ones.
     *          If not found, empty descriptor returned.
     *          Ownership is transferred to caller.
     * @since 3.0
     */
    HBufC* GetFirstStatusMessageL() const;

    /**
     * Get sharedData attribute value.
     * @return Shared data string.
     *         If not found, empty descriptor returned.
     *         Ownership is transferred to caller.
     * @since 3.0
     */
    HBufC* GetSharedDataL( TIMPSSharedKeys aKey ) const;

    /**
     * Finds alias for given aUserID inside aAliasTable.
     * @param aAliasTable string containeing all aliases for different ids
     * @param aUserId Userid to look for
     * @param aIndex Position inside aAliasTable, where the wanted alias starts.
     *               Or KErrNotFound
     * @param aLength aLength length of alias
     */
    void LocateAliasL( const TDesC& aAliasTable, const TDesC& aUserId,
                       TInt& aIndex, TInt& aLength );

private: // data

    // ETrue if reactive authorization is used
    TBool iRAUsed;

    // ETrue if alias attribute is supported
    TBool iAliasUsed;

    RFs   iRFs;

    // ETrue if attribute lists are supported
    TBool iAttribListsSupported;

    // Presence authorization setting value
    TInt iPresenceAuth;

    // own. cached alias value
    HBufC* iAlias;

    //owns
    MIMPSSharedData*	iSharedData;
    };

#endif      //__CCNUICLIENTPLUGINIM_H
//  End of File


