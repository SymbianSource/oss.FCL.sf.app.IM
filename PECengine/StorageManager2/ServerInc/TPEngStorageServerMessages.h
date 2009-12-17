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
* Description:  Service messages between Storage Manager and Storage Server
*
*/

#ifndef __TPENGSTORAGESERVERMESSAGES_H__
#define __TPENGSTORAGESERVERMESSAGES_H__

// CONSTANTS

// Mask to include additional TBool value for creation of entry in lock mechanism
const TInt KLockEntryCreateMask = 0x100000;

// DATA TYPES
enum TPEngStorageServerMessages
    {
    /**
     *	Every message to the server carries 4 data slots which can be used
     *	to transfer data to the server side.
     *	If sub-sessions are used, 4th slot of the clients message
     *	is reserved for main session identification.
     *	Meanings of parameters of the message vary for different requests,
     *	order of parameters is fixed, therefore parameters are future
     *	referred as	MessageSlot0, MessageSlot1, MessageSlot2, MessageSlot3.
     *	Type of the data in the message slots can vary.
     */

    // =======================================================================
    // ================= Main Session requests ===============================
    // =======================================================================

    /*********************************************************************/
    /* Server Management                                                 */
    /*********************************************************************/

    // shut down the server
    EMainSessShutdownServer,

    /*********************************************************************/
    /* Sub Session management                                            */
    /*********************************************************************/

    /**
     *	Create new Storage folder sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with Session Id
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCreateFolderSubSession,

    /**
     *	Create new Administrator sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with Session Id
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCreateAdminSubSession,

    /**
     *	close sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - not used
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCloseSubSession,

    /*********************************************************************/
    /* Global Event Notification                                         */
    /*********************************************************************/

    /**
     *	start listening of global event
     *
     *	Message package carries:
     *		MessageSlot 0 - not used
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 -
     */
    EMainSessListenGlobalEvents,

    /*********************************************************************/
    /* Asynchronous Requests Management                                  */
    /*********************************************************************/

    /**
     *	Cancel Asynchronous request Request
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessCancelRequest,

    /**
     *	Reload Asynchronous Scout on the Server side
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessReloadAsynchronousScout,

    /*********************************************************************/
    /* Session Slot Management                                           */
    /*********************************************************************/

    /**
     *	Create Session Slot Folder
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessCreateSessionFolder,

    /**
     *	Remove Session Slot Folder
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessRemoveSessionFolder,

    /**
     *	Get Session Slot State
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessGetSessionState,

    /**
     *	Get States of all sessions
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessGetAllSessionStates,

    /*********************************************************************/
    /* Resting support                                                   */
    /*********************************************************************/

    /**
     *  Wipe out Session slot Folder from the storage
     *
     *	Message package carries:
     *		MessageSlot 0 - packes session slot Id
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 -
     */
    EMainSessWipeSessionFolder,

    // ==================================================================
    // ================ Admin Sub session client requests ===============
    // ==================================================================

    /*********************************************************************/
    /* Global event notification                                         */
    /*********************************************************************/

    /**
     *	Notify new global event
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with detailed event description
     *		MessageSlot 1 - not-used
     *		MessageSlot 2 - not-used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EAdminSubSessNotifyNewEvent,

    /**
     *  Notify error to listeners
     *
     *	Message package carries:
     *		MessageSlot 0 - error code
     *		MessageSlot 1 - not-used
     *		MessageSlot 2 - not-used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EAdminSubSessNotifyError,

    /*********************************************************************/
    /* Registered Application Id check                                   */
    /*********************************************************************/

    /**
     *	Check if passed App Id is registered for passed Session Slot
     *
     *	Message package carries:
     *		MessageSlot 0 - application Id
     *		MessageSlot 1 - not-used
     *		MessageSlot 2 - not-used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EAdminSubSessCheckAppIdRegistration,


    // =======================================================================
    // ================= Folder Sub Session client requests ==================
    // =======================================================================

    /*********************************************************************/
    /*  Asynchronous Requests Management                                 */
    /*********************************************************************/

    /**
     *	Cancel Asynchronous request Request
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessCancelRequest,

    /**
     *	Reload Asynchronous Scout on the Server side
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessReloadAsynchronousScout,

    /*********************************************************************/
    /*  Store Entry management                                           */
    /*********************************************************************/

    /**
     *	Write store entry
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessWriteEntry,

    /**
     *	Read store Entry
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessReadEntry,

    /**
     *	Get Entry size
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessGetEntryLength,

    /**
     *	Remove Store entry from the storage
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessRemoveEntry,

    /**
     *	Lock Store Entry
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessLockEntry,

    /**
     *	Unlock the Store entry
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessUnlockEntry,

    /**
     *	Is Store entry locked
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessIsEntryLocked,

    /**
     *	Suspend notification to the client
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessSuspendNotification,

    /**
     *	Resume notification to the client
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessResumeNotification,

    /*********************************************************************/
    /*   Storage ID changes notify management                            */
    /*********************************************************************/

    /**
     *	Synchronous Request to register for notification
     *	of change of content of particular storage entry,
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessListenSIDsChanges,

    /**
     *	Synchronous Request to update set of SIDs client
     *	is interested in.
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessUpdateListenSIDsScout,

    /**
     *	Synchronous Request to fetch changed SIds indexes
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessFetchChangedSIDsIndexes,

    /**
     *	Synchronous Request to notify that client forces Store Entry
     *	change notification
     *
     *	Message package carries:
     *		MessageSlot 0 -
     *		MessageSlot 1 -
     *		MessageSlot 2 -
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EFolderSubSessNotifyChangedSIDs,

    /**
     * Highest Storage server request value, not used for any client's request
     * It is used for specifying of the Request ranges
     */
    EHighestStoreServerRequest
    };

#endif      //  __TPENGSTORAGESERVERMESSAGES_H__

//  End of File
