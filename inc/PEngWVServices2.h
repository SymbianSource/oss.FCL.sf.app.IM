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
* Description:  Wireless Village CSP features.
*
*/

#ifndef __PENGWVSERVICES2_H
#define __PENGWVSERVICES2_H

//  INCLUDES
#include <E32Std.h>


// CONSTANTS

/**
 * NWSessionSlot option name.
 * Option level: KPEngNWSessionSlotTransportQuery.
 *
 * Used to query TPEngWVCSPServicesTree2 from NWSessionSlot.
 * Supported services can be asked only after succesful login,
 * when the NWSessionSlot is connected to network. If the slot
 * isn't connected to network, CPEngNWSessionSlot2::GetOption()
 * returns KErrNotFound.
 *
 * Usage:
 *      CPEngNWSessionSlot2* nwSlot = ... //Open NWSession
 *
 *      TPEngWVCspServicesTree2 cspTree;
 *      TPckg<TPEngWVCspServicesTree2> cspTreePtr( cspTree );
 *      User::LeaveIfError( nwSlot->GetOption( KPEngWVCspServicesTree2,
 *                                             KPEngNWSessionSlotTransportQuery,
 *                                             cspTreePtr ) );
 *
 *      if( !cspTree.iPresenceFeat.FeatureSupported() )
 *          {
 *          }
 *
 * @since 3.0
 */
const TUint KPEngWVCspServicesTree2 = 0x50001;





// CLASS DECLARATION


/**
 * WV CSP Feature subtree.
 * Function and SubFunction constants
 * are in the end of this file.
 *
 * @since 3.0
 */
class TPEngWVCspFeature2
    {
    public:  // Constructor

        /**
         * C++ default constructor
         */
        inline TPEngWVCspFeature2();


    public: // Accessor helpers

        /**
         * Checks is the feature supported or not.
         *
         * @return ETrue if network server supports all those functions
         *         that were requested in capability negotiation.
         *         Else EFalse.
         */
        inline TBool FeatureSupported() const;


        /**
         * Function accessor.
         * Checks is the given function supported or not.
         *
         * @param aFunction Function to check. See function constants
         *        KPEngFFServiceFunction, KPEngFFSearchFunction etc below.
         * @return ETrue if network server supports all those
         *         sub-functions that were requested in capability
         *         negotiation. Else EFalse.
         */
        inline TBool FunctionSupported( TInt aFunction ) const;


        /**
         * Sub-function accessor.
         * Checks is the given sub-function supported or not.
         *
         * @param aSubFunction Sub-function to check. See sub-function constants
         *        KPEngFFSubFuncSRCH, KPEngWVSubFuncSTSRC etc below.
         * @return ETrue if network server supports given sub-function.
         *         Else EFalse.
         */
        inline TBool SubFunctionSupported( TInt aSubFunction ) const;



    public:    // Data

        //OWN: Is feature supported?
        TBool           iSupported;

        //OWN: Supported WV functions
        TInt32          iFunctions;

        //OWN: Supported WV SubFunctions
        TInt32          iSubFunctions;


    private:    // Reserved extensions space

        //OWN: Reserved extensions space
        TUint8       iReserved[ 8 ];
    };




/**
 * WV CSP Services Tree.
 *
 * Holds information about currently supported
 * Wireless Village CSP services.
 *
 * @since 3.0
 */
class TPEngWVCspServicesTree2
    {
    public:  // Constructors

        /**
         * C++ default constructor.
         */
        inline TPEngWVCspServicesTree2();



    public:    // Data

        //OWN:  Wireless Village Fundamental Features
        TPEngWVCspFeature2            iFundamentalFeat;

        //OWN:  Wireless Village Presence Features
        TPEngWVCspFeature2            iPresenceFeat;

        //OWN:  Wireless Village IM Features
        TPEngWVCspFeature2            iIMFeat;

        //OWN:  Wireless Village Group Features
        TPEngWVCspFeature2            iGroupFeat;


    private:    // Reserved extensions space

        //OWN: Reserved extensions space
        TUint8  iReserved[4 * sizeof( TPEngWVCspFeature2 ) + 8];
    };




// CONSTANTS

/**
 * Fundamental Functions subtree.
 * @since 3.0
 */
const TInt KPEngFFServiceFunction       = 0x0001;
const TInt KPEngFFSearchFunction        = 0x0002;
// Old name
const TInt KPEngWVSearchFunction    = 0x0002;
const TInt KPEngFFInviteFunction        = 0x0004;


// SERVICE FUNCTIONS
// GETSPI: Support for Get Service Provider Info transaction
const TInt KPEngFFSubFuncGETSPI         = 0x0001;

// SEARCHE FUNCTIONS

// SRCH: Support for searching based on various group/user properties
const TInt KPEngFFSubFuncSRCH           = 0x0010;

// STSRC: Support for stop search transaction
const TInt KPEngFFSubFuncSTSRC          = 0x0020;
// Old names
const TInt KPEngWVSubFuncSRCH   = 0x0010;
const TInt KPEngWVSubFuncSTSRC  = 0x0020;


// INVITE FUNCTIONS

// INVIT: Support for invitation transaction
const TInt KPEngFFSubFuncINVIT          = 0x0100;
// CAINV: Support for cancel invitation transaction
const TInt KPEngFFSubFuncCAINV          = 0x0200;



/**
 * Presence Functions subtree.
 * @since 3.0
 */
const TInt KPEngWVContListFunction      = 0x0001;
const TInt KPEngWVPresAuthFunction      = 0x0002;
const TInt KPEngWVPresDelivFunction     = 0x0004;
const TInt KPEngWVAttrListFunction      = 0x0008;


//CONTLIST FUNCTIONS

// GCLI: Support for get list of contact lists (IDs) transaction
const TInt KPEngWVSubFuncGCLI           = 0x0001;
// CCLI: Support for create contact list transaction
const TInt KPEngWVSubFuncCCLI           = 0x0002;
// DCLI: Support for delete contact list transaction
const TInt KPEngWVSubFuncDCLI           = 0x0004;
// MCLS: Support for manage contact list transaction
const TInt KPEngWVSubFuncMCLS           = 0x0008;


//PRESAUTH FUNCTIONS

// GETWL: Support for get watcher list transaction
const TInt KPEngWVSubFuncGETWL          = 0x0010;
// REACT: Support for reactive presence authorization request
const TInt KPEngWVSubFuncREACT          = 0x0020;
// CAAUT: Support for cancel presence authorization transaction
const TInt KPEngWVSubFuncCAAUT          = 0x0040;
// GETAUT: Support for get reactive authorization status transaction
const TInt KPEngWVSubFuncGETAUT         = 0x0080;


//PRESDELIV FUNCTIONS

// GETPR: Support for get presence transaction
const TInt KPEngWVSubFuncGETPR          = 0x0100;
// UPDPR: Support for update presence transaction
const TInt KPEngWVSubFuncUPDPR          = 0x0200;


//ATTRLIST FUNCTIONS

// CALI: Support for create attribute list transaction
const TInt KPEngWVSubFuncCALI           = 0x1000;
// DALI: Support for delete attribute list transaction
const TInt KPEngWVSubFuncDALI           = 0x2000;
// GALS: Support for get attribute list transaction
const TInt KPEngWVSubFuncGALS           = 0x4000;




/**
 * IM Functions subtree.
 * @since 3.0
 */
const TInt KPEngIMSendFunctions         = 0x0001;
const TInt KPEngIMReceiveFunctions      = 0x0002;
const TInt KPEngIMAuthorFunctions       = 0x0004;

//IM SEND FUNCTIONS

// MDELIV: Support for delivery status report transaction
const TInt KPEngIMSubFuncMDELIV         = 0x0001;
// FWMSG: Support for forward message transaction
const TInt KPEngIMSubFuncFWMSG          = 0x0002;


//IM RECEIVE FUNCTIONS

// SETD: Support for setting delivery method
const TInt KPEngIMSubFuncSETD           = 0x0010;
// GETLM: Support for get list of messages transaction
const TInt KPEngIMSubFuncGETLM          = 0x0020;
// GETM: Support for get message transaction
const TInt KPEngIMSubFuncGETM           = 0x0040;
// REJCM: Support for reject message transaction
const TInt KPEngIMSubFuncREJCM          = 0x0080;
// NOTIF: Support for message notification transaction
const TInt KPEngIMSubFuncNOTIF          = 0x0100;
// NEWM: Support for new message transaction
const TInt KPEngIMSubFuncNEWM           = 0x0200;

//IM AUTHORIZATION FUNCTIONS

// GLBLU: Support for get list of blocked entities transaction
const TInt KPEngIMSubFuncGLBLU          = 0x1000;
// BLENT: Support for block entity transaction
const TInt KPEngIMSubFuncBLENT          = 0x2000;



/**
 * Group Functions subtree.
 * @since 3.0
 */
const TInt KPEngGRManageFunctions       = 0x0001;
const TInt KPEngGRUseFunctions          = 0x0002;
const TInt KPEngGRAuthorFunctions       = 0x0004;

//GROUP MANAGE FUNCTIONS

// CREAG: Support for group creation transaction
const TInt KPEngGRSubFuncCREAG          = 0x0001;
// DELGR: Support for group deletion transaction
const TInt KPEngGRSubFuncDELGR          = 0x0002;
// GETGP: Support for get group properties transaction
const TInt KPEngGRSubFuncGETGP          = 0x0004;
// SETGP: Support for set group properties transaction
const TInt KPEngGRSubFuncSETGP          = 0x0008;

//GROUP USE FUNCTIONS

// SUBGCN: Support for subscribe group notice transaction
const TInt KPEngGRSubFuncSUBGCN         = 0x0010;
// GRCHN: Support for group change notification transaction
const TInt KPEngGRSubFuncGRCHN          = 0x0020;

//GROUP AUTHORIZATION FUNCTIONS

// GETGM: Support for get group members transaction
const TInt KPEngGRSubFuncGETGM          = 0x0100;
// ADDGM: Support for add group members transaction
const TInt KPEngGRSubFuncADDGM          = 0x0200;
// RMVGM: Support for remove group members transaction
const TInt KPEngGRSubFuncRMVGM          = 0x0400;
// MBRAC: Support for member access rights transaction
const TInt KPEngGRSubFuncMBRAC          = 0x0800;
// REJEC: Support for reject user(s) from group transaction
const TInt KPEngGRSubFuncREJEC          = 0x1000;


// Inline methods
// =================== TPEngWVCSPFeature2 MEMBER FUNCTIONS ===================

// C++ default constructor can NOT contain any code, that
// might leave.
//
inline TPEngWVCspFeature2::TPEngWVCspFeature2()
    {
    Mem::FillZ( this, sizeof( TPEngWVCspFeature2 ) );
    }


// -----------------------------------------------------------------------------
// TPEngWVCspFeature2::FeatureSupported()
// -----------------------------------------------------------------------------
//
inline TBool TPEngWVCspFeature2::FeatureSupported() const
    {
    return iSupported;
    }

// -----------------------------------------------------------------------------
// TPEngWVCspFeature2::FunctionSupported()
// -----------------------------------------------------------------------------
//
inline TBool TPEngWVCspFeature2::FunctionSupported( TInt aFunction ) const
    {
    return ( iFunctions & aFunction ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// TPEngWVCspFeature2::SubFunctionSupported()
// -----------------------------------------------------------------------------
//
inline TBool TPEngWVCspFeature2::SubFunctionSupported( TInt aSubFunction ) const
    {
    return ( iSubFunctions & aSubFunction ? ETrue : EFalse );
    }



// ================= TPEngWVCspServicesTree2 MEMBER FUNCTIONS =================

// C++ default constructor can NOT contain any code, that
// might leave.
//
inline TPEngWVCspServicesTree2::TPEngWVCspServicesTree2()
    {
    Mem::FillZ( this, sizeof( TPEngWVCspServicesTree2 ) );
    }


#endif //__PENGWVSERVICES2_H

//End of file



