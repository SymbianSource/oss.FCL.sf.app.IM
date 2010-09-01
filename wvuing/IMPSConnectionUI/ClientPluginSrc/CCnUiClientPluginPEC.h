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
* Description:  PEC client plugin.
*
*/

#ifndef __CCNUICLIENTPLUGINPEC_H
#define __CCNUICLIENTPLUGINPEC_H

//  INCLUDES
#include <E32Base.h>
#include "CCnUiClientPluginBase.h"

class CPEngNWSessionSlotID2;
// CLASS DECLARATION

/**
 * PEC client plugin.
 *
 * NOTE!!
 * PEC project has responsibility to maintain / change implementation
 * of this class as they need.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiClientPluginPEC ) : public CCnUiClientPluginBase
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiClientPluginPEC* NewL( CPEngNWSessionSlotID2& aNWSessionSlotID );


    /**
     * Destructor.
     */
    ~CCnUiClientPluginPEC();

private:

    /**
     * C++ constructor.
     */
    CCnUiClientPluginPEC();

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
    void DoPostLoginProsessingL( TRequestStatus& aStatus );


    /**
     * From MCnUiClientPlugin
     * Overloaded from CCnUiClientPluginBase
     * @see MCnUiClientPlugin
     * @since 2.1
     */
    void DoPreLogoutProsessingL( TRequestStatus& aStatus );



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


    /**
    * Calls ResetAndDestroy to given array
    * @since 3.0
    * @param aObject Array (RPointerArray<MPEngPresenceAttrModel2>)
    */
    static void DestroyCloseModelArray( TAny* aObject );

private: // New methods
    /**
    * Update PECui Attributes
    * @since 3.0
    */
    void UpdateAttributesL();


private: // data

    };

#endif      //__CCNUICLIENTPLUGINPEC_H
//  End of File


