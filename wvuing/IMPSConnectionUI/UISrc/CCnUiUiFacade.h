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
* Description:  UI facade.
*
*/

#ifndef __CCNUIFACADE_H
#define __CCNUIFACADE_H

//  INCLUDES
#include <e32base.h>
#include "mimpsconnprocessobserverng.h"
#include "MCnUiUiFacade.h"
#include "CCnUiSignInControl.h"


// FORWARD DECLARATIONS
class CCoeEnv;
class MCnUiWaitNoteObserver;
class CCommandAbsorbingControl;
class CAknIconArray;
class CIMPSSAPSettings;
class MIMPSSharedData;
class CBrowserLauncher;

/**
 * Internal note type enumeration.
 * @since 2.1
 */
enum TCnUiConnUiNoteType
    {
    EInfoNote = 0,
    EConfirmationNote,
    EErrorNote,
    EConfirmedErrorNote,
    EConfirmedErrorNoteMSKSelectIcon
    };



// CLASS DECLARATION
/**
 * UI facade implementation for
 * Connection UI
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( CCnUiUiFacade ) : public CBase,
        public MCnUiUiFacade,
        public MIMPSConnProcessUi
    {
public:  // Two-phased constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCnUiUiFacade* NewL();


    /**
     * Destructor.
     */
    virtual ~CCnUiUiFacade();

private:

    /**
     * C++ constructor.
     */
    CCnUiUiFacade();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();


public: // Functions from MCnUiUiFacade


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    MIMPSConnProcessUi& ConnProcessUi();


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void CommandAbsorbOnLC();


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void FlushCommandBuffer();


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    MCnUiConnWaitNote* WaitNoteL( TCnUiConnectionWaitNotes aNoteId,
                                  MCnUiWaitNoteObserver* aObserver = NULL );

    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    MCnUiConnWaitNote* WaitNoteL( TCnUiConnectionWaitNotesDynText aNoteId,
                                  const TDesC& aInsertText,
                                  MCnUiWaitNoteObserver* aObserver = NULL );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    MCnUiConnQuery* QueryL( TCnUiConnAsyncQuery aQueryId,
                            MCnUiConnQueryObserver& aObserver );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    MCnUiConnQuery* QueryL( TCnUiConnAsyncQueryDynText aQueryId,
                            const TDesC& aInsertText,
                            MCnUiConnQueryObserver& aObserver );

    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void ShowNoteL( TCnUiConnectionNotes aNoteId );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void ShowNoteL( TCnUiConnectionNotesDynText aNoteId,
                    const TDesC& aInsertText );

    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void ShowLoginErrorNoteL( TInt aErrorCode );

    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void ShowLogoutErrorNoteL( TInt aErrorCode );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    TBool ConfirmationQueryL( TCnUiConnectionQueries aQueryId );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    TBool ConfirmationQueryL( TCnUiConnectionQueriesDynText aQueryId,
                              const TDesC& aInsertText );

    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    TBool LoginQueryL( TDes& aUsername,
                       TDes& aPassword,
                       CIMPSSAPSettings& aSap,
                       TCnUiLoginDataQueryMode aMode );


    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    TBool ServerToUseQueryL( MDesCArray& aServers,
                             TInt aIndexToHighlight,
                             TInt& aSelectedIndex );
    /**
     * @see MCnUiUiFacade
     * @since 2.1
     */
    void HandleIfError( TInt aErr );

    /**
     * @see MCnUiUiFacade
     * @since 2.6
     */
    void SwitchResourceFileL( const TDesC& aNewResourceFile );

    /**
     * @see MCnUiUiFacade
     * @since 3.1u
     */
    TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                       CIMPSSAPSettings* aSap = NULL );

    /**
     * @see MCnUiUiFacade
     * @since 3.1u
     */
    TBool DisplayTermsOfUseAndQueryL();

public: //from MIMPSConnProcessUi

    /**
     * Sets the new resource file to use in Connection UI.
     *
     * @see MIMPSConnProcessUi
     * @since 2.1
     */
    void UseResourceFileL( const TDesC& aNewResourceFile );


    /**
     * Gets Connection UI's default resource file name.
     *
     * @see MIMPSConnProcessUi
     * @since 2.1
     */
    void ResourceFileNameL( TDes& aResourceFileName );


private: //New helpers


    /**
     * Removes the possibly added resource file
     * from resource file stack,
     * @since 2.1
     */
    void RemoveUsedResourceFile();


    /**
     * Retrieves a system error text for given
     * error code and returns it as a HBufC.
     * Ownership of HBufC is returned to caller.
     *
     * If any matching system error text isn't
     * found for error number, returns a NULL.
     *
     * Performs internally certain filtering and
     * mapping operations for basic KErr* error
     * codes.
     *
     * @since 2.1
     * @param aErrorCode for which to get the error
     * text.
     * @return HbufC buffer containing the proper error
     * text or NULL pointer if matching text wasn't found.
     */
    HBufC* SystemErrorTextOrNullForCodeL( TInt aErrorCode );


    /**
     * Shows a desired type note with desired text
     * loaded from given resource id.
     *
     * @since 2.1
     * @param aNoteTextResourceId Resource of the note text to show.
     * @param aNoteType Type of shown note.
     */
    void DoShowNoteL( TInt aNoteTextResourceId, TCnUiConnUiNoteType aNoteType );


    /**
     * Shows a desired type note with given text.
     *
     * @since 2.1
     * @param aNotePrompt The prompt text to show in note.
     * @param aNoteType Type of shown note.
     */
    void DoShowNoteL( const TDesC& aNotePrompt, TCnUiConnUiNoteType aNoteType );


    /**
     * Shows a confirmation query with given text.
     *
     * @since 2.1
     * @param aPromptText The prompt text to show in query.
     * @return ETrue if user accepted the query, else EFalse.
     */
    TBool DoShowConfirmationQueryL( const TDesC& aPromptText );

    /**
    * Constructs complete path from parts and checks for correctness.
    * @param aResult, the complete path.
    * @param aModuleId, dll's (or app) id. For example "IM".
    * @param aAlternative, id for alternative (for example operator "Oper1" ) files.
    * @param aFileName, filename of the complete path
    * @return ETrue if path was found from C or MMC card, otherwise false
    * @since 2.1
    */
    TBool ConstructCompletePathAndCheck( TDes& aResult,
                                         const TDesC& aModuleId,
                                         const TDesC& aAlternative,
                                         const TDesC& aFileName );

    /**
    * Returns MMC drive.
    * @param aDrive, the result.
    */
    void MMCDrive( TDes& aDrive ) const;

    /**
     * Loads the branded server icons to iIconArray
     * @param aServers List of the server names
     */
    void LoadServerIconsL( MDesCArray& aServers );


    /**
     * Opens correct resource file.
     * !!! Two items left to cleanup stack. !!!
     * @param aResourceFile Contains opened file on return
     * @since 3.2
     */
    void OpenResourceFileLC( RResourceFile& aResourceFile );

    /**
     * Reads integer resource value from resources
     * @param aResourceId resource id which is read
     * @return value of wanted integer resource flag
     */
    TInt IntResourceValueL( TInt aResourceId );

    /**
     * Shows WAP registration query
     * @return command id for user action
     */
    TInt ShowWAPQueryL();

    /**
     * Shows Terms and Conditions query
     * @return command id for user action
     */
    TInt ShowTAndCDlgL();

    /**
     * Opens native browser with the specified URL
     * @param a Url
     */
    void OpenWebBrowserL( const TDesC& aUrl );

    TInt GetIntResourceValueL( TInt aResourceId );

private: // data
    ///<Cached pointer to control environment, not owned
    CCoeEnv*                    iCoeEnv;

    ///<Resource file offset, owned
    TInt                        iResourceFileOffset;

    ///<Default resource file name, owned
    HBufC*                      iDefaultRssFileName;

    ///<Flag wether the resource file has been added or not.
    TBool                       iResourceFileAdded;

    ///<Command absorber, owned
    CCommandAbsorbingControl*   iCommandAbsorber;

    ///<Icon array for server list query, owned
    CAknIconArray* iIconArray;

    MIMPSSharedData* iSharedData;

    // Browser launcher
    CBrowserLauncher* iBrLauncher;
    // file handle used by resource file reading routine
    RFs   iRFs;
    CCnUiSignInControl* iControl;
    };

#endif      //  __CCNUIFACADE_H

//  End of File

