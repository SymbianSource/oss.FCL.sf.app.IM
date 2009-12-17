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
* Description:  Login SAP select UI control.
*
*/

#ifndef __CCNUILOGINSAPSELECTUICNTRLSTEP_H
#define __CCNUILOGINSAPSELECTUICNTRLSTEP_H

//  INCLUDES
#include <E32Base.h>
#include <f32file.h>
#include "MCnUiCntrlStep.h"
#include "impspresenceconnectionuiconstsng.h"

//FORWARD DECLARATION
class MCnUiUiControlContext;
class MCnUiOpContextAdv;
class CIMPSSAPSettingsList;
class CIMPSSAPSettings;
class RResourceFile;
class MIMPSSharedData;

// CLASS DECLARATION
/**
 * Login SAP select UI control.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiLoginSAPSelectUiCntrlStep ) : public CBase,
        public MCnUiCntrlStep
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     * @param aCCntxt The UI control context.
     * @param aLoginSap login sap
     * @param aLoginType The login type mode.
     * @param aLastLoginSap
     */
    static CCnUiLoginSAPSelectUiCntrlStep* NewLC( MCnUiUiControlContext& aCCntxt,
    CIMPSSAPSettings& aLoginSap,
    TIMPSLoginType aLoginType, TBool aLastLoginSap = EFalse );


    /**
     * Destructor.
     */
    virtual ~CCnUiLoginSAPSelectUiCntrlStep();


private:

    /**
     * C++ constructor.
     * @param aLastLoginSap
     */
    CCnUiLoginSAPSelectUiCntrlStep( MCnUiUiControlContext& aCCntxt,
                                    CIMPSSAPSettings& aLoginSap,
                                    TIMPSLoginType aLoginType, TBool aLastLoginSap = EFalse );


    /**
     * Symbian OS constructor.
     */
    void ConstructL();



public: // New functions from MCnUiCntrlStep


    /**
     * Runs SAP selection step.
     *
     * @since 2.1
     * @return Error code from SAP selection.
     */
    TInt RunStepL();


    /**
     * Handles complete.
     *
     * If needed updates the selected login SAP
     * sername and passoword to those which were
     * logged in.
     *
     * @since 2.1
     * @return Always ECnUiStepContinueTeardown.
     */
    TCnUiHandleCompleteStatus HandleCompleteL();


    /**
     * Handles undo.
     *
     *
     * @since 2.1
     */
    void UndoStepL();



public: // New functions, left public to allow testing


    /**
     * Shows a note about simultanously runing
     * login operation.
     *
     * @since 2.1
     */
    void ShowSimultanousLoginOperationNoteL();


    /**
     * Loads default SAP from SAP Settings
     * store as login SAP.
     *
     * @since 2.1
     * @param aSap
     * @return KErrNone / KErrNotFound.
     */
    TInt LoadDefaultSapL( CIMPSSAPSettings& aSap );


    /**
     * Loads SAP from SAP Settings
     * store as based on user selection.
     *
     * If 0 SAP ==> KErrNotFound
     * If 1 SAP ==> Selects this automaticly
     * If 2..n SAP ==> Shows a selection list
     * if user cancels the selection ==> KErrCancel
     *
     * @since 2.1
     * @param aSap
     * @return KErrNone / KErrNotFound / KErrCancel.
     */
    TInt SelectSapManuallyL( CIMPSSAPSettings& aSap );


    /**
     * Gets server list and index which to highlight
     * in selection list.
     *
     * @since 2.1
     * @param aIndexToHighlight On the return contains the
     * index that should be high lighted by default in the
     * selection list, or KErrNotFound if default index should
     * be ignored.
     * @return SAP settings list. Ownership of list returned to client.
     */
    CIMPSSAPSettingsList* ServerListLC( TInt& aIndexToHighlight );


    /**
     * Loads the used login SAP based the SAP Settings list
     * and index of selected server.
     *
     * @since 2.1
     * @param aList Server list.
     * @param aIndexOfSelectedServer The index of selected server.
     * @param aSap
     */
    void LoadSapByServerListL( const CIMPSSAPSettingsList& aList,
                               TInt aIndexOfSelectedServer,
                               CIMPSSAPSettings& aSap );


private: // new helper
    /**
     * Gets the current client information from TLS
     *
     * @since 3.0
     */
    TIMPSConnectionClient ConnectionClient();

    /**
     * Opens correct resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.1
     */
    void OpenResourceFileLC( RResourceFile& aResourceFile );

    /**
     * Resource reader for non-Cone situations
     * @param aResourceId The integer-valued resource id to read
     * @return The value of the integer
     * @since 3.1
     */
    TInt ReadResourceIntValueL( TInt aResourceId );


private: // data

    ///< UI control context to use, not owned
    MCnUiUiControlContext&              iCCntxt;

    ///<Login SAP settings, not owned
    CIMPSSAPSettings&                     iLoginSap;

    ///<Login type setting, owned
    const TIMPSLoginType                iLoginType;

    ///<State flag, is post login SAP update needed, owned
    TBool                               iLoginSapUpdateNeeded;

    ///<Login SAP uid, owned
    TUint32                             iLoginSapUid;

    // file handle used by resource file reading routine
    RFs   iRFs;

    MIMPSSharedData*	iSharedData;

    //whether to use for new login
    //
    TBool iLastLoginSap;

    };

#endif      //  __CCNUILOGINSAPSELECTUICNTRLSTEP_H

//  End of File

