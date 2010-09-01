/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence server message definition
*
*/

#ifndef __TPENGSERVERMESSAGES_H__
#define __TPENGSERVERMESSAGES_H__

// INCLUDES

// DATA
enum TPEngServerMessages
    {
    // !!!! If new Sever message is added here, update it also here
    // !!!! Remeber to update Highest request number for
    // !!!! Server Policy check up

    /**
     *  EMainSessShutdownServer                 = 1,
     *  EMainSessCreateHolderSubSession         = 2,
     *  EMainSessCreateTransactionSubSession    = 3,
     *  EMainSessCloseSubSession                = 4,
     *  EMainSessCreateAdminSubSession          = 5,
     *  EMainSessCloseAdminSubSession           = 6,
     *  ESubSessCancelRequest                   = 7,
     *  ESubSessLogIn                           = 8,
     *  ESubSessForceLogOut                     = 9,
     *  ESubSessAttachHolder                    = 10,
     *  ESubSessAttachWithProcessName           = 11,
     *  ESubSessDetachHolder                    = 12,
     *  ESubSessLeaveAlive                      = 13,
     *  ESubSessGetServiceTree                  = 14,
     *  ESubSessGetLogInData                    = 15,
     *  ESubSessUpdateData                      = 16,
     *  ESubSessFetchUpdateRqstResult           = 17,
     *  EHighestServerRequest                   = 18
     */

    /**
      *	Every message to the server carries 4 data slots which can be used
      *	to transfer data to the server side.
      *	If sub-sessions are used, 4th slot of the clients message
      *	is reserved for main session identification.
      *	Meanings of parameters of the message vary for different requests,
      *	order of parameters is fixed, therefore parameters are future
      *	referred as	MessageSlot0, MessageSlot 1, MessageSlot 2, MessageSlot 3.
      *	Type of the data in the message slots can vary.
      */

    // =======================================================================
    // ================= Main Session requests ===============================
    // =======================================================================

    /*********************************************************************/
    /* Server Management                                                 */
    /*********************************************************************/

    // shut down the server
    EMainSessShutdownServer				    = 1,

    /*********************************************************************/
    /* Sub Session management                                            */
    /*********************************************************************/

    /**
     *	Create new Session Holder sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with Session Id
     *		MessageSlot 1 - Application Id
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCreateHolderSubSession		    = 2,

    /**
     *	Create new Transaction sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with Session Id
     *		MessageSlot 1 - Application Id
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCreateTransactionSubSession    = 3,

    /**
     *	close sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - not used
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCloseSubSession                = 4,


    /**
     *	Create new Administrator sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with Session Id
     *		MessageSlot 1 - Application Id
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCreateAdminSubSession          = 5,

    /**
     *	close Administrator sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - not used
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    EMainSessCloseAdminSubSession           = 6,


    // =======================================================================
    // ================= Sub Session requests ================================
    // =======================================================================

    /*********************************************************************/
    /*  Asynchronous Requests Management                                 */
    /*********************************************************************/

    /**
     *	Cancel Asynchronous request Request in the sub-session
     *
     *	Message package carries:
     *		MessageSlot 0 - Enumeration of the request function
     *		MessageSlot 1 - not used
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessCancelRequest                   = 7,

    /*********************************************************************/
    /*  Session holder client                                            */
    /*********************************************************************/

    /**
     *	Asynchronous request to log in to the presence service
     *
     *	There can be only one request of this kind pending
     *	on the server per each CSP session
     *
     *	Message package carries:
     *		MessageSlot 0 - Buffer with the Log In data
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessLogIn                           = 8,

    /**
     *	Asynchronous request to Force Log out CSP session
     *
     *	There can be only one request of this kind pending
     *	on the server per each CSP session
     *
     *	Message package carries:
     *		MessageSlot 0 - Not used
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessForceLogOut                     = 9,

    /**
     *	Synchronous message to attach session holder
     *	Session holder is attached to the CSP session if it
     *	exists
     *
     *	Message package carries:
     *		MessageSlot 0 - Not used
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessAttachHolder                    = 10,

    /**
     *	Synchronous message to attach session holder
     *	Attaching is done for reserved Process Name
     *	Session holder is attached to the CSP session if it
     *	exists.
     *
     *	Message package carries:
     *		MessageSlot 0 - Process Name
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessAttachWithProcessName           = 11,

    /**
     *	Asynchronous request to detach session holder from CPS
     *	session. This can lead in closing of whole CPS session
     *	therefore request is asynchronous.
     *
     *	Message package carries:
     *		MessageSlot 0 - Not used
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessDetachHolder                    = 12,

    /**
     *	Synchronous request to reserve CPS session for defined
     *	process name
     *
     *	Message package carries:
     *		MessageSlot 0 - process name
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessLeaveAlive                      = 13,

    /**
     *	Synchronous request to get Service tree of opened session
     *
     *	Message package carries:
     *		MessageSlot 0 - descriptor to write buffer with the
     *						packed service tree
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessGetServiceTree                  = 14,

    /**
     *	Synchronous request to get Log in data of opened session
     *
     *	Message package carries:
     *		MessageSlot 0 - descriptor to write Log In data
     *		MessageSlot 1 - Not used
     *		MessageSlot 2 - Not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessGetLogInData                    = 15,



    /*********************************************************************/
    /*  Transfer client                                                  */
    /*********************************************************************/

    /**
     *	Asynchronous request to update defined data to the network
     *
     *	Message package carries:
     *		MessageSlot 0 - Operation Id
     *		MessageSlot 1 - data to be updated
     *		MessageSlot 2 - Length of passed data
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessUpdateData                      = 16,

    /**
     *	Synchronous request to fetch result data for last Update
     *	request
     *
     *	Message package carries:
     *		MessageSlot 0 - buffer server should store result
     *		MessageSlot 1 - max available size of buffer
     *		MessageSlot 2 - not used
     *		MessageSlot 3 - reserved by Sub-session framework
     */
    ESubSessFetchUpdateRqstResult           = 17,

    /**
     * Highest server request value, not used for any client's request
     * It is used for specifying of the Request ranges
     * which are allowed for the client
     * @see CPolicyServer
     */
    EHighestServerRequest                           = 18
    };


#endif      //  __TPENGSERVERMESSAGES_H__

//  End of File

