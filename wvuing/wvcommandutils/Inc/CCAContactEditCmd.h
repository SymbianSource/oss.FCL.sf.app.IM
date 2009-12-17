/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Login Command class
*
*/



#ifndef CCACONTACTEDITCMD_H
#define CCACONTACTEDITCMD_H


//  INCLUDES
#include <e32base.h>
#include "MCACommand.h"
#include "MCAUiContactEditCmdCB.h"
#include "MCAMainViewArrayPC.h"
#include "CCAContactEditPC.h"


// FORWARD DECLARATIONS
class MCAProcessManager;


// CLASS DECLARATION
/**
*  Login Command class.
*
*/
NONSHARABLE_CLASS( CCAContactEditCmd ) : public CBase, public MCACommand
    {
public:
    // Constructors and destructor

    /**
    * Destructor.
    */
    virtual ~CCAContactEditCmd();


public:

    /**
    * Set command observer.
    *
    * @param aObserver Command observer
    */
    void SetObserver( MCACommandObserver& aObserver );

    /**
    * Cancel command.
    *
    */
    void CancelCommand();
    /**
    * Executes the command.
    *
    */
    void ExecuteCommandL();

    /**
    *
    * @return ETrue if the command is asynchronous, otherwise EFalse.
    */
    TBool IsAsynchronous();

    /**
    * Two-phased constructor
    * @param aProcessManager reference to MCAProcessManager
    * @param aContactEditCmdCB reference to MCAUiContactEditCmdCB
    * @return New instance of CCAContactEditCmd
    */
    static CCAContactEditCmd* NewL( MCAProcessManager& aProcessManager,
                                    MCAUiContactEditCmdCB& aContactEditCmdCB );

    /**
    * Two-phased constructor
    * @param aProcessManager reference to MCAProcessManager
    * @param aContactEditCmdCB reference to MCAUiContactEditCmdCB
    * @return New instance of CCAContactEditCmd
    */
    static CCAContactEditCmd* NewLC( MCAProcessManager& aProcessManager,
                                     MCAUiContactEditCmdCB& aContactEditCmdCB );


protected:


    /**
    * LoadCCAContactDetails command.
    *
    * @param MCAUiContactEditCmdCB::TContactEditParams - Required
    *			data to perform the load operation
    */
    void LoadCCAContactDetailsL(
        MCAUiContactEditCmdCB::TContactEditParams &aParams );

    /**
    * SaveCCAContactDetails command.
    *
    * @param MCAUiContactEditCmdCB::TContactEditParams - Required
    *			data to perform the save operation
    */
    void CCAContactEditCmd::SaveCCAContactDetailsL
    ( MCAUiContactEditCmdCB::TContactEditParams &aParams );

    /**
    * MoveContactL command.
    *
    * @param MCAUiContactEditCmdCB::TContactEditParams - Required
    *			data to perform the move operation
    */
    void MoveContactL( MCAUiContactEditCmdCB::TContactEditParams &aParams );

private:

    /**
    * C++ default constructor.
    * @param1 Reference to UI call back object
    * @param2 Reference to contact editors process component
    */
    CCAContactEditCmd( MCAUiContactEditCmdCB& aUiContactEditCmdCB,
                       MCAContactEditPC& aContactEditPC );


private:

    //not owned - Reference to UI call back object
    MCAUiContactEditCmdCB& iUiContactEditCmdCB;

    //not owned - Reference to contact editors process component
    CCAContactEditPC&	iCCAContactEditPC;
    };

#endif      // CCACONTACTEDITCMD_H

// End of File
