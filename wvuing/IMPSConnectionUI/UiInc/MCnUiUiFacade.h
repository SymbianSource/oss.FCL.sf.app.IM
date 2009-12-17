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
* Description:  UI Facade for Connection UI.
*
*/

#ifndef __MCNUIUIFACADE_H
#define __MCNUIUIFACADE_H

//  INCLUDES
#include <E32Base.h>
#include <bamdesca.h>


//FORWARD DECLARATION
class MCnUiUiFacade;
class MCnUiConnWaitNote;
class MCnUiWaitNoteObserver;
class MIMPSConnProcessUi;
class MCnUiConnQuery;
class MCnUiConnQueryObserver;
class CIMPSSAPSettings;

/**
 * Factory method for UI facade construction.
 *
 * Global factory method to create concrete
 * UI facade.
 *
 * @since 2.1
 * @return New MCnUiUiFacade instance.
 */
GLREF_D MCnUiUiFacade* CreateUiFacadeL();
GLREF_D MCnUiUiFacade* CreateUiFacadeLC();


// DATA TYPES

/**
 * Enumeration defining different
 * UI notes.
 * @since 2.1
 */
enum TCnUiConnectionNotes
    {
    ECnUiConnectionOK,
    ECnUiDisconnectedIM,
    ECnUiDisconnectedPEC,
    ECnUiDisconnectedAutomaticConnection,
    ECnUiConnCanceled,   //Use only when canceling login out of login cntrl
    ECnUiConnForceLogout,
    ECnUiGPRSConnectionLost
    };

/**
 * Enumeration defining different
 * UI notes with dynamic text.
 * @since 2.1
 */
enum TCnUiConnectionNotesDynText
    {
    ECnUiImproperPresenceId,
    ECnUiConnOperationAllreadyRunning
    };


/**
 * Enumeration defining different
 * UI wait notes.
 * @since 2.1
 */
enum TCnUiConnectionWaitNotes
    {
    ECnUiWaitNoteDisconnecting,
    ECnUiWaitNotePlugin
    };


/**
 * Enumeration defining different
 * UI wait notes with dynamic text.
 * @since 2.1
 */
enum TCnUiConnectionWaitNotesDynText
    {
    ECnUiWaitNoteConnectingChat,
    ECnUiWaitNoteConnectingPEC,
    ECnUiWaitNoteReConnectingChat,
    ECnUiWaitNoteReConnectingPEC
    };


/**
 * Enumeration defining different
 * confirmation queries.
 * @since 2.1
 */
enum TCnUiConnectionQueries
    {
    ECnUiCloseConnectionToNetwork
    };


/**
 * Enumeration defining different
 * confirmation queries with dynamic text.
 * @since 2.1
 */
enum TCnUiConnectionQueriesDynText
    {
    ECnUiDropPECLogin,
    ECnUiDropIMLogin
    };



/**
 * Enumeration defining different queries.
 *
 * Async queries are asynchronous
 * queries which report the user selection as
 * observer callback.
 *
 * @since 2.1
 */
enum TCnUiConnAsyncQuery
    {
    ECnUiQueryActiveConnectionScheduledClose
    };


/**
 * Enumeration defining different
 * queries with dynamic text.
 *
 * Async queries are asynchronous
 * queries which report the user selection as
 * observer callback.
 *
 * @since 2.1
 */
enum TCnUiConnAsyncQueryDynText
    {
    ECnUiQueryActiveConnectionSuppressForScheduledConnection
    };



/**
 * Modes for login data query.
 * @since 2.1
 */
enum TCnUiLoginDataQueryMode
    {
    ECnUiNormal,            //<- Normal login data edition mode.
    ECnUiReEdit,            //<- Login data is re-edited after unsuccesful
    //   login try.
    ECnUiEditPrefilledUser  //<- Cursor is in the user id field, before the
    //   pre-filleduser id
    };



/**
 * UI facade interface for Connection UI.
 *
 * UI facade interface for Connection UI.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiUiFacade )
    {
public: // Ui features

    /**
     * Returns a reference to Connection Process UI.
     *
     * Connection Process UI is generalized UI interface
     * offered to connection observers.
     *
     * @since 2.1
     * @return MIMPSConnProcessUi The connection process UI.
     */
    virtual MIMPSConnProcessUi& ConnProcessUi() = 0;


    /**
     * Launches command absorber and leaves it to cleanup stack.
     * (Calling CleanupStack::PopAndDestroy() clears
     * command absorber. Note! CleanupStack::PopAndDestroy() call
     * must be used, calling just CleanupStack::Pop() will cause
     * a memory leak.)
     *
     * Command absorber clears CBA buttons (puts empty ones in the
     * screen) and puts also a command absorbing control as top of
     * control stack. (These prevent event leakage to using
     * applications during login and logout operations.)
     * @since 2.1
     */
    virtual void CommandAbsorbOnLC() = 0;



    /**
     * Flushes the UI command buffer.
     * @since 2.1
     */
    virtual void FlushCommandBuffer() = 0;


    /**
     * Launches an asynchronous wait note and returns a handle to it.
     *
     * Launches an asynchronous wait note and returns a handle to it.
     * Wait note is cleared by deleting the returned MCnUiConnWaitNote
     * handle object. Wait note can be pushed on the CleanupStack by
     * MCnUiConnWaitNote::PushLC() if needed.
     *
     * @since 2.1
     * @param aNoteId Id enumerating which wait note to show.
     * @param aObserver Observer to notify from wait note cancel.
     * If notification isn't wanted, NULL can be used.
     * @return waitnote
     */
    virtual MCnUiConnWaitNote* WaitNoteL( TCnUiConnectionWaitNotes aNoteId,
                                          MCnUiWaitNoteObserver* aObserver = NULL ) = 0;


    /**
     * Launches an asynchronous wait note and returns a handle to it.
     *
     * Launches an asynchronous wait note and returns a handle to it.
     * Wait note is cleared by deleting the returned MCnUiConnWaitNote
     * handle object. Wait note can be pushed on the CleanupStack by
     * MCnUiConnWaitNote::PushLC() if needed.
     *
     * If wait note has a cancel feature enabled and the note gets
     * cancelled, this causes a call to registered wait note observer
     * MCnUiWaitNoteObserver::HandleWaitNoteCancel() method.
     *
     * @since 2.1
     * @param aNoteId Id enumerating which wait note to show.
     * @param aInsertText The text to insert inside prompt text.
     * @param aObserver Observer to notify from wait note cancel.
     * If notification isn't wanted, NULL can be used.
     * @return waitnote
     */
    virtual MCnUiConnWaitNote* WaitNoteL( TCnUiConnectionWaitNotesDynText aNoteId,
                                          const TDesC& aInsertText,
                                          MCnUiWaitNoteObserver* aObserver = NULL ) = 0;

    /**
     * Launches an non blocking query and returns a handle to it.
     *
     * Query is executed in non blocking manner and it
     * reports selection to registered observer.
     *
     * @since 2.1
     * @param aQueryId Id enumerating which query to show.
     * @param aInsertText The dynamic text to insert in query.
     * @param aObserver The observer to report query selections.
     * @return Query handle.
     */
    virtual MCnUiConnQuery* QueryL( TCnUiConnAsyncQueryDynText aQueryId,
                                    const TDesC& aInsertText,
                                    MCnUiConnQueryObserver& aObserver ) = 0;


    /**
     * Launches an non blocking query and returns a handle to it.
     *
     * Query is executed in non blocking manner and it
     * reports selection to registered observer.
     *
     * @since 2.1
     * @param aQueryId Id enumerating which query to show.
     * @param aObserver The observer to report query selections.
     * @return Query handle.
     */
    virtual MCnUiConnQuery* QueryL( TCnUiConnAsyncQuery aQueryId,
                                    MCnUiConnQueryObserver& aObserver ) = 0;



    /**
     * Shows enumerated information/error note.
     * Method implementation is synchronous,
     * eg. method call returns after note timeout.
     *
     * @since 2.1
     * @param aNoteId Id enumerating which note to show.
     */
    virtual void ShowNoteL( TCnUiConnectionNotes aNoteId ) = 0;


    /**
     * Shows enumerated information/error note.
     * Method implementation is synchronous,
     * eg. method call returns after note timeout.
     *
     * @since 2.1
     * @param aNoteId Id enumerating which note to show.
     * @param aInsertText The text to insert inside prompt text.
     */
    virtual void ShowNoteL( TCnUiConnectionNotesDynText aNoteId,
                            const TDesC& aInsertText ) = 0;

    /**
     * Shows information / error note for login error code.
     * Method implementation is synchronous,
     * eg. method call returns after note timeout.
     *
     * @since 2.1
     * @param aErrorCode
     */
    virtual void ShowLoginErrorNoteL( TInt aErrorCode ) = 0;


    /**
     * Shows information / error note for logout error code.
     * Method implementation is synchronous,
     * eg. method call returns after note timeout.
     *
     * @since 2.1
     * @param aErrorCode
     */
    virtual void ShowLogoutErrorNoteL( TInt aErrorCode ) = 0;


    /**
     * Asks a confirmation question from user.
     * Method implementation is synchronous,
     * eg. method call returns after user selection.
     *
     * @since 2.1
     * @param aQueryId Id enumerating which query to show.
     * @return ETrue if user accepted the question,
     *         else EFalse.
     */
    virtual TBool ConfirmationQueryL( TCnUiConnectionQueries aQueryId ) = 0;


    /**
     * Asks a confirmation question from user.
     * Method implementation is synchronous,
     * eg. method call returns after user selection.
     *
     * @since 2.1
     * @param aQueryId Id enumerating which query to show.
     * @param aInsertText The text to insert inside prompt text.
     * @return ETrue if user accepted the question,
     *         else EFalse.
     */
    virtual TBool ConfirmationQueryL( TCnUiConnectionQueriesDynText aQueryId,
                                      const TDesC& aInsertText ) = 0;


    /**
     * Asks a login data query from user.
     * Login data consists from username and password.
     *
     * (Method call returns after user decision input.)
     *
     * @since 2.1
     * @param aUsername The username string to to edit.
     * @param aPassword The password string to to edit.
     * @param aSap Current SAP.
     * @param aMode The required query mode.
     * @return ETrue if user accepted the query.
     *         EFalse if user cancels the query.
     */
    virtual TBool LoginQueryL( TDes& aUsername,
                               TDes& aPassword,
                               CIMPSSAPSettings& aSap,
                               TCnUiLoginDataQueryMode aMode ) = 0;


    /**
     * Asks a server list query from user.
     *
     * Shows listed servers in the list query. User can select
     * from the list the server to connect. Returns the index of
     * selected server.
     *
     * Passed list must contain two or more servers. If there is
     * less servers, leaves with KErrNotSupported.
     *
     * (Method call returns after user decision input.)
     *
     * @since 2.1
     * @param aServers Descriptor array of server names to show in query.
     * @param aIndexToHighlight The index to highlight in server query.
     * If passed index is out of bounds, highlights the first server.
     * @param aSelectedIndex Returns the index of selected server.
     *        If user doesn't make any selection, aSelectedIndex doesn't get changed.
     *        Index value follows normal MDesCArray index semantics.
     * @return ETrue if user accepted the query.
     *         EFalse if user cancels the query.
     */
    virtual TBool ServerToUseQueryL( MDesCArray& aServers,
                                     TInt aIndexToHighlight,
                                     TInt& aSelectedIndex ) = 0;


    /**
     * Handles error by showing general application
     * UI context error note.
     *
     * Handles error by showing general application
     * UI context error note. If not a error, does nothing.
     *
     * @since 2.1
     * @param aErr The error to handle.
     */
    virtual void HandleIfError( TInt aErr ) = 0;


    /**
     * Sets the new resource file to use in Connection UI.
     *
     * @param aNewResourceFile the new resource file to be used
     *		  If empty, switches to default resource file
     * @since 2.6
     */
    virtual void SwitchResourceFileL( const TDesC& aNewResourceFile ) = 0;

    /**
     * Displays a list query for selecting a domain ("@domain.com" etc.)
     * @param aSelectedDomain Here is copied the domain user selected.
     * @param aSap Current SAP, can be NULL.
     * @return If user cancelled the query return 0,
     *         otherwise the dialog dismiss key code
     * @since 3.1u
     */
    virtual TInt DisplayDomainSelectionQueryL( TDes& aSelectedDomain,
                                               CIMPSSAPSettings* aSap = NULL ) = 0;

    /**
     * Displays the Terms of Use -text and asks from user if he/she
     * wants to accept them.
     * @return ETrue if user accepted the Terms of Use.
     *         If user cancelled or selected no from the
     *         query, return EFalse.
     *         otherwise the dialog dismiss key code
     * @since 3.1u
     */
    virtual TBool DisplayTermsOfUseAndQueryL() = 0;

public:  //Destructor

    /**
     * Virtual inline destructor.
     * Allows destruction using this
     * interface
     */
    virtual ~MCnUiUiFacade() {};
    };



/**
 * General wainote handle.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnWaitNote )
    {
public:  //New functions

    /**
     * Pushes waitnote object on
     * the CleanupStack.
     *
     * @since 2.1
     */
    virtual void PushLC() = 0;


public:  //Destructor

    /**
     * Virtual inline destructor.
     * Allows destruction using this
     * interface
     */
    virtual ~MCnUiConnWaitNote() {};
    };



/**
 * General query handle.
 *
 * @since 2.1
 */
NONSHARABLE_CLASS( MCnUiConnQuery )
    {
public:  //Destructor

    /**
     * Virtual inline destructor.
     * Allows destruction using this
     * interface
     */
    virtual ~MCnUiConnQuery() {};
    };


#endif      //  __MCNUIUIFACADE_H


//  End of File

